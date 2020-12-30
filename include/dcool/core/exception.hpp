#ifndef DCOOL_CORE_EXCEPTION_HPP_INCLUDED_
#	define DCOOL_CORE_EXCEPTION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/member_detector.hpp>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core, HasExceptionSafetyStrategy, extractedExceptionSafetyStrategyValue, exceptionSafetyStrategy
)

namespace dcool::core {
	struct ExceptionSafetyStrategy {
		::dcool::core::Boolean atAnyCost: 1;
		::dcool::core::Boolean strongOrTerminate: 1;
	};

	constexpr ::dcool::core::ExceptionSafetyStrategy defaultExceptionSafetyStrategy = {
		.atAnyCost = false, .strongOrTerminate = false
	};

	template <typename T_> constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategyOf =
		::dcool::core::extractedExceptionSafetyStrategyValue<T_>(defaultExceptionSafetyStrategy)
	;
}

#endif
