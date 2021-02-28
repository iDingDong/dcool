#include <dcool/container.hpp>
#include <dcool/resource.hpp>
#include <dcool/test.hpp>

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

	using Linked = dcool::container::ForwardLinked<Traced>;
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

DCOOL_TEST_CASE(dcoolContainer, linkedWithConcretePool) {
	struct Config {
		using Pool = dcool::resource::ConcreteReferencePool<8, dcool::core::storageRequirement<8, 4>>;
	};

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

	using Linked = dcool::container::ForwardLinked<Traced, Config>;
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
