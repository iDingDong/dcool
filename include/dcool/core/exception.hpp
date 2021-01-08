#ifndef DCOOL_CORE_EXCEPTION_HPP_INCLUDED_
#	define DCOOL_CORE_EXCEPTION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/member_detector.hpp>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core, HasExceptionSafetyStrategy, extractedExceptionSafetyStrategyValue, exceptionSafetyStrategy
)

namespace dcool::core {
#	if defined(__GNUC__)
	// TODO: After GCC bug 95291 gets fixed, withdraw this workaround.
	using ExceptionSafetyStrategy = unsigned char;

	constexpr auto atAnyCost(::dcool::core::ExceptionSafetyStrategy strategy_) noexcept -> ::dcool::core::Boolean {
		return strategy_ & 0b01;
	}

	constexpr auto strongOrTerminate(::dcool::core::ExceptionSafetyStrategy strategy_) noexcept -> ::dcool::core::Boolean {
		return strategy_ & 0b10;
	}

	template <
		::dcool::core::Boolean atAnyCostC_ = false,
		::dcool::core::Boolean strongOrTerminateC_ = false
	> constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
		(atAnyCostC_ ? 0b01 : 0) | (strongOrTerminateC_ ? 0b10 : 0)
	;
#	else
	using ExceptionSafetyStrategy {
		private: using Self_ = ExceptionSafetyStrategy;

		public: ::dcool::core::Boolean atAnyCost: 1;
		public: ::dcool::core::Boolean strongOrTerminate: 1;

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	constexpr auto atAnyCost(::dcool::core::ExceptionSafetyStrategy strategy_) noexcept -> ::dcool::core::Boolean {
		return strategy_.atAnyCost;
	}

	constexpr auto strongOrTerminate(::dcool::core::ExceptionSafetyStrategy strategy_) noexcept -> ::dcool::core::Boolean {
		return strategy_.strongOrTerminate;
	}

	template <
		::dcool::core::Boolean atAnyCostC_ = false,
		::dcool::core::Boolean strongOrTerminateC_ = false
	> constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = {
		.atAnyCost = atAnyCostC_, .strongOrTerminate = strongOrTerminateC_
	};
#	endif

	constexpr ::dcool::core::ExceptionSafetyStrategy defaultExceptionSafetyStrategy = exceptionSafetyStrategy<>;

	template <typename T_> constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategyOf =
		::dcool::core::extractedExceptionSafetyStrategyValue<T_>(defaultExceptionSafetyStrategy)
	;

	template <::dcool::core::ExceptionSafetyStrategy strategyC_> constexpr void goWeak() noexcept {
		if constexpr (::dcool::core::strongOrTerminate(strategyC_)) {
			::dcool::core::terminate();
		}
	}
}

#endif
