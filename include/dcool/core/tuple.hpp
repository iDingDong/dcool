#ifndef DCOOL_CORE_TUPLE_HPP_INCLUDED_
#	define DCOOL_CORE_TUPLE_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>

#	include <tuple>

namespace dcool::core {
	template <typename... Ts_> using Tuple = ::std::tuple<Ts_...>;

	using ::std::get;
	using ::std::apply;

	template <typename... Ts_> constexpr auto makeTuple(Ts_&&... elements_) {
		return ::std::make_tuple(::dcool::core::forward<Ts_>(elements_)...);
	}
}

#endif
