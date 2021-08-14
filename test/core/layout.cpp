#include <dcool/core.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolCore, basicLayoutBasics) {
	using Layout1 = dcool::core::OrderedCompactBasicLayout<int, int, int>;
	static_assert(Layout1::memberOffsetAt<0> == 0);
	static_assert(Layout1::memberOffsetAt<1> == sizeof(int));
	static_assert(Layout1::memberOffsetAt<2> == 2 * sizeof(int));
	static_assert(Layout1::occupation == 3 * sizeof(int));
	static_assert(Layout1::size == 3 * sizeof(int));
	static_assert(Layout1::alignment == alignof(int));
	using Layout2 = dcool::core::OrderedCompactBasicLayout<char, long long, char>;
	static_assert(Layout2::memberOffsetAt<0> == 0);
	static_assert(Layout2::memberOffsetAt<1> == alignof(long long));
	static_assert(Layout2::memberOffsetAt<2> == alignof(long long) + sizeof(long long));
	static_assert(Layout2::occupation == alignof(long long) + sizeof(long long) + 1);
	static_assert(Layout2::size == alignof(long long) + sizeof(long long) + alignof(long long));
	static_assert(Layout2::alignment == alignof(long long));
}
