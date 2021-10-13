#include <dcool/concurrency.hpp>
#include <dcool/container.hpp>
#include <dcool/test.hpp>

#include <bit>
#include <chrono>
#include <thread>

static constexpr dcool::core::UnsignedMaxInteger repeatCount = 10000;

template <typename ConfigT = dcool::core::Empty<>> static void testAtomBasics(DCOOL_TEST_CONTEXT_PARAMETER) {
	using Atom = dcool::concurrency::Atom<dcool::core::UnsignedMaxInteger, ConfigT>;
	Atom atom = 0;
	for (dcool::core::UnsignedMaxInteger i = 0; i < repeatCount; ++i) {
		if constexpr (Atom::stampWidth >= std::bit_width(repeatCount)) {
			auto stamped = atom.sequencedLoadStamped();
			DCOOL_TEST_EXPECT(stamped.value == i);
			DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
			stamped = atom.sequencedStampedFetchAddStamped(1);
			DCOOL_TEST_EXPECT(stamped.value == i);
			DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
			stamped = atom.sequencedLoadStamped();
			DCOOL_TEST_EXPECT(stamped.value == i + 1);
			DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
		} else {
			DCOOL_TEST_EXPECT(atom.load() == i);
			DCOOL_TEST_EXPECT(atom.sequencedFetchAdd(1) == i);
			DCOOL_TEST_EXPECT(atom.load() == i + 1);
		}
	}
	DCOOL_TEST_EXPECT(atom.load() == repeatCount);
	if constexpr (!(Atom::atomic.isDeterminateFalse())) {
		unsigned const concurrencyLimit = dcool::core::min(std::jthread::hardware_concurrency(), 2U);
		dcool::concurrency::Latch<> taskBlocker(1);
		dcool::concurrency::Semaphore<> mainBlocker;
		auto task = [DCOOL_TEST_CAPTURE_CONTEXT, &taskBlocker, &mainBlocker, &atom]() {
			taskBlocker.wait();
			dcool::core::UnsignedMaxInteger lastValue = atom.atomicallyLoad();
			DCOOL_TEST_EXPECT(lastValue >= repeatCount);
			for (dcool::core::UnsignedMaxInteger i = 0; i < repeatCount; ++i) {
				if constexpr (Atom::stampWidth >= std::bit_width(repeatCount)) {
					auto stamped = atom.atomicallyLoadStamped();
					DCOOL_TEST_EXPECT(stamped.value >= lastValue);
					DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
					lastValue = stamped.value;
					stamped = atom.atomicallyStampedFetchAddStamped(1);
					DCOOL_TEST_EXPECT(stamped.value >= lastValue);
					DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
					stamped = atom.atomicallyLoadStamped();
					DCOOL_TEST_EXPECT(stamped.value > lastValue);
					DCOOL_TEST_EXPECT(stamped.stamp == stamped.value);
				} else {
					dcool::core::UnsignedMaxInteger newValue = atom.atomicallyLoad();
					DCOOL_TEST_EXPECT(newValue >= lastValue);
					lastValue = newValue;
					newValue = atom.atomicallyFetchAdd(1);
					DCOOL_TEST_EXPECT(newValue >= lastValue);
					lastValue = newValue;
					newValue = atom.atomicallyLoad();
					DCOOL_TEST_EXPECT(newValue > lastValue);
					lastValue = newValue;
				}
			}
			mainBlocker.release();
		};
		dcool::container::List<std::jthread> tasks;
		for (unsigned i = 0; i < concurrencyLimit; ++i) {
			tasks.pushBack(std::jthread(task));
		}
		taskBlocker.countDown();
		for (unsigned i = 0; i < concurrencyLimit; ++i) {
			mainBlocker.acquire();
			DCOOL_TEST_EXPECT(atom.load() >= repeatCount * (i + 1));
		}
		DCOOL_TEST_EXPECT(atom.load() == repeatCount * (concurrencyLimit + 1));
		for (std::jthread& task: tasks) {
			task.join();
		}
		DCOOL_TEST_EXPECT(atom.load() == repeatCount * (concurrencyLimit + 1));
	}
}

struct StampedAtomConfig {
	static constexpr dcool::core::Length stampWidth = 64;
};

struct FlexibleAtomConfig {
	static constexpr dcool::core::Triboolean atomic = dcool::core::indeterminate;
};

struct FlexibleStampedAtomConfig {
	static constexpr dcool::core::Length stampWidth = 64;
	static constexpr dcool::core::Triboolean atomic = dcool::core::indeterminate;
};

struct PhoneyAtomConfig {
	static constexpr dcool::core::Triboolean atomic = dcool::core::determinateFalse;
};

struct PhoneyStampedAtomConfig {
	static constexpr dcool::core::Length stampWidth = 64;
	static constexpr dcool::core::Triboolean atomic = dcool::core::determinateFalse;
};

DCOOL_TEST_CASE(dcoolConcurrency, atomBasics) {
	testAtomBasics(DCOOL_TEST_CONTEXT_ARGUMENT);
	testAtomBasics<StampedAtomConfig>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testAtomBasics<FlexibleAtomConfig>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testAtomBasics<FlexibleStampedAtomConfig>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testAtomBasics<PhoneyAtomConfig>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testAtomBasics<PhoneyStampedAtomConfig>(DCOOL_TEST_CONTEXT_ARGUMENT);
}
