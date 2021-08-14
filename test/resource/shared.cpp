#include <dcool/resource.hpp>
#include <dcool/test.hpp>

#include <set>

DCOOL_TEST_CASE(dcoolResource, sharedBasics) {
	class A {
		private: int m_id;

		public: A() noexcept: m_id(allocateId()) {
			living().insert(this->m_id);
		}

		public: A(A const& other) noexcept: m_id(allocateId()) {
			living().insert(this->m_id);
		}

		public: A(A&& other) noexcept: m_id(allocateId()) {
			living().insert(this->m_id);
		}

		public: ~A() noexcept {
			living().erase(this->m_id);
		}

		private: static int allocateId() {
			static int id = 0;
			return id++;
		}

		public: static ::std::set<int>& living() {
			static ::std::set<int> livings;
			return livings;
		}
	};

	{
		auto ptr1 = ::dcool::resource::wrapToShare(*new A);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0));
		{
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0));
			auto ptr2 = ::dcool::resource::wrapToShare(*new A);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0, 1));
			auto ptr3 = ::dcool::resource::wrapToShare(*new A);
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0, 1, 2));
			ptr1 = ptr2;
			DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(1, 2));
		}
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(1));
	}
	DCOOL_TEST_EXPECT(A::living().empty());
}
