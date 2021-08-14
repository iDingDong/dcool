#include <dcool/concurrency/thread.h>

#include <dcool/core.hpp>
#include <dcool/resource.hpp>
#include <dcool/utility.hpp>

#include <chrono>
#include <thread>

namespace dcool::concurrency {
	namespace detail_ {
		namespace {
			struct CThreadResultSharedAgentConfig {
				static constexpr ::dcool::core::Boolean compact = true;
				static constexpr ::dcool::core::Boolean atomicallyCounted = true;
				static constexpr ::dcool::core::Boolean maxStrongReferenceCount = 2;
			};

			using ResultPointer = dcool::resource::SharedStrongPointer<void*, CThreadResultSharedAgentConfig>;
		}
	}
}

struct DCOOL_CONCURRENCY_DETAIL_ThreadHolder_ {
	dcool::concurrency::detail_::ResultPointer result;
	std::jthread thread;
};

namespace dcool::concurrency {
	namespace detail_ {
		static auto initializeCThread(
			DCOOL_CONCURRENCY_Thread* thread, DCOOL_CONCURRENCY_ThreadProcedure procedure, void* extraData
		) noexcept -> DCOOL_CORE_Result {
			DCOOL_CORE_ASSERT(thread != dcool::core::nullPointer);
			DCOOL_CORE_ASSERT(procedure != dcool::core::nullPointer);
			try {
				auto result = dcool::concurrency::detail_::ResultPointer(dcool::core::inPlace);
				auto createdThread = std::jthread(
					[](DCOOL_CONCURRENCY_ThreadProcedure procedure, void* extraData, dcool::concurrency::detail_::ResultPointer result) {
						*result = procedure(extraData);
					},
					procedure,
					extraData,
					result
				);
				thread->holder_ = new DCOOL_CONCURRENCY_DETAIL_ThreadHolder_ {
					.result = dcool::core::move(result),
					.thread = dcool::core::move(createdThread)
				};
			} catch (std::bad_alloc const&) {
				return DCOOL_CORE_OutOfMemory;
			} catch (...) {
				return DCOOL_CORE_Error;
			}
			return DCOOL_CORE_Ok;
		}

		static auto joinCThread(DCOOL_CONCURRENCY_Thread* thread, void** outputResult) noexcept -> DCOOL_CORE_Result {
			DCOOL_CORE_ASSERT(thread != dcool::core::nullPointer);
			DCOOL_CORE_ASSERT(outputResult != dcool::core::nullPointer);
			try {
				thread->holder_->thread.join();
			} catch (...) {
				return DCOOL_CORE_Error;
			}
			*outputResult = *(thread->holder_->result);
			delete thread->holder_;
			return DCOOL_CORE_Ok;
		}

		static auto detachCThread(DCOOL_CONCURRENCY_Thread* thread) noexcept -> DCOOL_CORE_Result {
			DCOOL_CORE_ASSERT(thread != dcool::core::nullPointer);
			try {
				thread->holder_->thread.detach();
			} catch (...) {
				return DCOOL_CORE_Error;
			}
			delete thread->holder_;
			return DCOOL_CORE_Ok;
		}

		static auto sleepCurrentCThreadUntil(DCOOL_CORE_TimePoint const* deadline) noexcept -> DCOOL_CORE_Result {
			DCOOL_CORE_ASSERT(deadline != dcool::core::nullPointer);
			try {
				std::this_thread::sleep_until(utility::toClockTimePoint(*deadline));
			} catch (...) {
				return DCOOL_CORE_Error;
			}
			return DCOOL_CORE_Ok;
		}

		static auto sleepCurrentCThreadFor(DCOOL_CORE_Duration const* duration) noexcept -> DCOOL_CORE_Result {
			try {
				std::this_thread::sleep_for(utility::toDuration(*duration));
			} catch (...) {
				return DCOOL_CORE_Error;
			}
			return DCOOL_CORE_Ok;
		}

		static auto yieldCurrentCThread() noexcept -> DCOOL_CORE_Result {
			std::this_thread::yield();
			return DCOOL_CORE_Ok;
		}
	}
}

extern "C" auto DCOOL_CONCURRENCY_initializeThread(
	DCOOL_CONCURRENCY_Thread* thread, DCOOL_CONCURRENCY_ThreadProcedure procedure, void* extraData
) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::initializeCThread(thread, procedure, extraData);
}

extern "C" auto DCOOL_CONCURRENCY_joinThread(
	DCOOL_CONCURRENCY_Thread* thread, void** outputResult
) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::joinCThread(thread, outputResult);
}

extern "C" auto DCOOL_CONCURRENCY_detachThread(DCOOL_CONCURRENCY_Thread* thread) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::detachCThread(thread);
}

extern "C" auto DCOOL_CONCURRENCY_sleepCurrentCThreadUntil(DCOOL_CORE_TimePoint const* deadline) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::sleepCurrentCThreadUntil(deadline);
}

extern "C" auto DCOOL_CONCURRENCY_sleepCurrentCThreadFor(DCOOL_CORE_Duration const* duration) -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::sleepCurrentCThreadFor(duration);
}

extern "C" auto DCOOL_CONCURRENCY_yieldCurrentThread() -> DCOOL_CORE_Result {
	return dcool::concurrency::detail_::yieldCurrentCThread();
}
