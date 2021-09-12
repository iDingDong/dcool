#include <dcool/core.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolCore, integerBasics) {
	using namespace dcool::core::extendedIntegerLiteral;
	auto i1 = 0_UI1;
	DCOOL_TEST_EXPECT(i1 == 0);
	++i1;
	DCOOL_TEST_EXPECT(i1 == 1);
	++i1;
	DCOOL_TEST_EXPECT(i1 == 0);
	i1 += 1_UI1;
	DCOOL_TEST_EXPECT(i1 == 1);
	i1 += 1_UI1;
	DCOOL_TEST_EXPECT(i1 == 0);
	i1 -= 1_UI1;
	DCOOL_TEST_EXPECT(i1 == 1);
	auto i6 = 0b011111_UI6;
	i6 += i6;
	DCOOL_TEST_EXPECT(i6 == 0b111110);
	++i6;
	DCOOL_TEST_EXPECT(i6 == i6.max);
	i6 += i1;
	DCOOL_TEST_EXPECT(i6 == 0);
	DCOOL_TEST_EXPECT(i6 < i1);
}
