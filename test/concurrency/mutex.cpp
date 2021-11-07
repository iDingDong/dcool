#include <dcool/concurrency.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <future>

template <typename MutexT> static void testMutexCommon(DCOOL_TEST_CONTEXT_PARAMETER) {
	using Locker = dcool::concurrency::Locker<MutexT>;
	MutexT mutex;
	dcool::concurrency::Latch<> mainBlocker(1);
	Locker locker(mutex, Locker::Status::idle);
	dcool::core::Boolean attempResult = locker.tryLock();
	DCOOL_TEST_EXPECT(attempResult);
	if (!attempResult) {
		return;
	}
	std::future<void> task1 = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &mutex, &mainBlocker]() {
			Locker locker(mutex, Locker::Status::idle);
			DCOOL_TEST_EXPECT(!(locker.tryLock()));
			mainBlocker.countDown();
			locker.lock();
			locker.unlock();
			DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
		}
	);
	mainBlocker.wait();
	locker.unlock();
	task1.get();
	DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
}

template <typename MutexT> static void testMutexShare(DCOOL_TEST_CONTEXT_PARAMETER) {
	using Locker = dcool::concurrency::Locker<MutexT>;
	using namespace std::literals::chrono_literals;
	MutexT mutex;
	dcool::core::Boolean produced = false;
	dcool::concurrency::Latch<> mainBlocker(1);
	dcool::concurrency::Latch<> task2Blocker(1);
	Locker locker(mutex, Locker::Status::idle);
	DCOOL_TEST_ASSERT(locker.tryLockShared());
	std::future<void> task1 = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &mutex, &produced, &task2Blocker]() {
			Locker locker(mutex, Locker::Status::idle);
			DCOOL_TEST_ASSERT(!(locker.tryLock()));
			task2Blocker.countDown();
			locker.lock();
			produced = true;
			locker.unlock();
			DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
		}
	);
	std::future<void> task2 = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &mutex, &produced, &mainBlocker, &task2Blocker]() {
			Locker locker(mutex, Locker::Status::idle);
			task2Blocker.wait();
			std::this_thread::sleep_for(10ms);
			mainBlocker.countDown();
			if (locker.tryLockShared()) {
				DCOOL_TEST_ASSERT(!(MutexT::preferExclusive.isDeterminateTrue()));
				DCOOL_TEST_EXPECT(!produced);
			} else {
				DCOOL_TEST_ASSERT(!(MutexT::preferExclusive.isDeterminateFalse()));
				locker.lockShared();
				DCOOL_TEST_EXPECT(produced);
			}
			locker.unlockShared();
			DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
		}
	);
	mainBlocker.wait();
	std::this_thread::sleep_for(10ms);
	DCOOL_TEST_EXPECT(!produced);
	locker.unlockShared();
	task1.get();
	task2.get();
	DCOOL_TEST_EXPECT(produced);
	DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
}

template <typename MutexT> static void testMutexFlexibility(DCOOL_TEST_CONTEXT_PARAMETER) {
	using Locker = dcool::concurrency::Locker<MutexT>;
	using namespace std::literals::chrono_literals;
	MutexT mutex;
	dcool::core::Boolean readed = false;
	dcool::concurrency::Latch<> mainBlocker1(1);
	dcool::concurrency::Latch<> mainBlocker2(1);
	dcool::concurrency::Latch<> task1Blocker(1);
	Locker locker(mutex, Locker::Status::idle);
	locker.lock();
	std::future<void> task1 = std::async(
		std::launch::async,
		[DCOOL_TEST_CAPTURE_CONTEXT, &mutex, &readed, &mainBlocker1, &mainBlocker2, &task1Blocker]() {
			Locker locker(mutex, Locker::Status::idle);
			DCOOL_TEST_ASSERT(!(locker.tryLockShared()));
			mainBlocker1.countDown();
			locker.lockShared();
			mainBlocker2.countDown();
			readed = true;
			task1Blocker.wait();
			locker.unlockShared();
			DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
		}
	);
	DCOOL_TEST_EXPECT(!readed);
	mainBlocker1.wait();
	DCOOL_TEST_EXPECT(!readed);
	//std::this_thread::sleep_for(10ms);
	locker.downgrade();
	mainBlocker2.wait();
	DCOOL_TEST_ASSERT(!(locker.tryUpgrade()));
	task1Blocker.countDown();
	locker.upgrade();
	locker.unlock();
	task1.get();
	DCOOL_TEST_EXPECT(readed);
	DCOOL_CORE_ASSERT(locker.status() == Locker::Status::idle);
}

DCOOL_TEST_CASE(dcoolConcurrency, mutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct SpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
};

DCOOL_TEST_CASE(dcoolConcurrency, spinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<SpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct SharedMutexConfig {
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
};

DCOOL_TEST_CASE(dcoolConcurrency, sharedMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<SharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<SharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct SharedSpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
};

DCOOL_TEST_CASE(dcoolConcurrency, sharedSpinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<SharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<SharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct FlexibleSharedMutexConfig {
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Boolean upgradeable = true;
	static constexpr dcool::core::Boolean downgradeable = true;
};

DCOOL_TEST_CASE(dcoolConcurrency, flexibleSharedMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<FlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<FlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexFlexibility<dcool::concurrency::Mutex<FlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct FlexibleSharedSpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Boolean upgradeable = true;
	static constexpr dcool::core::Boolean downgradeable = true;
};

DCOOL_TEST_CASE(dcoolConcurrency, flexibleSharedSpinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<FlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<FlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexFlexibility<dcool::concurrency::Mutex<FlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct InclusivePreferedSharedMutexConfig {
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateTrue;
};

DCOOL_TEST_CASE(dcoolConcurrency, inclusivePreferedSharedMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<InclusivePreferedSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<InclusivePreferedSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct InclusivePreferedSharedSpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateFalse;
};

DCOOL_TEST_CASE(dcoolConcurrency, inclusivePreferedSharedSpinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<InclusivePreferedSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<InclusivePreferedSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct ExclusivePreferedSharedMutexConfig {
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateFalse;
};

DCOOL_TEST_CASE(dcoolConcurrency, exclusivePreferedSharedMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<ExclusivePreferedSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<ExclusivePreferedSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct ExclusivePreferedSharedSpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateTrue;
};

DCOOL_TEST_CASE(dcoolConcurrency, exclusivePreferedSharedSpinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<ExclusivePreferedSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<ExclusivePreferedSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct ExclusivePreferedFlexibleSharedMutexConfig {
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Boolean upgradeable = true;
	static constexpr dcool::core::Boolean downgradeable = true;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateTrue;
};

DCOOL_TEST_CASE(dcoolConcurrency, exclusivePreferedFlexibleSharedMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexFlexibility<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

struct ExclusivePreferedFlexibleSharedSpinMutexConfig {
	static constexpr dcool::core::Boolean spinnable = true;
	static constexpr dcool::core::UnsignedMaxInteger maxShare = 3;
	static constexpr dcool::core::Boolean upgradeable = true;
	static constexpr dcool::core::Boolean downgradeable = true;
	static constexpr dcool::core::Triboolean preferExclusive = dcool::core::determinateTrue;
};

DCOOL_TEST_CASE(dcoolConcurrency, exclusivePreferedFlexibleSharedSpinMutexBasics) {
	testMutexCommon<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexShare<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testMutexFlexibility<dcool::concurrency::Mutex<ExclusivePreferedFlexibleSharedSpinMutexConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}
