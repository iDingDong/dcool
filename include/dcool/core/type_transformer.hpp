#ifndef DCOOL_CORE_TYPE_TRANSFORMER_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_TRANSFORMER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

#	include <type_traits>

namespace dcool::core {
	template <typename T_> using UndeducedType = ::std::type_identity_t<T_>;

	template <typename T_> using LvalueReferenceAddedType = ::std::add_lvalue_reference_t<T_>;
	template <typename T_> using ConstAddedType = ::std::add_const_t<T_>;
	template <typename T_> using ReferenceRemovedType = ::std::remove_reference_t<T_>;
	template <typename T_> using ConstRemovedType = ::std::remove_const_t<T_>;
	template <typename T_> using VolatileRemovedType = ::std::remove_volatile_t<T_>;
	template <typename T_> using QualifierRemovedType = ::std::remove_cv_t<T_>;
	template <typename T_> using QualifiedReferenceRemovedType = ::std::remove_cvref_t<T_>;

	namespace detail_ {
		template <typename ToQualifyT_, typename ImitatedT_> struct QualifyConstAs_ {
			using Result_ = ::dcool::core::ConstRemovedType<ToQualifyT_>;
		};

		template <typename ToQualifyT_, typename ImitatedT_> struct QualifyConstAs_<ToQualifyT_, ImitatedT_ const> {
			using Result_ = ToQualifyT_ const;
		};

		template <typename ToQualifyT_, typename ImitatedT_> struct QualifyVolatileAs_ {
			using Result_ = ::dcool::core::VolatileRemovedType<ToQualifyT_>;
		};

		template <typename ToQualifyT_, typename ImitatedT_> struct QualifyVolatileAs_<ToQualifyT_, ImitatedT_ volatile> {
			using Result_ = ToQualifyT_ volatile;
		};
	}

	template <
		typename T_, typename ImitatedT_
	> using IdenticallyConstType = ::dcool::core::detail_::QualifyConstAs_<T_, ImitatedT_>::Result_;

	template <
		typename T_, typename ImitatedT_
	> using IdenticallyVolatileType = ::dcool::core::detail_::QualifyVolatileAs_<T_, ImitatedT_>::Result_;

	template <typename T_, typename ImitatedT_> using IdenticallyQualifiedType = ::dcool::core::IdenticallyVolatileType<
		::dcool::core::IdenticallyConstType<T_, ImitatedT_>, ImitatedT_
	>;

	namespace detail_ {
		template <typename ToQualifyT_, typename ImitatedT_> struct ReferedAs_ {
			using Result_ = ::dcool::core::ReferenceRemovedType<ToQualifyT_>;
		};

		template <typename ToQualifyT_, typename ImitatedT_> struct ReferedAs_<ToQualifyT_, ImitatedT_&> {
			using Result_ = ::dcool::core::ReferenceRemovedType<ToQualifyT_>&;
		};

		template <typename ToQualifyT_, typename ImitatedT_> struct ReferedAs_<ToQualifyT_, ImitatedT_&&> {
			using Result_ = ::dcool::core::ReferenceRemovedType<ToQualifyT_>&&;
		};
	}

	template <
		typename T_, typename ImitatedT_
	> using IdenticallyReferedType = ::dcool::core::detail_::ReferedAs_<T_, ImitatedT_>::Result_;

	template <
		typename T_, typename ImitatedT_
	> using IdenticallyQualifiedlyReferedType = ::dcool::core::IdenticallyReferedType<
		::dcool::core::IdenticallyQualifiedType<
			::dcool::core::ReferenceRemovedType<T_>, ::dcool::core::ReferenceRemovedType<ImitatedT_>
		>,
		ImitatedT_
	>;

	namespace detail_ {
		template <typename T_> struct ConstantizePointed_ {
			using Result_ = T_;
		};

		template <typename T_> struct ConstantizePointed_<T_*> {
			using Result_ = T_ const*;
		};
	}

	template <typename T_> using PointedConstantizedType = ::dcool::core::IdenticallyQualifiedType<
		typename ::dcool::core::detail_::ConstantizePointed_<::dcool::core::QualifierRemovedType<T_>>::Result_, T_
	>;

