#ifndef DCOOL_CORE_EXCEPTION_HPP_INCLUDED_
#	define DCOOL_CORE_EXCEPTION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/execution.hpp>
#	include <dcool/core/member_detector.hpp>

#	include <exception>
#	include <stdexcept>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core, HasExceptionSafetyStrategy, extractedExceptionSafetyStrategyValue, exceptionSafetyStrategy
)

namespace dcool::core {
	enum class SusbicionStrategy {
		undefined,
		terminate,
		exception,
		ignore
	};

	enum class UnsoundStrategy {
		undefined,
		terminate,
		exception
	};

#	if DCOOL_DEPENDENCY_BUG_1
	// Workaround for a compiler bug.
	// See document/dependency_bugs#Bug_1 for more details.
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
	struct ExceptionSafetyStrategy {
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

	inline constexpr ::dcool::core::ExceptionSafetyStrategy defaultExceptionSafetyStrategy =
		::dcool::core::exceptionSafetyStrategy<>
	;

	template <typename T_> constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategyOf =
		::dcool::core::extractedExceptionSafetyStrategyValue<T_>(::dcool::core::defaultExceptionSafetyStrategy)
	;

	template <::dcool::core::ExceptionSafetyStrategy strategyC_> constexpr void goWeak() noexcept {
		if constexpr (::dcool::core::strongOrTerminate(strategyC_)) {
			::dcool::core::terminate();
		}
	}

	using ExceptionPointer = ::std::exception_ptr;

	template <typename ExceptionT_> auto makeExceptionPointer(ExceptionT_ exception_) noexcept {
		return ::std::make_exception_ptr(::dcool::core::move(exception_));
	}

	inline auto currentException() noexcept -> ::dcool::core::ExceptionPointer {
		return ::std::current_exception();
	}

	[[noreturn]] inline void rethrow(::dcool::core::ExceptionPointer pointer_) {
		::std::rethrow_exception(pointer_);
	}

	inline void rethrowIfValid(::dcool::core::ExceptionPointer pointer_) {
		if (pointer_) {
			::dcool::core::rethrow(pointer_);
		}
	}

	inline void rethrowAndConsumeIfValid(::dcool::core::ExceptionPointer& pointer_) {
		if (pointer_) {
			::dcool::core::ExceptionPointer toThrow_ = pointer_;
			pointer_ = ::dcool::core::nullPointer;
			::dcool::core::rethrow(toThrow_);
		}
	}

	template <typename... Ts_> class ExceptionSafetyDowngrade : ::std::runtime_error {
		private: using Super_ = ::std::runtime_error;

		private: ExceptionPointer m_underlying_;

		public: ExceptionSafetyDowngrade(
			ExceptionPointer pointer_ = ::dcool::core::currentException(), const char* what_ = "Exception safety downgraded."
		): Super_(what_), m_underlying_(pointer_) {
		}

		public: [[noreturn]] void rethrowUnderlying() const {
			::dcool::core::rethrow(this->m_underlying_);
		}
	};

	using OutOfRange = ::std::out_of_range;

	using RangeError = ::std::range_error;
}

#endif
