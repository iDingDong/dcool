#include <dcool/container.hpp>
#include <dcool/test.hpp>
#include <dcool/vigor.hpp>

DCOOL_TEST_CASE(dcoolVigor, sealerBasic) {
	struct Axception {
	};

	struct A {
		A() = default;

		A(A const& other) {
			if (countBeforeThrow() > 0) {
				--(countBeforeThrow());
			} else {
				throw Axception();
			}
		};

		auto operator =(A const& other) -> A& {
			if (countBeforeThrow() > 0) {
				--(countBeforeThrow());
			} else {
				throw Axception();
			}
			return *this;
		}

		static consteval auto initialCountBeforeThrow() -> int {
			return 8;
		}

		static void resetCountBeforeThrow() {
			countBeforeThrow() = initialCountBeforeThrow();
		}

		static int& countBeforeThrow() {
			static int count = initialCountBeforeThrow();
			return count;
		}
	};

	using SealedValue = dcool::container::StaticList<A, 3>;
	SealedValue la;
	la.emplaceBack();
	la.emplaceBack();
	dcool::vigor::Sealer<SealedValue> sealer(dcool::core::inPlace, dcool::core::move(la));
	auto observer = [DCOOL_TEST_CAPTURE_CONTEXT](SealedValue const& value) {
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
	A::resetCountBeforeThrow();
}
