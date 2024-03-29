#ifndef DCOOL_CORE_TYPE_LIST_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	template <typename... Ts_> struct Types;

	template <typename T_> constexpr ::dcool::core::Boolean isTypeList = false;

	template <typename... Ts_> constexpr ::dcool::core::Boolean isTypeList<::dcool::core::Types<Ts_...>> = true;

	template <typename T_> concept TypeList = ::dcool::core::isTypeList<T_>;

	namespace detail_ {
		template <typename ListT_, typename NewItemT_> struct PushBackToTypeList_;

		template <typename NewItemT_, typename... ItemTs_> struct PushBackToTypeList_<::dcool::core::Types<ItemTs_...>, NewItemT_> {
			using Result_ = ::dcool::core::Types<ItemTs_..., NewItemT_>;
		};

		template <::dcool::core::TypeList... ListTs_> struct ConcatTypeLists_ {
			using Result_ = ::dcool::core::Types<>;
		};

		template <::dcool::core::TypeList ListT_> struct ConcatTypeLists_<ListT_> {
			using Result_ = ListT_;
		};

		template <::dcool::core::TypeList FirstListT_> struct ConcatTypeLists_<FirstListT_, ::dcool::core::Types<>> {
			using Result_ = FirstListT_;
		};

		template <
			::dcool::core::TypeList FirstListT_, typename SecondListFrontT_, typename... SecondListRestTs_
		> struct ConcatTypeLists_<FirstListT_, ::dcool::core::Types<SecondListFrontT_, SecondListRestTs_...>> {
			using Result_ = ::dcool::core::detail_::ConcatTypeLists_<
				typename ::dcool::core::detail_::PushBackToTypeList_<FirstListT_, SecondListFrontT_>::Result_,
				::dcool::core::Types<SecondListRestTs_...>
			>::Result_;
		};

		template <
			::dcool::core::TypeList FirstListT_, ::dcool::core::TypeList SecondListT_, ::dcool::core::TypeList... RestListTs_
		> struct ConcatTypeLists_<FirstListT_, SecondListT_, RestListTs_...> {
			using Result_ = ::dcool::core::detail_::ConcatTypeLists_<
				typename ::dcool::core::detail_::ConcatTypeLists_<FirstListT_, SecondListT_>::Result_,
				RestListTs_...
			>::Result_;
		};

		template <::dcool::core::Index indexC_, typename... Ts_> struct TypeAtHelper_;

		template <::dcool::core::Index indexC_, typename FirstT_, typename... RestTs_> struct TypeAtHelper_<
			indexC_, FirstT_, RestTs_...
		> {
			using Result_ = ::dcool::core::detail_::TypeAtHelper_<indexC_ - 1, RestTs_...>::Result_;
		};

		template <typename FirstT_, typename... RestTs_> struct TypeAtHelper_<0, FirstT_, RestTs_...> {
			using Result_ = FirstT_;
		};

		template <typename ExpectedT_, typename... Ts_> struct FindIndexHelper_ {
			static constexpr ::dcool::core::Index result_ = 0;
		};

		template <typename ExpectedT_, typename FirstT_, typename... RestTs_> struct FindIndexHelper_<
			ExpectedT_, FirstT_, RestTs_...
		> {
			static constexpr ::dcool::core::Index result_ =
				::dcool::core::detail_::FindIndexHelper_<ExpectedT_, RestTs_...>::result_ + 1
			;
		};

		template <typename ExpectedT_, typename... RestTs_> struct FindIndexHelper_<
			ExpectedT_, ExpectedT_, RestTs_...
		> {
			static constexpr ::dcool::core::Index result_ = 0;
		};
	}

	template <typename... Ts_> struct Types {
		private: using Self_ = Types<Ts_...>;

		public: template <::dcool::core::Index indexC_> using At = ::dcool::core::detail_::TypeAtHelper_<indexC_, Ts_...>::Result_;
		public: template <typename... T__> using PushFront = ::dcool::core::Types<T__..., Ts_...>;
		public: template <typename... T__> using PushBack = ::dcool::core::Types<Ts_..., T__...>;
		public: template <::dcool::core::TypeList... OtherTs__> using Append = ::dcool::core::detail_::ConcatTypeLists_<
			Self_, OtherTs__...
		>;
		public: template <typename T__> static constexpr ::dcool::core::Index findIndex = ::dcool::core::detail_::FindIndexHelper_<
			T__, Ts_...
		>::result_;
	};
}

#endif
