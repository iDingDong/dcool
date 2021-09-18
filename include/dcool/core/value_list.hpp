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

		template <::dcool::core::ValueList ValuesT_, auto newItemC_> struct PushBackToValueList_;

		template <auto newItemC_, auto... itemCs_> struct PushBackToValueList_<::dcool::core::Values<itemCs_...>, newItemC_> {
			using Result_ = ::dcool::core::Values<itemCs_..., newItemC_>;
		};

		template <::dcool::core::ValueList... ListTs_> struct ConcatValueLists_ {
			using Result_ = ::dcool::core::Values<>;
		};

		template <::dcool::core::ValueList ListT_> struct ConcatValueLists_<ListT_> {
			using Result_ = ListT_;
		};

		template <::dcool::core::ValueList FirstListT_> struct ConcatValueLists_<FirstListT_, ::dcool::core::Values<>> {
			using Result_ = FirstListT_;
		};

		template <
			::dcool::core::ValueList FirstListT_, auto secondListFrontC_, auto... secondListRestCs_
		> struct ConcatValueLists_<FirstListT_, ::dcool::core::Values<secondListFrontC_, secondListRestCs_...>> {
			using Result_ = ::dcool::core::detail_::ConcatValueLists_<
				typename ::dcool::core::detail_::PushBackToValueList_<FirstListT_, secondListFrontC_>::Result_,
				::dcool::core::Values<secondListRestCs_...>
			>::Result_;
		};

		template <
			::dcool::core::ValueList FirstListT_, ::dcool::core::ValueList SecondListT_, ::dcool::core::ValueList... RestListTs_
		> struct ConcatValueLists_<FirstListT_, SecondListT_, RestListTs_...> {
			using Result_ = ::dcool::core::detail_::ConcatValueLists_<
				typename ::dcool::core::detail_::ConcatValueLists_<FirstListT_, SecondListT_>::Result_,
				RestListTs_...
			>::Result_;
		};

		template <
			::dcool::core::ValueList ScannedT_, ::dcool::core::ValueList ToScanT_, auto toRemoveC_
		> struct RemoveAllFromValueListHelper_ {
			using Result_ = ScannedT_;
		};

		template <
			::dcool::core::ValueList ScannedT_, auto toRemoveC_, auto firstC_, auto... restCs_
		> struct RemoveAllFromValueListHelper_<ScannedT_, ::dcool::core::Values<firstC_, restCs_...>, toRemoveC_> {
			using Result_ = ::dcool::core::detail_::RemoveAllFromValueListHelper_<
				typename ::dcool::core::detail_::PushBackToValueList_<ScannedT_, firstC_>::Result_,
				dcool::core::Values<restCs_...>,
				toRemoveC_
			>::Result_;
		};

		template <
			::dcool::core::ValueList ScannedT_, auto toRemoveC_, auto... restCs_
		> struct RemoveAllFromValueListHelper_<ScannedT_, ::dcool::core::Values<toRemoveC_, restCs_...>, toRemoveC_> {
			using Result_ = ::dcool::core::detail_::RemoveAllFromValueListHelper_<
				ScannedT_, dcool::core::Values<restCs_...>, toRemoveC_
			>::Result_;
		};

		template <
			::dcool::core::ValueList ListT_, auto toRemoveC_
		> using RemoveAllFromValueListType_ = ::dcool::core::detail_::RemoveAllFromValueListHelper_<
			::dcool::core::Values<>, ListT_, toRemoveC_
		>::Result_;
	}

	template <
		auto initialC_, auto iterationC_, ::dcool::core::Length lengthC_
	> using ValueSequence = ::dcool::core::detail_::ValueSequenceHelper_<initialC_, iterationC_, lengthC_>::Result_;

	template <auto... cs_> struct Values {
		private: using Self_ = Values<cs_...>;

		public: template <auto... cs__> using PushFront = ::dcool::core::Values<cs_..., cs__...>;
		public: template <auto... cs__> using PushBack = ::dcool::core::Values<cs__..., cs_...>;
		public: template <auto c__> using RemoveAll = ::dcool::core::detail_::RemoveAllFromValueListType_<Self_, c__>;
	};
}

#endif
