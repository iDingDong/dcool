#include <dcool/container.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolContainer, listBasics) {
	dcool::container::List<int> list1;
	list1.emplaceBack(1);
	list1.emplaceBack(2);
	list1.emplaceBack(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2, 3));
	dcool::container::List<int> list2 = list1;
	list1.erase(list1.begin() + 1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
}
