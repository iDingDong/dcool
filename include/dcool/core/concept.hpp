#ifndef DCOOL_CORE_CONCEPT_HPP_INCLUDED_
#	define DCOOL_CORE_CONCEPT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/type_transformer.hpp>

#	include <concepts>
#	include <type_traits>

namespace dcool::core {
	template <auto valueC_> using RequiresConsteval = void;

	namespace detail_ {
		template <typename... Ts_> struct IsSameHelper_ {
			static constexpr ::dcool::core::Boolean result_ = true;
		};

		template <typename FirstT_, typename SecondT_, typename... Ts_> struct IsSameHelper_<FirstT_, SecondT_, Ts_...> {
			static constexpr ::dcool::core::Boolean result_ =
				::std::is_same_v<FirstT_, SecondT_> && ::dcool::core::detail_::IsSameHelper_<FirstT_, Ts_...>::result_
			;
		};
	}

	template <typename... Ts_> constexpr ::dcool::core::Boolean isSame = ::dcool::core::detail_::IsSameHelper_<Ts_...>::result_;

	template <typename T_, typename... Ts_> concept SameAs = ::dcool::core::isSame<T_, Ts_...>;

	template <typename T_, typename TargetT_> concept ConvertibleTo = ::std::convertible_to<T_, TargetT_>;

	template <typename T_, typename OtherT_> concept ConvertibleBetween =
		::dcool::core::ConvertibleTo<T_, OtherT_> && ::dcool::core::ConvertibleTo<OtherT_, T_>
	;

	template <typename T_> concept NonVoid = !::dcool::core::SameAs<T_, void>;

	template <typename T_> concept Const = ::std::is_const_v<T_>;

	template <typename T_> concept Object = ::std::is_object_v<T_>;

	template <typename T_> concept Reference = ::std::is_reference_v<T_>;

	template <typename T_> concept ConstReference = Const<::dcool::core::ReferenceRemovedType<T_>> && Reference<T_>;

	template <typename T_> concept Pointer = ::std::is_pointer_v<T_>;

	template <typename T_> concept Class = ::std::is_class_v<T_>;

	template <typename T_> concept Integer = ::std::is_integral_v<T_>;

	namespace detail_ {
		template <typename T_, typename... Ts_> struct IsOneOf_ {
			static constexpr ::dcool::core::Boolean result_ = false;
		};

		template <typename T_, typename FirstT_, typename... Ts_> struct IsOneOf_<T_, FirstT_, Ts_...> {
			static constexpr ::dcool::core::Boolean result_ =
				::dcool::core::isSame<T_, FirstT_> || ::dcool::core::detail_::IsOneOf_<T_, Ts_...>::result_
			;
		};
	}

	template <typename T_, typename... Ts_> concept FormOfSame = ::dcool::core::SameAs<
		::dcool::core::QualifiedReferenceRemovedType<T_>, ::dcool::core::QualifiedReferenceRemovedType<Ts_>...
	>;

	template <
		typename T_, typename... Ts_
	> constexpr ::dcool::core::Boolean isOneOf = ::dcool::core::detail_::IsOneOf_<T_, Ts_...>::result_;

	template <typename T_, typename... Ts_> concept OneOf = ::dcool::core::isOneOf<T_, Ts_...>;

	template <typename T_> concept Dereferenceable = requires (T_ toDereference_) {
		{ *toDereference_ } -> ::dcool::core::Reference;
	};

	template <
		typename T_,
		typename ToDereferenceT_,
		typename ValueT_ = ::dcool::core::ReferenceRemovedType<
			decltype(::dcool::core::declval<T_>()(::dcool::core::declval<ToDereferenceT_>()))
		>
	> concept DereferencerOf = requires (T_ dereferencer_, ToDereferenceT_ toDereference_) {
		{ dereferencer_(toDereference_) } -> ::dcool::core::ConvertibleTo<ValueT_&>;
	};

	template <
		typename T_, typename IndexT_ = ::dcool::core::Difference
	> concept IndexableBy = requires (T_ range_, IndexT_ index_) {
		{ range_[index_] } -> ::dcool::core::Reference;
	};

	template <typename T_> concept StandardSwappable = ::std::is_swappable_v<T_>;
}

#endif
