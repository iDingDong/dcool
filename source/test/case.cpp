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

	Case::ActiveRecord::ActiveRecord(Case::ActiveRecord::Self_ const& other_): Case::ActiveRecord(other_.snapshot()) {
	}

	Case::ActiveRecord::ActiveRecord(
		Case::Record record
	): m_checkCount_(record.checkCount), m_failures_(::dcool::core::move(record.failures)) {
	}

	auto Case::ActiveRecord::operator =(Case::ActiveRecord::Self_ const& other) -> Self_& {
		(*this) = other.snapshot();
		return *this;
	}

	auto Case::ActiveRecord::operator =(Case::Record record) -> Self_& {
		core::PhoneySharedLockGuard locker(this->m_mutex_);
		this->m_checkCount_.store(record.checkCount, ::std::memory_order::relaxed);
		this->m_failures_ = ::dcool::core::move(record.failures);
		return *this;
	}

	Case::ActiveRecord::~ActiveRecord() noexcept = default;

	auto Case::ActiveRecord::snapshot() const -> Case::Record {
		core::PhoneySharedLockGuard locker(this->m_mutex_);
		return Case::Record{
			.checkCount = this->m_checkCount_.load(::std::memory_order::consume),
			.failures = this->m_failures_
		};
	}

	void Case::ActiveRecord::recordSuccess() {
		static_cast<void>(this->m_checkCount_.fetch_add(1, ::std::memory_order::acq_rel));
	}

	void Case::ActiveRecord::recordFailure(Case::Failure failure) {
		core::PhoneyLockGuard locker(this->m_mutex_);
		static_cast<void>(this->m_checkCount_.fetch_add(1, ::std::memory_order::relaxed));
		this->m_failures_.emplaceBack(core::move(failure));
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
