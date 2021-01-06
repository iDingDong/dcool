#include <dcool/resource.hpp>

#include <iostream>

class A {
	private: static int s_nextId;
	private: int m_id;
	public: A() noexcept: m_id(s_nextId++) {
		std::cout << "A[" << this->m_id << "] constructed.\n";
	}

	public: A(A const& other) noexcept: m_id(s_nextId++) {
		std::cout << "A[" << this->m_id << "] copy constructed from A[" << other.m_id << "].\n";
	}

	public: A(A&& other) noexcept: m_id(s_nextId++) {
		std::cout << "A[" << this->m_id << "] move constructed from A[" << other.m_id << "].\n";
	}

	public: ~A() noexcept {
		std::cout << "A[" << this->m_id << "] destructed.\n";
	}

	public: auto operator =(A const& other) noexcept -> A& {
		std::cout << "A[" << this->m_id << "] copied from A[" << other.m_id << "].\n";
		return *this;
	}

	public: auto operator =(A&& other) noexcept -> A& {
		std::cout << "A[" << this->m_id << "] moved from A[" << other.m_id << "].\n";
		return *this;
	}
};

int A::s_nextId = 0;

int main() {
	std::cout << "Begin\n";
	{
		auto ptr1 = ::dcool::resource::wrapToShare(*new A);
		std::cout << "Block 1 begin\n";
		{
			auto ptr2 = ::dcool::resource::wrapToShare(*new A);
			ptr1 = ptr2;
		}
		std::cout << "Block 1 end\n";
	}
	std::cout << "End\n";
	return 0;
}
