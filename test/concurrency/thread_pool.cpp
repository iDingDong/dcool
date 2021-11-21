#include <dcool/concurrency.hpp>
#include <dcool/test.hpp>

#include <atomic>
#include <thread>

static constexpr dcool::core::UnsignedMaxInteger repeatCount = 10000;

DCOOL_TEST_CASE(dcoolConcurrency, threadPoolBasics) {
	dcool::concurrency::Atom<dcool::core::UnsignedMaxInteger> count = 0;
	{
		auto threadPool = dcool::concurrency::ThreadPool<>(0);
		for (dcool::core::Length i = 0; i < repeatCount; ++i) {
			threadPool.enqueTask(
				[DCOOL_TEST_CAPTURE_CONTEXT, &count]() {
					dcool::core::UnsignedMaxInteger newValue = count.addFetch(1, std::memory_order::relaxed);
					DCOOL_TEST_EXPECT(newValue <= repeatCount);
				}
			);
		}
		DCOOL_TEST_ASSERT(count.load(std::memory_order::relaxed) == 0);
		threadPool.enparallel(std::jthread::hardware_concurrency());
	}
	DCOOL_TEST_ASSERT(count.load(std::memory_order::relaxed) == repeatCount);
}
