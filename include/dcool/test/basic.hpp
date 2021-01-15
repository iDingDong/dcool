#ifndef DCOOL_TEST_BASIC_HPP_INCLUDED_
#	define DCOOL_TEST_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <string_view>
#	include <chrono>

namespace dcool::test {
	using Count = ::dcool::core::Length;
	using Name = ::std::string_view;
	using FileName = ::dcool::test::Name;
	using LineNumber = ::dcool::core::UnsignedMaxInteger;
	using Clock = ::std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using Duration = Clock::duration;
}

#endif
