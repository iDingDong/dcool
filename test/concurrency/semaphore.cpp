#include <dcool/core.hpp>
#include <dcool/concurrency.hpp>
#include <dcool/test.hpp>

#include <atomic>
#include <chrono>
#include <future>

DCOOL_TEST_CASE(dcoolConcurrency, semaphoreBasics) {
	using namespace std::literals::chrono_literals;

	constexpr dcool::core::Length testCount = 5;

	std::atomic<dcool::core::Length> preCount = 0;
	std::atomic<dcool::core::Length> postCount = 0;
	dcool::concurrency::TimedSemaphore semaphore;

	auto checker = [DCOOL_TEST_CAPTURE_CONTEXT, &preCount, &postCount] {
		auto postCountValue = postCount.load();
		auto preCountValue = preCount.load();
		DCOOL_TEST_EXPECT(postCountValue <= preCountValue);
	};

	std::future<void> consumer = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &preCount, &postCount, &semaphore, checker]() {
			for (dcool::core::Length i = 0; i < testCount; ++i) {
				checker();
				semaphore.acquire();
				checker();
				auto newPostCountValue = ++postCount;
				DCOOL_TEST_EXPECT(newPostCountValue <= preCount);
			}
		}
	);

	DCOOL_TEST_ASSERT(!(semaphore.tryAcquire()));
	DCOOL_TEST_ASSERT(!(semaphore.tryAcquireFor(10ms)));
	DCOOL_TEST_EXPECT(postCount == 0);

	std::future<void> producer = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &preCount, &postCount, &semaphore, checker]() {
			for (dcool::core::Length i = 0; i < testCount; ++i) {
				checker();
				auto newPreCountValue = ++preCount;
				DCOOL_TEST_EXPECT(postCount < newPreCountValue);
				semaphore.release();
				checker();
				std::this_thread::sleep_for(20ms);
				checker();
			}
		}
	);
	checker();
	consumer.get();
	DCOOL_TEST_EXPECT(preCount == testCount);
	DCOOL_TEST_EXPECT(postCount == testCount);
	producer.get();
	DCOOL_TEST_EXPECT(preCount == testCount);
	DCOOL_TEST_EXPECT(postCount == testCount);
}
