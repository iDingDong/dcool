#include <dcool/config.hpp>
#include <dcool/test.hpp>
#include <dcool/service.hpp>

#include <chrono>
#include <stdexcept>
#include <thread>

template <typename InvokerT> static void testOnceInvokerBasics(DCOOL_TEST_CONTEXT_PARAMETER) {
	InvokerT invoker;
	dcool::core::Boolean flag = false;
	if constexpr (InvokerT::synchronizationStrategy == dcool::service::OnceInvokerSynchronizationStrategy::none) {
		auto toInvoke = [DCOOL_TEST_CAPTURE_CONTEXT, &flag]() {
			DCOOL_TEST_EXPECT(!flag);
			flag = true;
		};
		invoker.invoke(toInvoke);
		invoker.invoke(toInvoke);
	} else {
		auto toInvoke = [DCOOL_TEST_CAPTURE_CONTEXT, &flag]() {
			DCOOL_TEST_EXPECT(!flag);
			flag = true;
			using namespace std::literals::chrono_literals;
			::std::this_thread::sleep_for(10ms);
		};
		std::jthread task(
			[DCOOL_TEST_CAPTURE_CONTEXT, &invoker, &toInvoke]() {
				invoker.invoke(toInvoke);
			}
		);
		invoker.invoke(toInvoke);
		task.join();
	}
	DCOOL_TEST_EXPECT(flag);
}

template <typename InvokerT> static void testOnceInvokerException(DCOOL_TEST_CONTEXT_PARAMETER) {
	class Ixception: public std::exception {
	};

	// Workaround for a standard library bug.
	// See document/dependency_bugs#Bug_4 for more details.
#if DCOOL_DEPENDENCY_BUG_4
	if constexpr (InvokerT::synchronizationStrategy == dcool::service::OnceInvokerSynchronizationStrategy::standard) {
		return;
	}
#endif
	InvokerT invoker;
	unsigned step = 0;
	if constexpr (InvokerT::synchronizationStrategy == dcool::service::OnceInvokerSynchronizationStrategy::none) {
		auto toInvoke = [DCOOL_TEST_CAPTURE_CONTEXT, &step]() {
			DCOOL_TEST_EXPECT(step <= 1);
			++step;
			if (step <= 1) {
				throw Ixception();
			}
		};
		DCOOL_TEST_EXPECT_THROW(
			Ixception const&,
			{
				invoker.invoke(toInvoke);
			}
		);
		invoker.invoke(toInvoke);
		invoker.invoke(toInvoke);
	} else {
		dcool::core::Boolean thrown;
		auto toInvoke = [DCOOL_TEST_CAPTURE_CONTEXT, &step]() {
			DCOOL_TEST_EXPECT(step <= 1);
			++step;
			using namespace std::literals::chrono_literals;
			::std::this_thread::sleep_for(10ms);
			if (step <= 1) {
				throw Ixception();
			}
		};
		std::jthread task1(
			[DCOOL_TEST_CAPTURE_CONTEXT, &invoker, &toInvoke, &thrown]() {
				try {
					invoker.invoke(toInvoke);
				} catch (Ixception const&) {
					DCOOL_TEST_EXPECT(!thrown);
					thrown = true;
				}
			}
		);
		std::jthread task2(
			[DCOOL_TEST_CAPTURE_CONTEXT, &invoker, &toInvoke, &thrown]() {
				try {
					invoker.invoke(toInvoke);
				} catch (Ixception const&) {
					DCOOL_TEST_EXPECT(!thrown);
					thrown = true;
				}
			}
		);
		try {
			invoker.invoke(toInvoke);
		} catch (Ixception const&) {
			DCOOL_TEST_EXPECT(!thrown);
			thrown = true;
		}
		task1.join();
		task2.join();
	}
	DCOOL_TEST_EXPECT(step == 2);
}

struct RegularOnceInvokerConfig {
	static constexpr dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy =
		dcool::service::OnceInvokerSynchronizationStrategy::regular
	;
};

struct BuzyOnceInvokerConfig {
	static constexpr dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy =
		dcool::service::OnceInvokerSynchronizationStrategy::busy
	;
};

struct TrivialOnceInvokerConfig {
	static constexpr dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy =
		dcool::service::OnceInvokerSynchronizationStrategy::none
	;
};

DCOOL_TEST_CASE(dcoolService, onceInvokerBasics) {
	testOnceInvokerBasics<dcool::service::OnceInvoker<>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerBasics<dcool::service::OnceInvoker<RegularOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerBasics<dcool::service::OnceInvoker<BuzyOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerBasics<dcool::service::OnceInvoker<TrivialOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

DCOOL_TEST_CASE(dcoolService, onceInvokerException) {
	testOnceInvokerException<dcool::service::OnceInvoker<>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerException<dcool::service::OnceInvoker<RegularOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerException<dcool::service::OnceInvoker<BuzyOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
	testOnceInvokerException<dcool::service::OnceInvoker<TrivialOnceInvokerConfig>>(DCOOL_TEST_CONTEXT_ARGUMENT);
}
