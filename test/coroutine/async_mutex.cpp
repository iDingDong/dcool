#include <dcool/core.hpp>
#include <dcool/container.hpp>
#include <dcool/coroutine.hpp>
#include <dcool/test.hpp>

#include <atomic>
#include <chrono>
#include <future>

DCOOL_TEST_CASE(dcoolCoroutine, asyncMutexBasics) {
	using namespace std::literals::chrono_literals;

	class Returned {
		public: class promise_type {
			public: auto initial_suspend() const noexcept -> dcool::coroutine::SuspendNever {
				return dcool::coroutine::suspendNever;
			}

			public: auto final_suspend() const noexcept -> dcool::coroutine::SuspendNever {
				return dcool::coroutine::suspendNever;
			}

			public: auto get_return_object() -> Returned {
				return Returned();
			}

			public: void unhandled_exception() {
				::dcool::core::terminate();
			}
		};
	};

	dcool::coroutine::AsyncMutex asyncMutex;
	dcool::container::List<int> results;
	std::atomic_flag occupied;
	// We cannot capture any variables because the coroutines can be resumed after the thread ends the lifetime of this callable.
	auto coroutine = [](DCOOL_TEST_CONTEXT_PARAMETER, auto asyncMutex, auto results, auto occupied, int i) -> Returned {
		DCOOL_TEST_ASSERT(!(asyncMutex->tryLock()));
		std::this_thread::sleep_for(10ms);
		auto locker_ = co_await asyncMutex->guardedLockAsync();
		DCOOL_TEST_EXPECT(!(occupied->test_and_set()));
		std::this_thread::sleep_for(10ms);
		results->emplaceBack(i);
		std::this_thread::sleep_for(10ms);
		occupied->clear();
	};
	DCOOL_TEST_ASSERT(asyncMutex.tryLock());
	DCOOL_TEST_EXPECT(!(occupied.test_and_set()));
	results.emplaceBack(1);
	std::future<Returned> task1 = std::async(
		std::launch::async,
		coroutine,
		DCOOL_TEST_CONTEXT_ARGUMENT,
		dcool::core::addressOf(asyncMutex),
		dcool::core::addressOf(results),
		dcool::core::addressOf(occupied),
		5
	);
	std::future<Returned> task2 = std::async(
		std::launch::async,
		coroutine,
		DCOOL_TEST_CONTEXT_ARGUMENT,
		dcool::core::addressOf(asyncMutex),
		dcool::core::addressOf(results),
		dcool::core::addressOf(occupied),
		5
	);
	std::future<Returned> task3 = std::async(
		std::launch::async,
		coroutine,
		DCOOL_TEST_CONTEXT_ARGUMENT,
		dcool::core::addressOf(asyncMutex),
		dcool::core::addressOf(results),
		dcool::core::addressOf(occupied),
		5
	);
	std::this_thread::sleep_for(10ms);
	results.emplaceBack(2);
	task1.get();
	task2.get();
	task3.get();
	results.emplaceBack(3);
	std::future<Returned> finalTask = std::async(
		std::launch::async,
		coroutine,
		DCOOL_TEST_CONTEXT_ARGUMENT,
		dcool::core::addressOf(asyncMutex),
		dcool::core::addressOf(results),
		dcool::core::addressOf(occupied),
		6
	);
	finalTask.get();
	results.emplaceBack(4);
	occupied.clear();
	asyncMutex.unlock();
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(results, DCOOL_TEST_SEQUENCE(1, 2, 3, 4, 5, 5, 5, 6));
}
