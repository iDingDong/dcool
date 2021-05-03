#ifndef DCOOL_COROUTINE_ASYNC_MUTEX_HPP_INCLUDED_
#	define DCOOL_COROUTINE_ASYNC_MUTEX_HPP_INCLUDED_ 1

#	include <dcool/coroutine/basic.hpp>

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <atomic>

namespace dcool::coroutine {
	class AsyncMutex {
		private: using Self_ = AsyncMutex;

		private: class LockAwaiter_ {
			public: friend ::dcool::coroutine::AsyncMutex;

			protected: ::dcool::coroutine::AsyncMutex* m_asyncMutex_;
			private: ::dcool::coroutine::UniversalHandle handle_;
			private: void* next_;

			private: LockAwaiter_(
				::dcool::coroutine::AsyncMutex& asyncMutex
			) noexcept: m_asyncMutex_(::dcool::core::addressOf(asyncMutex)) {
			}

			public: auto await_ready() const noexcept -> ::dcool::core::Boolean {
				return this->m_asyncMutex_->tryLock();
			}

			public: auto await_suspend(::dcool::coroutine::UniversalHandle handle_) noexcept -> ::dcool::core::Boolean {
				this->handle_ = handle_;
				return !(this->m_asyncMutex_->lockOrQueue(*this));
			}

			public: void await_resume() noexcept {
			}
		};

		private: class GuardedLockAwaiter_: public LockAwaiter_ {
			private: using LockAwaiter_::LockAwaiter_;

			public: auto await_resume() const noexcept {
				return ::dcool::resource::makeUniqueGuard(*(this->m_asyncMutex_), [](::dcool::coroutine::AsyncMutex& asyncMutex_) {
					asyncMutex_.unlock();
				});
			}
		};

		public: friend LockAwaiter_;

		private: void* m_latestEnqued_ = ::dcool::core::nullPointer;
		private: LockAwaiter_* m_nextInQueue_;
		private: ::dcool::core::MinimalMutex m_mutex_;

		public: auto lockAsync() noexcept -> LockAwaiter_ {
			return LockAwaiter_(*this);
		}

		public: auto guardedLockAsync() noexcept -> GuardedLockAwaiter_ {
			return GuardedLockAwaiter_(*this);
		}

		public: auto tryLock() noexcept -> ::dcool::core::Boolean;
		public: void unlock() noexcept;

		private: auto lockOrQueue(LockAwaiter_& awaited_) noexcept -> ::dcool::core::Boolean;
	};
}

#endif
