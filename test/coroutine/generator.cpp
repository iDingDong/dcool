#include <dcool/core.hpp>
#include <dcool/coroutine.hpp>
#include <dcool/test.hpp>

#include <chrono>
#include <thread>

DCOOL_TEST_CASE(dcoolCoroutine, generatorBasics) {
	auto coroutine = []() -> dcool::coroutine::Generator<int> {
		for (int i = -2; i <= 2; ++i) {
			co_yield i;
		}
	};

	{
		int expected = -2;
		for (auto i: coroutine()) {
			DCOOL_TEST_EXPECT(i == expected);
			++expected;
		}
	}

	{
		auto generator = coroutine();
		for (int i = -2; i <= 2; ++i) {
			auto result = generator.tryGenerate();
			if (!(result.valid())) {
				DCOOL_TEST_EXPECT(false);
				break;
			}
			DCOOL_TEST_EXPECT(result.access() == i);
		}
		auto result = generator.tryGenerate();
		DCOOL_TEST_EXPECT(!(result.valid()));
	}
}

DCOOL_TEST_CASE(dcoolCoroutine, referenceGenerator) {
	int i = 1;
	int j = 2;
	auto coroutine = [&i, &j]() -> dcool::coroutine::Generator<int&> {
		co_yield i;
		co_yield j;
	};

	auto generator = coroutine();
	auto first = generator.tryGenerate();
	if (first.valid()) {
		auto address = ::dcool::core::addressOf(first.access());
		DCOOL_TEST_EXPECT(address == &i);
	} else {
		DCOOL_TEST_EXPECT(false);
	}
	auto second = generator.tryGenerate();
	if (second.valid()) {
		DCOOL_TEST_EXPECT(::dcool::core::addressOf(second.access()) == &j);
	} else {
		DCOOL_TEST_EXPECT(false);
	}
	auto third = generator.tryGenerate();
	DCOOL_TEST_EXPECT(!(third.valid()));
}
