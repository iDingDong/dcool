#ifndef DCOOL_TEST_BASIC_HPP_INCLUDED_
#	define DCOOL_TEST_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <string>
#	include <string_view>
#	include <chrono>

namespace dcool::test {
	using Count = ::dcool::core::Length;
	using Name = ::std::string_view;
	using Message = ::std::string;
	using Clock = ::std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Duration = Clock::duration;
}

#endif
