#include <dcool/service.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <thread>

DCOOL_TEST_CASE(dcoolService, quickTimerBasics) {
	using namespace std::literals::chrono_literals;
	using QuickTimer = dcool::service::QuickTimer<>;
	QuickTimer quickTimer(
		100ms,
		[DCOOL_TEST_CAPTURE_CONTEXT]() -> QuickTimer::ActionResult {
			DCOOL_TEST_EXPECT(false);
			return QuickTimer::ActionResult::done;
		}
	);
	quickTimer.stop();
	quickTimer.waitUntilDone();
}
