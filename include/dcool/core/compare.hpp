#ifndef DCOOL_CORE_COMPARE_HPP_INCLUDED_
#	define DCOOL_CORE_COMPARE_HPP_INCLUDED_ 1

#	include <concepts>
#	include <compare>

namespace dcool::core {
	template <typename T_> concept EqualityComparable = ::std::equality_comparable<T_>;
	template <typename T_> concept TotallyOrdered = ::std::totally_ordered<T_>;

	using PartialOrdering = ::std::partial_ordering;
	using StrongOrdering = ::std::strong_ordering;
}

#endif
