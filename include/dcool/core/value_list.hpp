#ifndef DCOOL_CORE_VALUE_LIST_HPP_INCLUDED_
#	define DCOOL_CORE_VALUE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	template <auto... cs_> struct Values;

	template <typename T_> constexpr ::dcool::core::Boolean isValueList = false;

	template <auto... cs_> constexpr ::dcool::core::Boolean isValueList<::dcool::core::Values<cs_...>> = true;

	template <typename T_> concept ValueList = ::dcool::core::isValueList<T_>;

	template <typename T_, typename ValueT_> constexpr ::dcool::core::Boolean isValueListOf = false;

	template <typename T_, T_... cs_> constexpr ::dcool::core::Boolean isValueListOf<T_, ::dcool::core::Values<cs_...>> = true;

	template <typename T_, typename ValueT_> concept ValueListOf = ::dcool::core::isValueListOf<T_, ValueT_>;

	namespace detail_ {
		template <
			auto currentC_, auto iterationC_, ::dcool::core::Length remainingLengthC_, auto... valueCs_
		> struct ValueSequenceHelper_ {
			using Result_ = ::dcool::core::detail_::ValueSequenceHelper_<
				static_cast<decltype(currentC_)>(iterationC_(currentC_)), iterationC_, remainingLengthC_ - 1, valueCs_..., currentC_
			>::Result_;
		};

		template <auto currentC_, auto iterationC_, auto... valueCs_> struct ValueSequenceHelper_<
			currentC_, iterationC_, 0, valueCs_...
		> {
			using Result_ = ::dcool::core::Values<valueCs_...>;
		};
	}

	template <
		auto initialC_, auto iterationC_, ::dcool::core::Length lengthC_
	> using ValueSequence = ::dcool::core::detail_::ValueSequenceHelper_<initialC_, iterationC_, lengthC_>::Result_;
}

#endif
