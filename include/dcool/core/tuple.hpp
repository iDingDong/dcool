#ifndef DCOOL_CORE_TUPLE_HPP_INCLUDED_
#	define DCOOL_CORE_TUPLE_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	include <tuple>

namespace dcool::core {
	template <dcool::core::Object... ValueTs_> using Tuple = ::std::tuple<ValueTs_...>;
}

#endif
