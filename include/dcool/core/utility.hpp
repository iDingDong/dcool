#ifndef DCOOL_CORE_UTILITY_HPP_INCLUDED_
#	define DCOOL_CORE_UTILITY_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	include <utility>

namespace dcool::core {
	using ::std::forward;
	using ::std::move;

	template <typename T_, typename... ArgumentTs_> constexpr auto create(ArgumentTs_&&... parameters_) noexcept(
		noexcept(T_(::dcool::core::forward<ArgumentTs_>(parameters_)...))
	) -> T_ {
		return T_(::dcool::core::forward<ArgumentTs_>(parameters_)...);
	}

	template <typename T_> concept SwappableByMember = requires (T_ first_, T_ second_) {
		first_.swapWith(second_);
	};

	template <typename T_> concept IntelliSwappable =
		::dcool::core::SwappableByMember<T_> || ::dcool::core::StandardSwappable<T_>
	;

	namespace detail_ {
		template <typename T_> void intelliSwapHelper_(T_& first_, T_& second_) noexcept(::dcool::core::isNoThrowSwappable<T_>) {
			using ::std::swap;
			swap(first_, second_);
		}

		template <::dcool::core::SwappableByMember T_> void intelliSwapHelper_(
			T_& first_, T_& second_
		) noexcept(noexcept(first_.swapWith(second_))) {
			first_.swapWith(second_);
		}
	}

	template <::dcool::core::IntelliSwappable T_> void intelliSwap(T_& first_, T_& second_) noexcept(
		noexcept(::dcool::core::detail_::intelliSwapHelper_(first_, second_))
	) {
		::dcool::core::detail_::intelliSwapHelper_(first_, second_);
	}

	template <typename T_> using TypedInPlaceTag = ::std::in_place_type_t<T_>;

	template <typename T_> constexpr ::dcool::core::TypedInPlaceTag<T_> typedInPlace = ::std::in_place_type<T_>;

	using InPlaceTag = ::std::in_place_t;
	constexpr ::dcool::core::InPlaceTag inPlace = ::std::in_place;

	template <typename T_> struct TypedTag {
		using Type = T_;

		constexpr explicit TypedTag() noexcept = default;
	};

	template <typename T_> constexpr ::dcool::core::TypedTag<T_> typed = ::dcool::core::TypedTag<T_>();
}

#endif
