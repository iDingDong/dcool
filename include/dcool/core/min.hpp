#ifndef DCOOL_CORE_MIN_HPP_INCLUDED_
#	define DCOOL_CORE_MIN_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>

#	include <algorithm>

namespace dcool::core {
	template <TotallyOrdered T_> constexpr auto min(T_ const& first_) -> T_ const& {
		return first_;
	}

	template <
		::dcool::core::TotallyOrdered T_, ::dcool::core::TotallyOrdered... Ts_
	> constexpr auto min(T_ const& first_, Ts_ const&... rests_) -> T_ const& {
		return ::std::min(first_, ::dcool::core::min(rests_...));
	}
}

#endif
