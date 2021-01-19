#include <dcool/test/center.hpp>

#include <dcool/test/suite.hpp>

#include <utility>

namespace dcool::test {
	void Center::addSuite(Name suiteName_) {
		if (this->access(suiteName_) != ::dcool::core::nullPointer) {
			return;
		}
		this->m_suites_.insert(::std::make_pair(suiteName_, Suite()));
	}

	auto Center::access(Name suiteName_) -> Suite* {
		auto current = this->m_suites_.find(suiteName_);
		if (current == this->m_suites_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Center::executeAll() -> Center::Result {
		Center::Result result = {};
		result.startTime = Clock::now();
		for (auto& current: this->m_suites_) {
			Suite::Result suiteResult = current.second.executeAll();
			if (suiteResult.fatalFailureCount != 0 || suiteResult.gentleFailureCount != 0) {
				++(result.suiteStatistics.failureCount);
			} else {
				++(result.suiteStatistics.successCount);
			}
			result.caseStatistics.successCount += suiteResult.successCount;
			result.caseStatistics.gentleFailureCount += suiteResult.gentleFailureCount;
			result.caseStatistics.fatalFailureCount += suiteResult.fatalFailureCount;
			result.details.insert(::std::make_pair(current.first, suiteResult));
		}
		result.finishTime = Clock::now();
		return result;
	}

	auto Center::executeOne(Name suiteName) -> Suite::Result {
		Suite* current = this->access(suiteName);
		if (current == ::dcool::core::nullPointer) {
			throw SuiteNotFound("Suite does not exist.");
		}
		return current->executeAll();
	}

	auto Center::instance() -> Self_& {
		static Center center;
		return center;
	}

	namespace detail_ {
		int registerCase_(Name suiteName, Name caseName, Case::Executor executor) {
			Center::instance().addSuite(suiteName);
			Center::instance().access(suiteName)->addCase(caseName, executor);
			return 0;
		}
	}
}
