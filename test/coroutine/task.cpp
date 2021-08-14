#include <dcool/core.hpp>
#include <dcool/coroutine.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolCoroutine, taskBasics) {
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

	int step = 0;
	auto coroutineTask = [DCOOL_TEST_CAPTURE_CONTEXT, &step]() -> dcool::coroutine::Task<int> {
		DCOOL_TEST_EXPECT(step == 1);
		step = 2;
		co_return -1;
	};
	auto task = coroutineTask();
	DCOOL_TEST_EXPECT(step == 0);
	auto coroutine = [DCOOL_TEST_CAPTURE_CONTEXT, &task, &step]() -> Returned {
		DCOOL_TEST_EXPECT(step == 0);
		step = 1;
		DCOOL_TEST_EXPECT((co_await task) == -1);
		DCOOL_TEST_EXPECT(step == 2);
		step = 3;
	};
	coroutine();
	DCOOL_TEST_EXPECT(step == 3);
}

DCOOL_TEST_CASE(dcoolCoroutine, syncWaitedTask) {
	auto taskCoroutine = []() -> dcool::coroutine::Task<int> {
		co_return 3;
	};
	auto task = taskCoroutine();
	DCOOL_TEST_EXPECT(dcool::coroutine::syncWait(task) == 3);
}
