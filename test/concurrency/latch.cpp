#include <dcool/core.hpp>
#include <dcool/concurrency.hpp>
#include <dcool/test.hpp>

#include <atomic>
#include <chrono>
#include <thread>

DCOOL_TEST_CASE(dcoolConcurrency, latchBasics) {
	using namespace std::literals::chrono_literals;

	std::atomic<dcool::core::Length> preCount = 0;
	std::atomic<dcool::core::Length> postCount = 0;
	dcool::concurrency::TimedLatch latch(3);

	std::jthread task1([&dcoolTestRecord, &preCount, &postCount, &latch]() {
		++preCount;
		DCOOL_TEST_EXPECT(postCount == 0);
		latch.arriveAndWait();
		DCOOL_TEST_EXPECT(preCount == 3);
		++postCount;
	});
	DCOOL_TEST_EXPECT(!(latch.tryWait()));
	DCOOL_TEST_EXPECT(!(latch.tryWaitFor(10ms)));
	DCOOL_TEST_EXPECT(postCount == 0);
	std::jthread task2([&dcoolTestRecord, &preCount, &postCount, &latch]() {
		++preCount;
		DCOOL_TEST_EXPECT(postCount == 0);
		latch.arriveAndWait();
		DCOOL_TEST_EXPECT(preCount == 3);
		++postCount;
	});
	DCOOL_TEST_EXPECT(postCount == 0);
	std::jthread task3([&dcoolTestRecord, &preCount, &postCount, &latch]() {
		++preCount;
		DCOOL_TEST_EXPECT(postCount == 0);
		latch.countDown();
		latch.wait();
		DCOOL_TEST_EXPECT(preCount == 3);
		++postCount;
	});
	task1.join();
	task2.join();
	task3.join();
	DCOOL_TEST_EXPECT(preCount == 3);
	DCOOL_TEST_EXPECT(postCount == 3);
	DCOOL_TEST_EXPECT(latch.tryWait());
	DCOOL_TEST_EXPECT(latch.tryWaitFor(10ms));
	DCOOL_TEST_EXPECT(preCount == 3);
	DCOOL_TEST_EXPECT(postCount == 3);
}
