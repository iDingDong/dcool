#ifndef DCOOL_CORE_FOR_EACH_HPP_INCLUDED_
#	define DCOOL_CORE_FOR_EACH_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/container.hpp>

#	include <algorithm>
#	include <functional>

namespace dcool::algorithm {
	namespace detail_ {
		template <
			::dcool::core::FormOfExtentedExecutionPolicy PolicyT_,
			::dcool::core::InputIterator IteratorT_,
			::dcool::core::Invocable<::dcool::core::IteratorReferenceType<IteratorT_>> FunctionT_
		> void forEach(PolicyT_&& policy_, IteratorT_ begin_, IteratorT_ end_, FunctionT_&& function_) {
			while (begin_ != end_) {
				::dcool::core::invoke(function_, ::dcool::core::dereference(begin_));
				++begin_;
			}
		}

		template <
			typename PolicyT_,
			::dcool::core::InputIterator IteratorT_,
			::dcool::core::Invocable<::dcool::core::IteratorReferenceType<IteratorT_>> FunctionT_
		> void forEach(PolicyT_&& policy_, IteratorT_ begin_, IteratorT_ end_, FunctionT_&& function_) {
			return ::std::for_each(::dcool::core::forward<PolicyT_>(policy_), begin_, end_, [function_](auto& current_) mutable {
				::dcool::core::invoke(function_, current_);
			});
		}
	}

	template <
		::dcool::core::FormOfExecutionPolicy PolicyT_,
		::dcool::core::InputIterator IteratorT_,
		::dcool::core::Invocable<::dcool::core::IteratorReferenceType<IteratorT_>> FunctionT_
	> void forEach(PolicyT_&& policy_, IteratorT_ begin_, IteratorT_ end_, FunctionT_&& function_) {
#	if defined(__GLIBCXX__)
		if constexpr ((!::dcool::core::FormOfSequencedPolicy<PolicyT_>) || (!::dcool::core::RandomAccessIterator<IteratorT_>)) {
			using Value_ = ::dcool::core::IteratorValueType<IteratorT_>;
			using ListValue_ = ::std::reference_wrapper<Value_>;
			::dcool::container::List<ListValue_> list_(::dcool::core::rangeInput, begin_, end_);
			::dcool::algorithm::detail_::forEach(
				::dcool::core::forward<PolicyT_>(policy_), list_.begin(), list_.end(), [&function_](Value_& current_) {
					::dcool::core::invoke(function_, current_);
				}
			);
			return;
		}
#	endif
		::dcool::algorithm::detail_::forEach(
			::dcool::core::forward<PolicyT_>(policy_), begin_, end_, ::dcool::core::forward<FunctionT_>(function_)
		);
	}

	template <
		::dcool::core::InputIterator IteratorT_,
		::dcool::core::Invocable<::dcool::core::IteratorReferenceType<IteratorT_>> FunctionT_
	> void forEach(IteratorT_ begin_, IteratorT_ end_, FunctionT_&& function_) {
		::dcool::algorithm::forEach(begin_, end_, ::dcool::core::forward<FunctionT_>(function_));
	}
}

#endif