	template <::dcool::core::Boolean predicateC_, typename TrueT_, typename FalseT_> using ConditionalType = ::std::conditional_t<
		predicateC_, TrueT_, FalseT_
	>;

	template <typename FirstT_, typename SecondT_> using SmallerType = ::dcool::core::ConditionalType<
		sizeof(FirstT_) <= sizeof(SecondT_), FirstT_, SecondT_
	>;

	template <auto keyC_, typename ValueT_> struct SelectionCase {
		static constexpr decltype(keyC_) key = keyC_;
		using Value = ValueT_;
	};

	namespace detail_ {
		template <typename T_, typename KeyT_> constexpr ::dcool::core::Boolean isSelectionCaseOf_ = false;

		template <
			typename KeyT_, KeyT_ keyC_, typename ValueT_
		> constexpr ::dcool::core::Boolean isSelectionCaseOf_<::dcool::core::SelectionCase<keyC_, ValueT_>, KeyT_> = true;

		template <typename T_, typename KeyT_> concept SelectionCaseOf_ = ::dcool::core::detail_::isSelectionCaseOf_<T_, KeyT_>;

		template <
			auto keyC_, typename DefaultT_, ::dcool::core::detail_::SelectionCaseOf_<decltype(keyC_)>... CaseTs_
		> struct Select_ {
			using Result_ = DefaultT_;
		};

		template <
			auto keyC_,
			typename DefaultT_,
			::dcool::core::detail_::SelectionCaseOf_<decltype(keyC_)> FirstCaseT_,
			::dcool::core::detail_::SelectionCaseOf_<decltype(keyC_)>... CaseTs_
		> struct Select_<keyC_, DefaultT_, FirstCaseT_, CaseTs_...> {
			using Result_ = ::dcool::core::detail_::Select_<keyC_, DefaultT_, CaseTs_...>::Result_;
		};

		template <
			auto keyC_,
			typename DefaultT_,
			typename FirstValueT_,
			::dcool::core::detail_::SelectionCaseOf_<decltype(keyC_)>... CaseTs_
		> struct Select_<keyC_, DefaultT_, ::dcool::core::SelectionCase<keyC_, FirstValueT_>, CaseTs_...> {
			using Result_ = FirstValueT_;
		};
	}

	template <
		auto keyC_, typename DefaultT_, ::dcool::core::detail_::SelectionCaseOf_<decltype(keyC_)>... CaseTs_
	> using SelectType = ::dcool::core::detail_::Select_<keyC_, DefaultT_, CaseTs_...>::Result_;

	namespace detail_ {
		template <::dcool::core::Index indexC_, typename... Ts_> struct VariadicElement_;

		template <::dcool::core::Index indexC_, typename T_, typename... Ts_> struct VariadicElement_<indexC_, T_, Ts_...> {
			using Result_ = ::dcool::core::detail_::VariadicElement_<indexC_ - 1, Ts_...>::Result_;
		};

		template <typename T_, typename... Ts_> struct VariadicElement_<0, T_, Ts_...> {
			using Result_ = T_;
		};
	}

	template <::dcool::core::Index indexC_, typename... Ts_> requires (indexC_ < sizeof...(Ts_)) using VariadicElementType =
		::dcool::core::detail_::VariadicElement_<indexC_, Ts_...>::Result_
	;

	template <typename T_> using DecayedType = ::std::decay_t<T_>;

	template <typename... T_> using CommonType = ::std::common_type_t<T_...>;

	template <typename LeftT_, typename RightT_> using UseRightType = RightT_;

	template <typename LeftT_, auto rightC_> constexpr decltype(rightC_) useRightValue = rightC_;

	namespace detail_ {
		struct VoidStub_;

		template <typename T_> struct StubReferenceableHelper_ {
			using Result_ = T_;
		};

		template <> struct StubReferenceableHelper_<void> {
			using Result_ = ::dcool::core::detail_::VoidStub_;
		};
	}

	template <typename T_> using StubReferenceableType = ::dcool::core::IdenticallyQualifiedType<
		typename ::dcool::core::detail_::StubReferenceableHelper_<::dcool::core::ReferenceRemovedType<T_>>::Result_, T_
	>;
}

#endif
