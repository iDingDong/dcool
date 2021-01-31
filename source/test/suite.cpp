#include <dcool/test/suite.hpp>

#include <utility>

namespace dcool::test {
	void Suite::setUpBeforeCase_(Name caseName) const {
		if (this->beforeCase == ::dcool::core::nullPointer) {
			return;
		}
		this->beforeCase(caseName);
	}

	void Suite::tearDownAfterCase_(Name caseName) const {
		if (this->afterCase == ::dcool::core::nullPointer) {
			return;
		}
		this->afterCase(caseName);
	}

	void Suite::addCase(Name caseName_, Case::Executor executor_) {
		if (this->getPointer(caseName_) != ::dcool::core::nullPointer) {
			::dcool::core::terminate();
		}
		this->m_cases_.insert(::std::make_pair(caseName_, Case(executor_)));
	}

	auto Suite::getPointer(Name caseName) const -> Case const* {
		auto current = this->m_cases_.find(caseName);
		if (current == this->m_cases_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Suite::getPointer(Name caseName) -> Case* {
		auto current = this->m_cases_.find(caseName);
		if (current == this->m_cases_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Suite::executeAll() const -> Suite::Result {
		return this->executeAll(::dcool::core::serialExecution);
	}

	auto Suite::executeOne(Name caseName) const -> Case::Result {
		Case const* current = this->getPointer(caseName);
		if (current == ::dcool::core::nullPointer) {
			throw CaseNotFound("Case does not exist.");
		}
		this->setUpBeforeCase_(caseName);
		Case::Result result = current->execute();
		this->tearDownAfterCase_(caseName);
		return result;
	}
}
