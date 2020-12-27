#ifndef DCOOL_CORE_CONVERTER_HPP_INCLUDED_
#	define DCOOL_CORE_CONVERTER_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>

namespace dcool::core {
	template <
		typename T_,
		typename FirstT_,
		typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
	> concept BidirectionalConverter = requires (T_ converter_, FirstT_ first_, SecondT_ second_) {
		{ converter_(first_) } -> ::dcool::core::ConvertibleTo<SecondT_>;
		{ converter_(second_) } -> ::dcool::core::ConvertibleTo<FirstT_>;
	};

	template <
		typename T_,
		typename FirstT_,
		typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
	> concept BidirectionalConsistentConverter = BidirectionalConverter<T_ const, FirstT_, SecondT_>;

	template <typename FromT_, typename ToT_> struct DefaultImplicitConverter {
		template <::dcool::core::FormOfSame<FromT_> FromT__> constexpr auto operator ()(FromT__&& from_) const -> ToT_ {
			return ::dcool::core::forward<FromT_>(from_);
		}
	};

	template <::dcool::core::Reference FromT_, typename ToT_> struct DefaultImplicitConverter<FromT_, ToT_> {
		constexpr auto operator ()(FromT_ from_) const -> ToT_ {
			return static_cast<FromT_>(from_);
		}
	};

	template <typename FirstT_, typename SecondT_> struct DefaultBidirectionalImplicitConverter:
		DefaultImplicitConverter<FirstT_, SecondT_>, DefaultImplicitConverter<SecondT_, FirstT_>
	{
	};

	template <typename T_> struct DefaultBidirectionalImplicitConverter<T_, T_>: DefaultImplicitConverter<T_, T_> {
	};

	namespace customizable {
		template <
			typename T_, typename FirstT_, typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
		> requires ::dcool::core::BidirectionalConverter<T_, FirstT_, SecondT_> struct IsBijectiveConverter {
			static constexpr ::dcool::core::Boolean result = false;
		};
	}

	namespace detail_ {
		template <
			typename T_, typename FirstT_, typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
		> requires ::dcool::core::BidirectionalConverter<T_, FirstT_, SecondT_> struct IsBijectiveConverterHelper_ {
			static constexpr ::dcool::core::Boolean result =
				::dcool::core::customizable::IsBijectiveConverter<T_, FirstT_, SecondT_>::result
			;
		};

		template <typename FirstT_, typename SecondT_> struct IsBijectiveConverterHelper_<
			::dcool::core::DefaultBidirectionalImplicitConverter<FirstT_, SecondT_>, FirstT_, SecondT_
		> {
			static constexpr ::dcool::core::Boolean result = true;
		};
	}

	template <
		typename T_, typename FirstT_, typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
	> constexpr ::dcool::core::Boolean isBijectiveConverter =
		::dcool::core::detail_::IsBijectiveConverterHelper_<T_, FirstT_, SecondT_>::result
	;

	template <
		typename T_, typename FirstT_, typename SecondT_ = decltype(::dcool::core::declval<T_&>()(::dcool::core::declval<FirstT_>()))
	> concept BijectiveConverter = ::dcool::core::isBijectiveConverter<T_, FirstT_, SecondT_>;
}

#endif
