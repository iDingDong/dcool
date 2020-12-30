#ifndef DCOOL_CORE_RANGE_HPP_INCLUDED_
#	define DCOOL_CORE_RANGE_HPP_INCLUDED_ 1

#	include <iterator>
#	include <ranges>

namespace dcool::core {
	template <typename T_> concept InputIterator = ::std::input_iterator<T_>;
	template <typename T_, typename ToOutputT_> concept OutputIterator = ::std::output_iterator<T_, ToOutputT_>;
	template <typename T_> concept ForwardIterator = ::std::forward_iterator<T_>;
	template <typename T_> concept RandomAccessRange = ::std::ranges::random_access_range<T_>;
	template <typename T_> concept BorrowedRange = ::std::ranges::borrowed_range<T_>;

	using ContiguousIteratorTag = ::std::contiguous_iterator_tag;
}

#endif
