#include <dcool/container.hpp>
#include <dcool/resource.hpp>
#include <dcool/test.hpp>

#include <iostream>

DCOOL_TEST_CASE(dcoolContainer, linkedBasics) {
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

	using Linked = dcool::container::Linked<Traced>;
	{
		Linked linked1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		linked1.emplaceFront(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		linked1.emplaceFront(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 2, 1));
		linked1.emplaceFront(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
		{
			Linked linked2 = linked1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			linked1.eraseAfter(linked1.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 1));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, bidirectionalLinkedBasics) {
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

	using Linked = dcool::container::BidirectionalLinked<Traced>;
	{
		Linked linked1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		linked1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		linked1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		linked1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			Linked linked2 = linked1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			linked1.erase(::dcool::core::next(linked1.begin()));
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, linkedWithConcretePool) {
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

	struct ExpectedNodeEquivalence {
		unsigned char next;
		Traced value;
	};

	struct Config {
		using Pool = dcool::resource::ConcreteReferencePool<
			8, dcool::core::storageRequirement<sizeof(ExpectedNodeEquivalence), alignof(Traced)>
		>;
	};

	using Linked = dcool::container::Linked<Traced, Config>;
	{
		Linked linked1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		linked1.emplaceFront(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		linked1.emplaceFront(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 2, 1));
		linked1.emplaceFront(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
		{
			Linked linked2 = linked1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			linked1.eraseAfter(linked1.begin());
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 1));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, bidirectionalLinkedWithConcretePool) {
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

	struct ExpectedNodeEquivalence {
		unsigned char next;
		unsigned char previous;
		Traced value;
	};

	struct Config {
		using Pool = dcool::resource::ConcreteReferencePool<
			8, dcool::core::storageRequirement<sizeof(ExpectedNodeEquivalence), alignof(Traced)>
		>;
	};

	using Linked = dcool::container::BidirectionalLinked<Traced, Config>;
	{
		Linked linked1;
		DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
		linked1.emplaceBack(1);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 1);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
		linked1.emplaceBack(2);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
		linked1.emplaceBack(3);
		DCOOL_TEST_EXPECT(Traced::livingCount() == 3);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		{
			Linked linked2 = linked1;
			DCOOL_TEST_EXPECT(Traced::livingCount() == 6);
			linked1.erase(::dcool::core::next(linked1.begin()));
			DCOOL_TEST_EXPECT(Traced::livingCount() == 5);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
		}
		DCOOL_TEST_EXPECT(Traced::livingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::livingCount() == 0);
}
