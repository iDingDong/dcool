#ifndef DCOOL_UTILITY_BASIC_HPP_INCLUDED_
#	define DCOOL_UTILITY_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/core.h>

#	include <chrono>

namespace dcool::utility {
	using CTimeCorrespondingDuration = ::std::chrono::nanoseconds;
	using CTimeCorrespondingClock = ::std::chrono::system_clock;

	constexpr auto toDuration(DCOOL_CORE_Duration const& duration_) noexcept -> CTimeCorrespondingDuration {
		return ::std::chrono::seconds(duration_.seconds) + ::std::chrono::nanoseconds(duration_.nanoseconds);
	}

	template <typename DurationT_> constexpr auto toCDuration(
		DurationT_ const& duration_
	) noexcept -> DCOOL_CORE_Duration {
		auto correspondingDuration_ = ::std::chrono::duration_cast<CTimeCorrespondingDuration>(duration_);
		return DCOOL_CORE_Duration{
			.seconds = static_cast<::std::time_t>(correspondingDuration_.count() / 1'000'000'000),
			.nanoseconds = static_cast<::std::uint32_t>(correspondingDuration_.count() % 1'000'000'000)
		};
	}

	constexpr auto toCTimePoint(CTimeCorrespondingClock::time_point const& timePoint_) -> DCOOL_CORE_TimePoint {
		return DCOOL_CORE_TimePoint{
			.sinceEpoch = ::dcool::utility::toCDuration(timePoint_.time_since_epoch())
		};
	}

	template <typename ClockT_ = CTimeCorrespondingClock> constexpr auto cTimeNow() noexcept -> DCOOL_CORE_TimePoint {
		return ::dcool::utility::toCTimePoint(ClockT_::now().time_since_epoch());
	}


	template <typename ClockT_ = CTimeCorrespondingClock> constexpr auto toClockTimePoint(
		DCOOL_CORE_TimePoint const& timePoint
	) noexcept -> ClockT_::time_point {
		return typename ClockT_::time_point(::dcool::utility::toDuration(timePoint.sinceEpoch));
	}
}

#endif
