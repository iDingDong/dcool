#include <dcool/concurrency/mutex.h>

#include <dcool/core.h>
#include <dcool/utility.hpp>

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>

namespace dcool::concurrency {
	namespace detail_ {
		namespace {
			struct AnyMutexConfig {
				using SupportedTimePoints = dcool::core::Types<std::chrono::system_clock::time_point>;
			};

			using CMutexUnderlying = dcool::utility::AnyMutex<dcool::concurrency::detail_::AnyMutexConfig>;
		}
	}
}

struct DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
	dcool::concurrency::detail_::CMutexUnderlying mutex;
};

namespace dcool::concurrency {
	namespace detail_ {
		static auto initializeCMutex(
			DCOOL_CONCURRENCY_Mutex* mutex, DCOOL_CONCURRENCY_MutexAttribute attribute
		) noexcept -> DCOOL_CORE_Result {
			try {
				if (
					DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexShareable) &&
					DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexRecursive)
				) {
					return DCOOL_CORE_resultUnsupported;
				}
				if (DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexTimed)) {
					if (DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexShareable)) {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::shared_timed_mutex>)
						};
					} else if (DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexRecursive)) {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::recursive_timed_mutex>)
						};
					} else {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::timed_mutex>)
						};
					}
				} else {
					if (DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexShareable)) {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::shared_mutex>)
						};
					} else if (DCOOL_CORE_FLAG_ENABLED(attribute, DCOOL_CONCURRENCY_mutexRecursive)) {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::recursive_mutex>)
						};
					} else {
						mutex->holder_ = new DCOOL_CONCURRENCY_DETAIL_MutexHolder_ {
							.mutex = CMutexUnderlying(core::typedInPlace<std::mutex>)
						};
					}
				}
			} catch (std::bad_alloc const&) {
				return DCOOL_CORE_resultOutOfMemory;
			} catch (...) {
				return DCOOL_CORE_resultUnknownError;
			}
			return DCOOL_CORE_resultOk;
		}

		static auto uninitializeCMutex(DCOOL_CONCURRENCY_Mutex* mutex) noexcept -> DCOOL_CORE_Result {
			delete mutex->holder_;
			return DCOOL_CORE_resultOk;
		}

		static auto lockCMutex(DCOOL_CONCURRENCY_Mutex* mutex_) noexcept -> DCOOL_CORE_Result {
			try {
				mutex_->holder_->mutex.lock();
			} catch (...) {
				return DCOOL_CORE_resultUnknownError;
			}
			return DCOOL_CORE_resultOk;
		}

		static auto tryLockCMutex(DCOOL_CONCURRENCY_Mutex* mutex_) noexcept -> DCOOL_CORE_Result {
			try {
				if (!(mutex_->holder_->mutex.tryLock())) {
					return DCOOL_CORE_resultBusy;
				}
			} catch (...) {
				return DCOOL_CORE_resultUnknownError;
			}
			return DCOOL_CORE_resultOk;
		}

		static auto tryLockCMutexUntil(
			DCOOL_CONCURRENCY_Mutex* mutex, DCOOL_CORE_TimePoint const* time
		) noexcept -> DCOOL_CORE_Result {
			try {
				if (!(mutex->holder_->mutex.tryLockUntil(core::toClockTimePoint(*time)))) {
					return DCOOL_CORE_resultBusy;
				}
			} catch (...) {
				return DCOOL_CORE_resultUnknownError;
			}
			return DCOOL_CORE_resultOk;
		}

		static auto unlockCMutex(DCOOL_CONCURRENCY_Mutex* mutex_) noexcept -> DCOOL_CORE_Result {
			try {
				mutex_->holder_->mutex.unlock();
			} catch (...) {
				return DCOOL_CORE_resultUnknownError;
			}
			return DCOOL_CORE_resultOk;
		}
	}
}

extern "C" auto DCOOL_CONCURRENCY_initializeMutex(
	DCOOL_CONCURRENCY_Mutex* mutex, DCOOL_CONCURRENCY_MutexAttribute attribute
) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::initializeCMutex(mutex, attribute);
}

extern "C" auto DCOOL_CONCURRENCY_uninitializeMutex(DCOOL_CONCURRENCY_Mutex* mutex) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::uninitializeCMutex(mutex);
}

extern "C" auto DCOOL_CONCURRENCY_lockMutex(DCOOL_CONCURRENCY_Mutex* mutex) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::lockCMutex(mutex);
}

extern "C" auto DCOOL_CONCURRENCY_tryLockMutex(DCOOL_CONCURRENCY_Mutex* mutex) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::tryLockCMutex(mutex);
}

extern "C" auto DCOOL_CONCURRENCY_tryLockMutexUntil(
	DCOOL_CONCURRENCY_Mutex* mutex, DCOOL_CORE_TimePoint const* time
) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::tryLockCMutexUntil(mutex, time);
}

extern "C" auto DCOOL_CONCURRENCY_unlockMutex(DCOOL_CONCURRENCY_Mutex* mutex) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::unlockCMutex(mutex);
}
