#include <dcool/coroutine/async_mutex.hpp>

namespace dcool::coroutine {
	namespace detail_ {
		static char lockedSentry;
		constexpr void* lockedSentryAddress = core::addressOf(lockedSentry);
	}

	auto AsyncMutex::tryLock() noexcept -> core::Boolean {
		core::PhoneyLockGuard locker(this->m_mutex_);
		if (this->m_latestEnqued_ != core::nullPointer) {
			return false;
		}
		this->m_latestEnqued_ = detail_::lockedSentryAddress;
		return true;
	}

	auto AsyncMutex::lockOrQueue(AsyncMutex::LockAwaiter_& awaited_) noexcept -> core::Boolean {
		core::PhoneyLockGuard locker(this->m_mutex_);
		if (this->m_latestEnqued_ == core::nullPointer) {
			this->m_latestEnqued_ = detail_::lockedSentryAddress;
			return true;
		}
		awaited_.next_ = detail_::lockedSentryAddress;
		if (this->m_latestEnqued_ == detail_::lockedSentryAddress) {
			this->m_nextInQueue_ = core::addressOf(awaited_);
		} else {
			static_cast<AsyncMutex::LockAwaiter_*>(this->m_latestEnqued_)->next_ = core::addressOf(awaited_);
		}
		this->m_latestEnqued_ = core::addressOf(awaited_);
		return false;
	}

	void AsyncMutex::unlock() noexcept {
		AsyncMutex::LockAwaiter_* toResume;
		{
			core::PhoneyLockGuard locker(this->m_mutex_);
			DCOOL_CORE_ASSERT(this->m_latestEnqued_ != core::nullPointer);
			if (this->m_latestEnqued_ == detail_::lockedSentryAddress) {
				this->m_latestEnqued_ = core::nullPointer;
				return;
			}
			toResume = this->m_nextInQueue_;
			if (this->m_nextInQueue_ == this->m_latestEnqued_) {
				this->m_latestEnqued_ = detail_::lockedSentryAddress;
			} else {
				this->m_nextInQueue_ = static_cast<AsyncMutex::LockAwaiter_*>(toResume->next_);
			}
		}
		toResume->handle_.resume();
	}
}
