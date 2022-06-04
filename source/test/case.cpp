#include <dcool/test/case.hpp>

#include <dcool/core.hpp>

#include <source_location>

namespace dcool::test {
	namespace {
		class FatalCaseFailure: public ::std::runtime_error {
			private: using Super_ = ::std::runtime_error;

			public: using Super_::Super_;
		};
	}

	Case::Case(Case::Executor executor) noexcept: m_executor_(executor) {
	}

	auto Case::execute(Name suiteName, Name caseName) const -> Case::Result {
		Result result = {};
		result.startTime = Clock::now();
		try {
			Case::Context context = {
				.suiteName = core::move(suiteName),
				.caseName = core::move(caseName)
			};
			this->m_executor_(context);
			result.record = context.activeRecord.snapshot();
			result.endedByFatal = false;
		} catch (FatalCaseFailure const&) {
			result.endedByFatal = true;
		}
		result.finishTime = Clock::now();
		return result;
	}

	Case::ActiveRecord::ActiveRecord() noexcept = default;

	// Case::ActiveRecord::ActiveRecord(Case::ActiveRecord const& other_): Case::ActiveRecord(other_.snapshot()) {
	// }

	// Case::ActiveRecord::ActiveRecord(
	// 	Case::Record record
	// ): m_checkCount_(record.checkCount), m_failures_(::dcool::core::move(record.failures)) {
	// }

	// auto Case::ActiveRecord::operator =(Case::ActiveRecord const& other) -> Case::ActiveRecord& {
	// 	(*this) = other.snapshot();
	// 	return *this;
	// }

	// auto Case::ActiveRecord::operator =(Case::Record record) -> Case::ActiveRecord& {
	// 	core::PhoneySharedLockGuard locker(this->m_mutex_);
	// 	this->m_checkCount_.store(record.checkCount, ::std::memory_order::relaxed);
	// 	this->m_failures_ = ::dcool::core::move(record.failures);
	// 	return *this;
	// }

	Case::ActiveRecord::~ActiveRecord() noexcept = default;

	auto Case::ActiveRecord::snapshot() const -> Case::Record {
		core::PhoneySharedLockGuard locker(this->m_mutex_);
		return Case::Record {
			.checkCount = this->m_checkCount_.load(::std::memory_order::consume),
			.failures = this->m_failures_
		};
	}

	void Case::ActiveRecord::recordSuccess() {
		static_cast<void>(this->m_checkCount_.fetch_add(1, ::std::memory_order::release));
	}

	void Case::ActiveRecord::recordFailure(Case::Failure failure) {
		core::PhoneyLockGuard locker(this->m_mutex_);
		static_cast<void>(this->m_checkCount_.fetch_add(1, ::std::memory_order::relaxed));
		this->m_failures_.emplaceBack(core::move(failure));
	}

	namespace detail_ {
		void check_(
			Case::Context& context, Case::Failure::Level level, core::Boolean predicate, const std::source_location& location
		) {
			if (predicate) {
				context.activeRecord.recordSuccess();
			} else {
				context.activeRecord.recordFailure(Case::Failure { .location = location, .time = Clock::now() });
				if (level == Case::Failure::Level::fatalForCase) {
					throw FatalCaseFailure("Case encoutered fatal failure.");
				}
			}
		}
	}
}
