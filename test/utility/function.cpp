#include <dcool/core.hpp>
#include <dcool/test.hpp>
#include <dcool/utility.hpp>

DCOOL_TEST_CASE(dcoolUtility, functionBasics) {
	dcool::utility::Function<auto(int, int) -> int> function1 = [](int a, int b) {
		return a + b;
	};
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(function1(3, 17) == 3 + 17));
	dcool::utility::Function<void(int, int, int&) const> function2 = [](int a, int b, int& c) {
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
