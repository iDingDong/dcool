#include <dcool/container.hpp>
#include <dcool/test.hpp>
#include <dcool/vigor.hpp>

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

DCOOL_TEST_CASE(dcoolVigor, sealerBasic) {
	using SealedValue = dcool::container::StaticList<A, 3>;
	SealedValue la;
	la.emplaceBack();
	la.emplaceBack();
	dcool::vigor::Sealer<SealedValue> sealer(dcool::core::inPlace, dcool::core::move(la));
	auto observer = [&dcoolTestRecord](SealedValue const& value) {
		DCOOL_TEST_EXPECT(value.length() == 2);
	};
	auto transaction = [](SealedValue value) -> SealedValue {
		value.emplaceBack();
		return value;
	};
	sealer.runObserver(observer);
	try {
		sealer.runTransaction(transaction);
	} catch (...) {
	}
	sealer.runObserver(observer);
}
