#include <dcool/container.hpp>
#include <dcool/resource.hpp>
#include <dcool/test.hpp>

template <dcool::core::Boolean bidirectionalC, typename ConfigT = dcool::core::Empty<>> void testLinked(
	DCOOL_TEST_CONTEXT_PARAMETER
) {
	struct UniqueType;
	using Traced = dcool::test::TraceableTag<::dcool::core::Empty<UniqueType>>;
	using Linked = dcool::core::ConditionalType<
		bidirectionalC, dcool::container::BidirectionalLinked<Traced, ConfigT>, dcool::container::Linked<Traced, ConfigT>
	>;
	{
		Linked linked1;
		DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 0);
		if constexpr (bidirectionalC) {
			linked1.emplaceBack(1);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 1);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
			linked1.emplaceBack(2);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 2);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2));
			linked1.emplaceBack(3);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 3);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			{
				Linked linked2 = linked1;
				DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 6);
				linked1.erase(::dcool::core::next(linked1.begin()));
				DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 5);
				DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 3));
				DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 1, 2, 3));
			}
		} else {
			linked1.emplaceFront(1);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 1);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 1));
			linked1.emplaceFront(2);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 2);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 2, 1));
			linked1.emplaceFront(3);
			DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 3);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
			{
				Linked linked2 = linked1;
				DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 6);
				linked1.eraseAfter(linked1.begin());
				DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 5);
				DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 1));
				DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE_OF(Traced, 3, 2, 1));
			}
		}
		DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 2);
	}
	DCOOL_TEST_EXPECT(Traced::physicallyLivingCount() == 0);
}

DCOOL_TEST_CASE(dcoolContainer, linkedBasics) {
	testLinked<false>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

DCOOL_TEST_CASE(dcoolContainer, bidirectionalLinkedBasics) {
	testLinked<true>(DCOOL_TEST_CONTEXT_ARGUMENT);
}

template <dcool::core::StorageRequirement requirementC> struct ConcretLinkedConfig {
	using Pool = dcool::resource::ConcreteReferencePool<8, requirementC>;
};

DCOOL_TEST_CASE(dcoolContainer, linkedWithConcretePool) {
	using Traced = dcool::test::TraceableTag<>;

	struct ExpectedNodeEquivalence {
		unsigned char next;
		Traced value;
	};

	testLinked<false, ConcretLinkedConfig<dcool::core::storageRequirement<sizeof(ExpectedNodeEquivalence), alignof(Traced)>>>(
		DCOOL_TEST_CONTEXT_ARGUMENT
	);
}

DCOOL_TEST_CASE(dcoolContainer, bidirectionalLinkedWithConcretePool) {
	using Traced = dcool::test::TraceableTag<>;

	struct ExpectedNodeEquivalence {
		unsigned char next;
		unsigned char previous;
		Traced value;
	};

	testLinked<true, ConcretLinkedConfig<dcool::core::storageRequirement<sizeof(ExpectedNodeEquivalence), alignof(Traced)>>>(
		DCOOL_TEST_CONTEXT_ARGUMENT
	);
}
