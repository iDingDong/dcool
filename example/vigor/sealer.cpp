#include <dcool/container.hpp>
#include <dcool/core.hpp>
#include <dcool/vigor.hpp>

#include <iostream>

struct Axception {
};

struct A {
	static int countBeforeThrow;
	A() = default;

	A(A const& other) {
		if (countBeforeThrow > 0) {
			--countBeforeThrow;
		} else {
			throw Axception();
		}
	};

	auto operator =(A const& other) -> A& {
		if (countBeforeThrow > 0) {
			--countBeforeThrow;
		} else {
			throw Axception();
		}
	}
};

int A::countBeforeThrow = 8;

int main() {
	using SealedValue = dcool::container::StaticList<A, 3>;
	SealedValue la;
	la.emplaceBack();
	la.emplaceBack();
	dcool::vigor::Sealer<SealedValue> sealer(dcool::core::inPlace, dcool::core::move(la));
	auto observer = [](SealedValue const& value) {
		std::cout << static_cast<dcool::core::Length>(value.length()) << std::endl;
	};
	auto transaction = [](SealedValue value) -> SealedValue {
		value.emplaceBack();
		return value;
	};
	sealer.runObserver(observer);
	try {
		sealer.runTransaction(transaction);
	} catch (...) {
		std::cout << "Expected exception." << std::endl;
	}
	sealer.runObserver(observer);
	return 0;
}
