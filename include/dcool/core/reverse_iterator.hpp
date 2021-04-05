#ifndef DCOOL_CORE_REVERSE_ITERATOR_HPP_INCLUDED_
#	define DCOOL_CORE_REVERSE_ITERATOR_HPP_INCLUDED_ 1

#	include <iterator>

namespace dcool::core {
	template <typename IteratorT_> using ReverseIterator = ::std::reverse_iterator<IteratorT_>;
}

#endif
