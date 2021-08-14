#ifndef DCOOL_CORE_ITERATION_HPP_INCLUDED_
#	define DCOOL_CORE_ITERATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	template <typename InitialT_, typename IterationT_> constexpr auto applyNIteration(
		InitialT_ initial_, IterationT_&& iteration_, ::dcool::core::Length count_
	) -> InitialT_ {
		while (count_ > 0) {
			initial_ = iteration_(initial_);
			--count_;
		}
		return initial_;
	}
}

#endif
