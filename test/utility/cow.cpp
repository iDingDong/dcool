#include <dcool/test.hpp>
#include <dcool/utility.hpp>

#include <cstdint>
#include <set>

DCOOL_TEST_CASE(dcoolUtility, cowBasics) {
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

		public: static std::set<int>& living() {
			static std::set<int> livings;
			return livings;
		}
	};

	{
		dcool::utility::Cow<A> object1(dcool::core::inPlace);
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0));
		dcool::utility::Cow<A> object2(object1);
		DCOOL_TEST_EXPECT(dcool::core::addressOf(object1.value()) == dcool::core::addressOf(object2.value()));
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0));
		dcool::utility::Cow<A> object3(object2);
		DCOOL_TEST_EXPECT(dcool::core::addressOf(object2.value()) == dcool::core::addressOf(object3.value()));
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0));
		DCOOL_TEST_EXPECT(dcool::core::addressOf(object1.value()) != dcool::core::addressOf(object2.mutableValue()));
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0, 1));
		std::uintptr_t object2Address = reinterpret_cast<std::uintptr_t>(dcool::core::addressOf(object2.value()));
		DCOOL_TEST_EXPECT(object2Address == reinterpret_cast<std::uintptr_t>(dcool::core::addressOf(object2.mutableValue())));
		DCOOL_TEST_EXPECT_RANGE_EQUALITY(A::living(), DCOOL_TEST_SEQUENCE(0, 1));
	}
	DCOOL_TEST_EXPECT(A::living().empty());
}
