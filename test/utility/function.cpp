#include <dcool/core.hpp>
#include <dcool/test.hpp>
#include <dcool/utility.hpp>

DCOOL_TEST_CASE(dcoolUtility, functionBasics) {
	dcool::utility::Function<auto(int, int) -> int> function1 = [](int a, int b) {
		return a + b;
	};
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(function1(3, 17) == 3 + 17));
	dcool::utility::Function<void(int, int, int&)> function2 = [](int a, int b, int& c) {
		c = a + b;
	};
	int result;
	function2(28, 4, result);
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(result == 28 + 4));
	function2 = [](int a, int b, int& c) {
		c = a - b;
	};
	function2(28, 4, result);
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(result == 28 - 4));
}

DCOOL_TEST_CASE(dcoolUtility, nonImmutablyInvocableFunction) {
	struct Counter {
		dcool::core::Length count = 0;

		void operator ()() noexcept {
			++(this->count);
		}
	};

	dcool::utility::Function<auto(int, int) -> int> function1 = [](int a, int b) {
		return a + b;
	};
	DCOOL_TEST_EXPECT(function1.immutablyInvocable());
	dcool::utility::Function<void()> function2 = Counter();
	DCOOL_TEST_EXPECT(!(function2.immutablyInvocable()));
	function2();
	DCOOL_TEST_EXPECT(function2.value<Counter>().count == 1);
	{
		dcool::core::Boolean thrown = false;
		try {
			dcool::core::constantize(function2)();
		} catch (dcool::utility::BadFunctionCall const&) {
			thrown = true;
		}
		DCOOL_TEST_EXPECT(thrown);
	}
}
