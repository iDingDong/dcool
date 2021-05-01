#include <dcool/test/case.hpp>

#include <dcool/core.hpp>

namespace dcool::test {
	namespace {
		class FatalCaseFailure: public ::std::runtime_error {
			private: using Super_ = ::std::runtime_error;

			public: using Super_::Super_;
		};
	}

	Case::Case(Case::Executor executor) noexcept: m_executor_(executor) {
	}

	auto Case::execute() const -> Case::Result {
		Result result = {};
		result.startTime = Clock::now();
		try {
			Case::ActiveRecord activeRecord;
			this->m_executor_(activeRecord);
			result.record = activeRecord.snapshot();
			result.endedByFatal = false;
		} catch (FatalCaseFailure const&) {
			result.endedByFatal = true;
		}
		result.finishTime = Clock::now();
		return result;
	}

	Case::ActiveRecord::ActiveRecord() noexcept = default;

	Case::ActiveRecord::ActiveRecord(Case::ActiveRecord::Self_ const& other_) {
		core::PhoneySharedLockGuard locker(other_.m_mutex_);
		this->m_record_ = other_.m_record_;
	}

	Case::ActiveRecord::ActiveRecord(Case::ActiveRecord::Self_&& other_) noexcept {
		core::PhoneyLockGuard locker(other_.m_mutex_);
		this->m_record_ = core::move(other_.m_record_);
	}

	auto Case::ActiveRecord::operator =(Case::ActiveRecord::Self_ const& other_) -> Self_& {
		Case::ActiveRecord middleMan_(other_);
		{
			core::PhoneyLockGuard locker(this->m_mutex_);
			core::intelliSwap(this->m_record_, middleMan_.m_record_);
		}
		return *this;
	}

	auto Case::ActiveRecord::operator =(Case::ActiveRecord::Self_&& other_) noexcept -> Self_& {
		Case::ActiveRecord middleMan_(::dcool::core::move(other_));
		{
			core::PhoneyLockGuard locker(this->m_mutex_);
			core::intelliSwap(this->m_record_, middleMan_.m_record_);
		}
		return *this;
	}

	Case::ActiveRecord::~ActiveRecord() noexcept = default;

	auto Case::ActiveRecord::snapshot() -> Case::Record {
		core::PhoneySharedLockGuard locker(this->m_mutex_);
		return this->m_record_;
	}

	void Case::ActiveRecord::recordSuccess() {
		core::PhoneyLockGuard locker(this->m_mutex_);
		++(this->m_record_.checkCount);
	}

	void Case::ActiveRecord::recordFailure(Case::Failure failure) {
		core::PhoneyLockGuard locker(this->m_mutex_);
		++(this->m_record_.checkCount);
		this->m_record_.failures.emplaceBack(core::move(failure));
	}

	namespace detail_ {
		void check_(
			FileName fileName, LineNumber lineNumber, Case::Failure::Level level, core::Boolean predicate, Case::ActiveRecord& record
		) {
			if (predicate) {
				record.recordSuccess();
			} else {
				record.recordFailure(Case::Failure { .fileName = fileName, .lineNumber = lineNumber, .time = Clock::now() });
				if (level == Case::Failure::Level::fatalForCase) {
					throw FatalCaseFailure("Case encoutered fatal failure.");
				}
			}
		}
	}
}
