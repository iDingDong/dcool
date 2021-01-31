#include <dcool/container.hpp>
#include <dcool/resource.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolContainer, linkedBasics) {
	dcool::container::ForwardLinked<int> linked1;
	linked1.emplaceFront(1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(1));
	linked1.emplaceFront(2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(2, 1));
	linked1.emplaceFront(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(3, 2, 1));
	dcool::container::ForwardLinked<int> linked2 = linked1;
	linked1.eraseAfter(linked1.begin());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(3, 1));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE(3, 2, 1));
}

DCOOL_TEST_CASE(dcoolContainer, linkedWithConcretePool) {
	struct Config {
		using Pool = dcool::resource::ConcreteReferencePool<8, dcool::core::storageRequirement<8, 4>>;
	};

	dcool::container::ForwardLinked<int, Config> linked1;
	linked1.emplaceFront(1);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(1));
	linked1.emplaceFront(2);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(2, 1));
	linked1.emplaceFront(3);
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(3, 2, 1));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(3, 2, 1));
	dcool::container::ForwardLinked<int, Config> linked2 = linked1;
	linked1.eraseAfter(linked1.begin());
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked1, DCOOL_TEST_SEQUENCE(3, 1));
	DCOOL_TEST_EXPECT_RANGE_EQUALITY(linked2, DCOOL_TEST_SEQUENCE(3, 2, 1));
}
