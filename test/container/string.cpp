#include <dcool/container.hpp>
#include <dcool/test.hpp>

#include <string>
#include <string_view>

DCOOL_TEST_CASE(dcoolContainer, stringBasics) {
	constexpr char8_t sample1[] = u8"Test string 1";
	constexpr char8_t sample2[] = u8"Test string 123";
	dcool::container::String string1 = sample1;
	dcool::container::String string2 = sample2;
	DCOOL_TEST_EXPECT(string1 < string2);
	std::u8string standardString1 = sample1;
	std::u8string_view standardStringView1 = sample1;
	DCOOL_TEST_EXPECT(string1 == sample1);
	DCOOL_TEST_EXPECT(string1.toStandard() == standardString1);
	DCOOL_TEST_EXPECT(string1.toStandardView() == standardStringView1);
}
