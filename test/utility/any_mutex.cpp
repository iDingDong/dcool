#include <dcool/core.hpp>
#include <dcool/test.hpp>
#include <dcool/utility.hpp>

#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>

namespace {
	struct TimedAnyMutexConfig {
		using SupportedTimePoints = dcool::core::Types<std::chrono::steady_clock::time_point>;
	};
}

DCOOL_TEST_CASE(dcoolUtility, anyMutexBasics) {
	struct FakeMutex {
		dcool::core::Boolean locked = false;

		FakeMutex() = default;
		FakeMutex(FakeMutex const&) = delete;
		auto operator = (FakeMutex const&) -> FakeMutex& = delete;

		void lock() {
			if (locked) {
				throw std::exception();
			}
			locked = true;
		}

		auto tryLock() noexcept -> dcool::core::Boolean {
			if (locked) {
				return false;
			}
			locked = true;
			return true;
		}

		auto tryLockUntil(std::chrono::steady_clock::time_point t) noexcept -> dcool::core::Boolean {
			if (locked) {
				std::this_thread::sleep_until(t);
				return this->tryLock();
			}
			locked = true;
			return true;
		}

		void unlock() {
			if (!locked) {
				throw std::exception();
			}
			locked = false;
		}
	};

	dcool::utility::AnyMutex anyMutex1;
	DCOOL_TEST_EXPECT(!(anyMutex1.valid()));
	DCOOL_TEST_EXPECT(anyMutex1.typeInfo() == typeid(void));
	DCOOL_TEST_EXPECT(dcool::core::size(anyMutex1.storageRequirement()) == 0);
	DCOOL_TEST_EXPECT_THROW(
		dcool::utility::BadAnyMutexAction const&,
		{
			static_cast<void>(anyMutex1.lock());
		}
	);
	using TimedAnyMutex = dcool::utility::AnyMutex<TimedAnyMutexConfig>;
	auto anyMutex2 = TimedAnyMutex(dcool::core::typedInPlace<FakeMutex>);
	DCOOL_TEST_EXPECT(anyMutex2.valid());
	DCOOL_TEST_EXPECT(!(anyMutex2.access<FakeMutex>().locked));
	DCOOL_TEST_EXPECT(anyMutex2.tryLock());
	using namespace std::literals::chrono_literals;
	DCOOL_TEST_EXPECT(!(anyMutex2.tryLockFor(10ms)));
	DCOOL_TEST_EXPECT(anyMutex2.access<FakeMutex>().locked);
	anyMutex2.unlock();
	DCOOL_TEST_EXPECT(!(anyMutex2.access<FakeMutex>().locked));
	anyMutex2.lock();
	DCOOL_TEST_EXPECT(anyMutex2.access<FakeMutex>().locked);
	DCOOL_TEST_EXPECT(!(anyMutex2.tryLock()));
	DCOOL_TEST_EXPECT(anyMutex2.access<FakeMutex>().locked);
	anyMutex2.unlock();
	DCOOL_TEST_EXPECT(!(anyMutex2.access<FakeMutex>().locked));
}
