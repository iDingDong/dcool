#include <dcool/test/center.hpp>

#include <dcool/test/suite.hpp>

#include <dcool/core.hpp>

#include <utility>

namespace dcool::test {
	void Center::setUpBeforeFullExecution_() const {
		if (this->beforeFullExecution == ::dcool::core::nullPointer) {
			return;
		}
		this->beforeFullExecution();
	}

	void Center::tearDownAfterFullExecution_() const {
		if (this->afterFullExecution == ::dcool::core::nullPointer) {
			return;
		}
		this->afterFullExecution();
	}

	void Center::setUpBeforeSuite_(Name suiteName) const {
		if (this->beforeSuite == ::dcool::core::nullPointer) {
			return;
		}
		this->beforeSuite(suiteName);
	}

	void Center::tearDownAfterSuite_(Name suiteName) const {
		if (this->afterSuite == ::dcool::core::nullPointer) {
			return;
		}
		this->afterSuite(suiteName);
	}

	void Center::addSuite(Name suiteName) {
		this->m_suites_.insert(::std::make_pair(suiteName, Suite()));
	}

	auto Center::getPointer(Name suiteName) const -> Suite const* {
		auto current = this->m_suites_.find(suiteName);
		if (current == this->m_suites_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Center::getPointer(Name suiteName) -> Suite* {
		auto current = this->m_suites_.find(suiteName);
		if (current == this->m_suites_.end()) {
			return ::dcool::core::nullPointer;
		}
		return ::dcool::core::addressOf(current->second);
	}

	auto Center::forceAccess(Name suiteName) -> Suite& {
		return this->m_suites_[suiteName];
	}

	auto Center::executeAll() const -> Center::Result {
		return this->executeAll(::dcool::core::serialExecution);
	}

	auto Center::executeOne(Name suiteName) const -> Suite::Result {
		Suite const* current = this->getPointer(suiteName);
		if (current == ::dcool::core::nullPointer) {
			throw SuiteNotFound("Suite does not exist.");
		}
		this->setUpBeforeSuite_(suiteName);
		Suite::Result result = current->executeAll(suiteName);
		this->tearDownAfterSuite_(suiteName);
		return result;
	}

	auto Center::instance() -> Center& {
		static Center center;
		return center;
	}

	namespace detail_ {
		int registerCase_(Name suiteName, Name caseName, Case::Executor executor) {
			Center::instance().forceAccess(suiteName).addCase(caseName, executor);
			return 0;
		}
	}
}
