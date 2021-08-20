#include <dcool/test.hpp>
#include <dcool/utility.hpp>

DCOOL_TEST_CASE(dcoolUtility, overloadedFunctionBasics) {
	struct OverloadedAdder {
		int stashed = 0;

		auto operator ()(int a, int b) const noexcept -> int {
			return a + b;
		}

		// This overload is not immutably invocable
		void operator ()(int b) noexcept {
			this->stashed += b;
		}

		auto operator ()() const noexcept -> int {
			return stashed;
		}
	};

	dcool::utility::DefaultOverloadedFunction<
		auto (int, int) const -> int, void (int), auto () -> int
	> function1 = OverloadedAdder();
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(function1(3, 17) == 3 + 17));
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(dcool::core::constantize(function1)(3, 17) == 3 + 17));
	DCOOL_TEST_EXPECT(function1() == 0);
	function1(4);
	DCOOL_TEST_EXPECT(function1() == 4);
	function1(27);
	DCOOL_TEST_EXPECT(function1() == 4 + 27);
}

DCOOL_TEST_CASE(dcoolUtility, overloadedFunctionOverloading) {
	struct OverloadedFunctor {
		auto operator ()(int) const noexcept -> int {
			return 0;
		}

		auto operator ()(int) noexcept -> int {
			return 1;
		}

		auto operator ()(double) noexcept -> int {
			return 2;
		}
	};

	dcool::utility::DefaultOverloadedFunction<
		auto (int) const noexcept -> int, auto (int) noexcept -> int, auto (double) noexcept -> int
	> function1 = OverloadedFunctor();

	DCOOL_TEST_EXPECT(dcool::core::constantize(function1)(1) == 0);
	DCOOL_TEST_EXPECT(function1(1) == 1);
	DCOOL_TEST_EXPECT(function1(1.0) == 2);
}
