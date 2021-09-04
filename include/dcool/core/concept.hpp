#ifndef DCOOL_CORE_CONCEPT_HPP_INCLUDED_
#	define DCOOL_CORE_CONCEPT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/type_transformer.hpp>

#	include <concepts>
#	include <type_traits>

namespace dcool::core {
	template <auto valueC_> using RequiresConsteval = void;

	template <typename T_> concept Complete = requires {
		sizeof(T_);
	};

	template <typename... Ts_> constexpr ::dcool::core::Boolean isSame = true;

	template <
		typename FirstT_, typename SecondT_, typename... Ts_
	> constexpr ::dcool::core::Boolean isSame<FirstT_, SecondT_, Ts_...> =
		::std::is_same_v<FirstT_, SecondT_> && ::dcool::core::isSame<FirstT_, Ts_...>
	;

	template <typename T_, typename... Ts_> concept SameAs = ::dcool::core::isSame<T_, Ts_...>;

	template <typename T_, typename TargetT_> concept ConvertibleTo = ::std::convertible_to<T_, TargetT_>;

	template <typename T_, typename OtherT_> concept ConvertibleBetween =
		::dcool::core::ConvertibleTo<T_, OtherT_> && ::dcool::core::ConvertibleTo<OtherT_, T_>
	;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowCopyConstructible =
		::std::is_nothrow_copy_constructible_v<T_>
	;

	template <typename T_> concept NoThrowCopyConstructible = ::dcool::core::isNoThrowCopyConstructible<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowCopyAssignable = ::std::is_nothrow_copy_assignable_v<T_>;

	template <typename T_> concept NoThrowCopyAssignable = ::dcool::core::isNoThrowCopyAssignable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowCopyable =
		::dcool::core::isNoThrowCopyConstructible<T_> && ::dcool::core::isNoThrowCopyAssignable<T_>
	;

	template <typename T_> concept NoThrowCopyable = ::dcool::core::isNoThrowCopyable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isTriviallyCopyable = ::std::is_trivially_copyable_v<T_>;

	template <typename T_> concept TriviallyCopyable = ::dcool::core::isTriviallyCopyable<T_>;

	template <
		typename T_, typename... ArgumentTs_
	> constexpr ::dcool::core::Boolean isTriviallyConstructible = ::std::is_trivially_constructible_v<T_, ArgumentTs_...>;

	template <
		typename T_, typename... ArgumentTs_
	> concept TriviallyConstructible = ::dcool::core::isTriviallyConstructible<T_, ArgumentTs_...>;

	template <
		typename T_
	> constexpr ::dcool::core::Boolean isTriviallyCopyConstructible = ::std::is_trivially_copy_constructible_v<T_>;

	template <typename T_> concept TriviallyCopyConstructible = ::dcool::core::isTriviallyCopyConstructible<T_>;

	template <
		typename T_
	> constexpr ::dcool::core::Boolean isTriviallyMoveConstructible = ::std::is_trivially_move_constructible_v<T_>;

	template <typename T_> concept TriviallyMoveConstructible = ::dcool::core::isTriviallyMoveConstructible<T_>;

	template <
		typename T_
	> constexpr ::dcool::core::Boolean isTriviallyRelocatable = ::dcool::core::isTriviallyCopyable<T_>;

	template <typename T_> concept TriviallyRelocatable = ::dcool::core::isTriviallyRelocatable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowMoveConstructible =
		::std::is_nothrow_move_constructible_v<T_>
	;

	template <typename T_> constexpr ::dcool::core::Boolean isTriviallyDestructible = ::std::is_trivially_destructible_v<T_>;

	template <typename T_> concept TriviallyDestructible = ::dcool::core::isTriviallyDestructible<T_>;

	template <typename T_> concept NoThrowMoveConstructible = ::dcool::core::isNoThrowMoveConstructible<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isTrivial = ::std::is_trivial_v<T_>;

