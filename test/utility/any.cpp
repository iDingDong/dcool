#include <dcool/core.hpp>
#include <dcool/test.hpp>
#include <dcool/utility.hpp>

DCOOL_TEST_CASE(dcoolUtility, anyBasics) {
	dcool::utility::Any any1;
	DCOOL_TEST_EXPECT(!(any1.valid()));
	DCOOL_TEST_EXPECT(any1.typeInfo() == typeid(void));
	DCOOL_TEST_EXPECT(dcool::core::size(any1.storageRequirement()) == 0);
	DCOOL_TEST_EXPECT_THROW(dcool::utility::BadAnyCast const&, {
		static_cast<void>(any1.value<int>());
	});
	dcool::utility::Any any2 = 237;
	DCOOL_TEST_EXPECT(any2.valid());
	DCOOL_TEST_EXPECT(any2.typeInfo() == typeid(int));
	DCOOL_TEST_EXPECT(any2.storageRequirement() == dcool::core::storageRequirementFor<int>);
	DCOOL_TEST_EXPECT(any2.value<int>() == 237);
	DCOOL_TEST_EXPECT_THROW(dcool::utility::BadAnyCast const&, {
		static_cast<void>(any2.value<char>());
	});
	any1 = 137;
	DCOOL_TEST_EXPECT(any1.valid());
	DCOOL_TEST_EXPECT(any1.typeInfo() == typeid(int));
	DCOOL_TEST_EXPECT(any1.storageRequirement() == dcool::core::storageRequirementFor<int>);
	DCOOL_TEST_EXPECT(any1.value<int>() == 137);
	DCOOL_TEST_EXPECT_THROW(dcool::utility::BadAnyCast const&, {
		static_cast<void>(any1.value<char>());
	});
	auto bigObject = dcool::core::Tuple<int, int, int, int, int>(7, 3, 4, 6, 5);
	dcool::utility::Any any3 = bigObject;
	DCOOL_TEST_EXPECT(any3.value<decltype(bigObject)>() == bigObject);
	any2 = any3;
	DCOOL_TEST_EXPECT(any2.value<decltype(bigObject)>() == bigObject);
	DCOOL_TEST_EXPECT(any3.value<decltype(bigObject)>() == bigObject);
	any1 = dcool::core::move(any3);
	DCOOL_TEST_EXPECT(any1.value<decltype(bigObject)>() == bigObject);
	DCOOL_TEST_EXPECT(any2.value<decltype(bigObject)>() == bigObject);
}
