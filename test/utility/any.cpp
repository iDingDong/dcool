#include <dcool/test.hpp>
#include <dcool/utility.hpp>

DCOOL_TEST_CASE(dcoolUtility, anyBasics) {
	dcool::utility::Any any1;
	DCOOL_TEST_EXPECT(!(any1.valid()));
	DCOOL_TEST_EXPECT(any1.typeInfo() == typeid(void));
	DCOOL_TEST_EXPECT(dcool::core::size(any1.storageRequirement()) == 0);
	DCOOL_TEST_EXPECT(any1.storage() == dcool::core::nullPointer);
	{
		dcool::core::Boolean thrown = false;
		try {
			static_cast<void>(any1.value<int>());
		} catch (dcool::utility::BadAnyCast const&) {
			thrown = true;
		}
		DCOOL_TEST_EXPECT(thrown);
	}
	dcool::utility::Any any2 = 237;
	DCOOL_TEST_EXPECT(any2.valid());
	DCOOL_TEST_EXPECT(any2.typeInfo() == typeid(int));
	DCOOL_TEST_EXPECT(any2.storageRequirement() == dcool::core::storageRequirementFor<int>);
	DCOOL_TEST_EXPECT(any2.storage() != dcool::core::nullPointer);
	DCOOL_TEST_EXPECT(any2.value<int>() == 237);
	{
		dcool::core::Boolean thrown = false;
		try {
			static_cast<void>(any2.value<char>());
		} catch (dcool::utility::BadAnyCast const&) {
			thrown = true;
		}
		DCOOL_TEST_EXPECT(thrown);
	}
	any1 = 137;
	DCOOL_TEST_EXPECT(any1.valid());
	DCOOL_TEST_EXPECT(any1.typeInfo() == typeid(int));
	DCOOL_TEST_EXPECT(any1.storageRequirement() == dcool::core::storageRequirementFor<int>);
	DCOOL_TEST_EXPECT(any1.storage() != dcool::core::nullPointer);
	DCOOL_TEST_EXPECT(any1.value<int>() == 137);
	{
		dcool::core::Boolean thrown = false;
		try {
			static_cast<void>(any1.value<char>());
		} catch (dcool::utility::BadAnyCast const&) {
			thrown = true;
		}
		DCOOL_TEST_EXPECT(thrown);
	}
}