	template <typename T_> concept Trivial = ::dcool::core::isTrivial<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowMoveAssignable = ::std::is_nothrow_move_assignable_v<T_>;

	template <typename T_> concept NoThrowMoveAssignable = ::dcool::core::isNoThrowMoveAssignable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowMovable =
		::dcool::core::isNoThrowMoveConstructible<T_> && ::dcool::core::isNoThrowMoveAssignable<T_>
	;

	template <typename T_> concept NoThrowMovable = ::dcool::core::isNoThrowMovable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowRelocatable = ::dcool::core::isNoThrowMoveConstructible<T_>;

	template <typename T_> concept NoThrowRelocatable = ::dcool::core::isNoThrowRelocatable<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowSwappable = ::std::is_nothrow_swappable_v<T_>;

	template <typename T_> concept NoThrowSwappable = ::dcool::core::isNoThrowSwappable<T_>;

	template <typename T_> concept NonVoid = !::dcool::core::SameAs<T_, void>;

	template <typename T_> concept Const = ::std::is_const_v<T_>;

	template <typename T_> concept Object = ::std::is_object_v<T_>;

	template <typename T_> concept Reference = ::std::is_reference_v<T_>;

	template <typename T_> concept LvalueReference = ::std::is_lvalue_reference_v<T_>;

	template <typename T_> concept RvalueReference = ::std::is_rvalue_reference_v<T_>;

	template <typename T_, typename ReferedT_> concept ReferenceOf =
		::dcool::core::Reference<T_> && ::dcool::core::SameAs<::dcool::core::ReferenceRemovedType<T_>, ReferedT_>
	;

	template <typename T_> concept ConstReference = ::dcool::core::Const<::dcool::core::ReferenceRemovedType<T_>> && Reference<T_>;

	template <typename T_> concept Pointer = ::std::is_pointer_v<T_>;

	template <typename T_> concept Class = ::std::is_class_v<T_>;

	template <typename T_> concept Integral = ::std::is_integral_v<T_>;

	template <typename T_> concept SignedIntegral = ::std::is_signed_v<T_>;

	template <typename T_> concept UnsignedIntegral = ::std::is_unsigned_v<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isFinal = ::std::is_final_v<T_>;

	template <typename T_> concept Final = ::dcool::core::isFinal<T_>;

	template <typename T_> concept StandardLayout = ::std::is_standard_layout_v<T_>;

	template <typename T_> constexpr ::dcool::core::Boolean isPolymorphic = ::std::is_polymorphic_v<T_>;

	template <typename T_> concept Polymorphic = ::dcool::core::isPolymorphic<T_>;

	template <typename T_, typename... Ts_> constexpr ::dcool::core::Boolean isOneOf = false;

	template <
		typename T_, typename FirstT_, typename... Ts_
	> constexpr ::dcool::core::Boolean isOneOf<T_, FirstT_, Ts_...> =
		::dcool::core::isSame<T_, FirstT_> || ::dcool::core::isOneOf<T_, Ts_...>
	;

	template <typename T_, typename... Ts_> concept OneOf = ::dcool::core::isOneOf<T_, Ts_...>;

	template <typename T_, typename... Ts_> concept QualifiedOfSame = ::dcool::core::SameAs<
		::dcool::core::QualifierRemovedType<T_>, ::dcool::core::QualifierRemovedType<Ts_>...
	>;

	template <typename T_, typename... Ts_> concept FormOfSame = ::dcool::core::QualifiedOfSame<
		::dcool::core::ReferenceRemovedType<T_>, ::dcool::core::ReferenceRemovedType<Ts_>...
	>;

	template <typename T_, typename... Ts_> concept FormOfOneOf = ::dcool::core::OneOf<
		::dcool::core::QualifiedReferenceRemovedType<T_>, ::dcool::core::QualifiedReferenceRemovedType<Ts_>...
	>;

	template <typename T_> concept Dereferenceable = requires (T_ toDereference_) {
		{ *toDereference_ };
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
