#include <dcool/container.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolContainer, listBasics) {
	dcool::container::List<int> list1;
	list1.emplaceBack(1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1));
	list1.emplaceBack(2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2));
	list1.emplaceBack(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2, 3));
	dcool::container::List<int> list2 = list1;
	list1.erase(list1.begin() + 1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
	list1.emplace(list1.begin(), 0);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 3));
	list1.emplace(list1.begin() + 2, 2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.emplace(list2.begin(), -1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	list1.swapWith(list2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.erase(list2.begin());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
}

DCOOL_TEST_CASE(dcoolContainer, stuffedlistBasics) {
	dcool::container::StuffedList<int> list1;
	list1.emplaceBack(1);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1));
	list1.emplaceBack(2);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2));
	list1.emplaceBack(3);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2, 3));
	dcool::container::StuffedList<int> list2 = list1;
	list1.erase(list1.begin() + 1);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 3));
	DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
	list1.emplace(list1.begin(), 0);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 3));
	list1.emplace(list1.begin() + 2, 2);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.emplace(list2.begin(), -1);
	DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	list1.swapWith(list2);
	DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.erase(list2.begin());
	DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
}

DCOOL_TEST_CASE(dcoolContainer, circularlistBasics) {
	dcool::container::CircularList<int> list1;
	list1.emplaceBack(1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1));
	list1.emplaceBack(2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2));
	list1.emplaceBack(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2, 3));
	dcool::container::CircularList<int> list2 = list1;
	list1.erase(list1.begin() + 1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
	list1.emplace(list1.begin(), 0);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 3));
	list1.emplace(list1.begin() + 2, 2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.emplace(list2.begin(), -1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	list1.swapWith(list2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.erase(list2.begin());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
}

DCOOL_TEST_CASE(dcoolContainer, smallListBasics) {
	dcool::container::SmallList<int, 2> list1;
	list1.emplaceBack(1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1));
	list1.emplaceBack(2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2));
	list1.emplaceBack(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 2, 3));
	dcool::container::SmallList<int, 2> list2 = list1;
	list1.erase(list1.begin() + 1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(1, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
	list1.emplace(list1.begin(), 0);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 3));
	list1.emplace(list1.begin() + 2, 2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.emplace(list2.begin(), -1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	list1.swapWith(list2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE(-1, 1, 2, 3));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(0, 1, 2, 3));
	list2.erase(list2.begin());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE(1, 2, 3));
}
