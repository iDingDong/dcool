#include <dcool/container.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolContainer, listBasics) {
	struct Traced {
		static auto livingCount() -> dcool::core::Length& {
			static dcool::core::Length count = 0;
			return count;
		}

		int value;

		Traced(int inputValue): value(inputValue) {
			++(livingCount());
		}

		Traced(Traced const& other): value(other.value) {
			++(livingCount());
		}

		~Traced() {
			--(livingCount());
		}

		auto operator ==(Traced const&) const -> dcool::core::Boolean = default;
	};

	using List = dcool::container::List<Traced>;
	{
		List list1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		list1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		list1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		list1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			List list2 = list1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			list1.erase(list1.begin() + 1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list1.emplace(list1.begin(), 0);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 3));
			list1.emplace(list1.begin() + 2, 2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.emplace(list2.begin(), -1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			list1.swapWith(list2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.erase(list2.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list2.batchInsert(list2.begin() + 1, list1.begin() + 1, list1.begin() + 3);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 9);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 1, 2, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 4);
		list1.erase(list1.begin() + 1, list1.end() - 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 3));
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, stuffedlistBasics) {
	struct Traced {
		static auto livingCount() -> dcool::core::Length& {
			static dcool::core::Length count = 0;
			return count;
		}

		int value;

		Traced(int inputValue): value(inputValue) {
			++(livingCount());
		}

		Traced(Traced const& other): value(other.value) {
			++(livingCount());
		}

		~Traced() {
			--(livingCount());
		}

		auto operator ==(Traced const&) const -> dcool::core::Boolean = default;
	};

	using List = dcool::container::StuffedList<Traced>;
	{
		List list1;
		DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		list1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		list1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		list1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			List list2 = list1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
			list1.erase(list1.begin() + 1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list1.emplace(list1.begin(), 0);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 3));
			list1.emplace(list1.begin() + 2, 2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.emplace(list2.begin(), -1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			list1.swapWith(list2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.erase(list2.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list2.batchInsert(list2.begin() + 1, list1.begin() + 1, list1.begin() + 3);
			DCOOL_TEST_EXPECT(list2.length() == list2.capacity());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 9);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 1, 2, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 4);
		list1.erase(list1.begin() + 1, list1.end() - 1);
		DCOOL_TEST_EXPECT(list1.length() == list1.capacity());
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 3));
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, circularlistBasics) {
	struct Traced {
		static auto livingCount() -> dcool::core::Length& {
			static dcool::core::Length count = 0;
			return count;
		}

		int value;

		Traced(int inputValue): value(inputValue) {
			++(livingCount());
		}

		Traced(Traced const& other): value(other.value) {
			++(livingCount());
		}

		~Traced() {
			--(livingCount());
		}

		auto operator ==(Traced const&) const -> dcool::core::Boolean = default;
	};

	using List = dcool::container::CircularList<Traced>;
	{
		List list1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		list1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		list1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		list1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			List list2 = list1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			list1.erase(list1.begin() + 1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list1.emplace(list1.begin(), 0);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 3));
			list1.emplace(list1.begin() + 2, 2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.emplace(list2.begin(), -1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			list1.swapWith(list2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.erase(list2.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list2.batchInsert(list2.begin() + 1, list1.begin() + 1, list1.begin() + 3);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 9);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 1, 2, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 4);
		list1.erase(list1.begin() + 1, list1.end() - 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 3));
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, smallListBasics) {
	struct Traced {
		static auto livingCount() -> dcool::core::Length& {
			static dcool::core::Length count = 0;
			return count;
		}

		int value;

		Traced(int inputValue): value(inputValue) {
			++(livingCount());
		}

		Traced(Traced const& other): value(other.value) {
			++(livingCount());
		}

		~Traced() {
			--(livingCount());
		}

		auto operator ==(Traced const&) const -> dcool::core::Boolean = default;
	};

	using List = dcool::container::SmallList<Traced, 2>;
	{
		List list1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		list1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		list1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		list1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			List list2 = list1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			list1.erase(list1.begin() + 1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list1.emplace(list1.begin(), 0);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 3));
			list1.emplace(list1.begin() + 2, 2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.emplace(list2.begin(), -1);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			list1.swapWith(list2);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 8);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 1, 2, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 0, 1, 2, 3));
			list2.erase(list2.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 7);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			list2.batchInsert(list2.begin() + 1, list1.begin() + 1, list1.begin() + 3);
			DCOOL_TEST_EXPECT(Traced::livingCount() == 9);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(list2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 1, 2, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 4);
		list1.erase(list1.begin() + 1, list1.end() - 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(list1, DCOOL_TEST_SEQUENCE_OF(Traced, -1, 3));
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}
