#include <dcool/test/suite.hpp>

#include <dcool/test/center.hpp>

#include <utility>

namespace dcool::test {
	void Suite::addCase(Name caseName_, Case::Executor executor_) {
		if (this->access(caseName_) != ::dcool::core::nullPointer) {
			::dcool::core::terminate();
		}
		this->m_cases_.insert(::std::make_pair(caseName_, Case(executor_)));
	}

	auto Suite::access(Name caseName) -> Case* {
		auto current = this->m_cases_.find(caseName);
		if (current == this->m_cases_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Suite::executeAll() -> Suite::Result {
		Suite::Result result = {};
		result.startTime = Clock::now();
		for (auto& currentCase: this->m_cases_) {
			Case::Result caseResult = currentCase.second.execute();
			if (caseResult.record.failures.vacant()) {
				++(result.successCount);
			} else if (caseResult.endedByFatal) {
				++(result.fatalFailureCount);
			} else {
				++(result.gentleFailureCount);
			}
			result.details.insert(std::pair{ currentCase.first, caseResult });
		}
		result.finishTime = Clock::now();
		return result;
	}

	auto Suite::executeOne(Name caseName) -> Case::Result {
		Case* current = this->access(caseName);
		if (current == ::dcool::core::nullPointer) {
			throw CaseNotFound("Case does not exist.");
		}
		return current->execute();
	}
}
