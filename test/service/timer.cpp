#include <dcool/concurrency.hpp>
#include <dcool/service.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <thread>

DCOOL_TEST_CASE(dcoolService, quickTimerBasics) {
	using namespace std::literals::chrono_literals;
	using QuickTimer = dcool::service::QuickTimer<>;
	dcool::concurrency::AtomicFlag flag;
	QuickTimer quickTimer(
		10ms,
		[DCOOL_TEST_CAPTURE_CONTEXT, &flag]() -> QuickTimer::ActionResult {
			DCOOL_TEST_EXPECT(!(flag.testAndSet()));
			return QuickTimer::ActionResult::done;
		}
	);
	quickTimer.waitUntilDone();
	DCOOL_TEST_EXPECT(flag.testAndSet());
}

DCOOL_TEST_CASE(dcoolService, quickTimerStop) {
	using namespace std::literals::chrono_literals;
	using QuickTimer = dcool::service::QuickTimer<>;
	QuickTimer quickTimer(
		10ms,
		[DCOOL_TEST_CAPTURE_CONTEXT]() -> QuickTimer::ActionResult {
			DCOOL_TEST_EXPECT(false);
			return QuickTimer::ActionResult::done;
		}
	);
	quickTimer.stop();
	quickTimer.waitUntilDone();
}
