#ifndef DCOOL_CORE_TYPE_TRANSFORMER_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_TRANSFORMER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

#	include <type_traits>

namespace dcool::core {
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
		::dcool::core::detail_::ConstantizePointed_<::dcool::core::QualifierRemovedType<T_>>, T_
	>;

	template <typename FirstT_, typename SecondT_> using SmallerType = ::std::conditional_t<
		sizeof(FirstT_) <= sizeof(SecondT_), FirstT_, SecondT_
	>::type;
}

#endif
