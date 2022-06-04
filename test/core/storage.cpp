#include <dcool/core.hpp>
#include <dcool/test.hpp>

#include <array>
#include <cstddef>

DCOOL_TEST_CASE(dcoolCore, storageBasics) {
	constexpr dcool::core::Alignment testedAlignment = 4;
	alignas(testedAlignment) std::array<char, testedAlignment * 2> a;
	auto p = a.data();
	for (dcool::core::Index i = 0; i < a.size(); ++i) {
		if (i % testedAlignment == 0) {
			DCOOL_TEST_EXPECT(dcool::core::aligned(p + i, testedAlignment));
		} else {
			DCOOL_TEST_EXPECT(!(dcool::core::aligned(p + i, testedAlignment)));
		}
	}
}
