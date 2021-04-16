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

	dcool::utility::DefaultOverloadedFunction<auto(int, int) -> int, void(int), auto() -> int> function1 = OverloadedAdder();
	DCOOL_TEST_EXPECT(function1.immutablyInvocable<0>());
	DCOOL_TEST_EXPECT(!(function1.immutablyInvocable<1>()));
	DCOOL_TEST_EXPECT(function1.immutablyInvocable<2>());
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(function1(3, 17) == 3 + 17));
	DCOOL_TEST_EXPECT(DCOOL_CORE_PARAMETER(dcool::core::constantize(function1)(3, 17) == 3 + 17));
	DCOOL_TEST_EXPECT(function1() == 0);
	function1(4);
	DCOOL_TEST_EXPECT(function1() == 4);
	function1(27);
	DCOOL_TEST_EXPECT(function1() == 4 + 27);
	{
		dcool::core::Boolean thrown = false;
		try {
			dcool::core::constantize(function1)(3);
		} catch (dcool::utility::BadFunctionCall const&) {
			thrown = true;
		}
		DCOOL_TEST_EXPECT(thrown);
	}
}

DCOOL_TEST_CASE(dcoolUtility, overloadedFunctionOverloading) {
	struct OverloadedTypeInfo {
		auto operator ()(int) noexcept -> dcool::core::TypeInfo const& {
			return typeid(int);
		}

		auto operator ()(double) noexcept -> dcool::core::TypeInfo const& {
			return typeid(double);
		}
	};

	dcool::utility::DefaultOverloadedFunction<
		auto(int) noexcept -> dcool::core::TypeInfo const&, auto(double) noexcept -> dcool::core::TypeInfo const&
	> function1 = OverloadedTypeInfo();

	DCOOL_TEST_EXPECT(function1(1) == typeid(int));
	DCOOL_TEST_EXPECT(function1(1.0) == typeid(double));
}
