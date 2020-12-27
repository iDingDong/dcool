#ifndef DCOOL_CORE_UTILITY_HPP_INCLUDED_
#	define DCOOL_CORE_UTILITY_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	include <utility>

namespace dcool::core {
	using ::std::forward;
	using ::std::move;

	template <typename T_> concept SwappableByMember = requires (T_ first_, T_ second_) {
		first_.swapWith(second_);
	};

	template <typename T_> concept Intelliswappable = ::dcool::core::SwappableByMember<T_> || ::dcool::core::StandardSwappable<T_>;

	namespace detail_ {
		template <typename T_> void intelliswapHelper_(T_& first_, T_& second_) {
			using ::std::swap;
			swap(first_, second_);
		}

		template <::dcool::core::SwappableByMember T_> void intelliswapHelper_(
			T_& first_, T_& second_
		) noexcept(noexcept(first_.swapWith(second_))) {
			first_.swapWith(second_);
		}
	}

	template <::dcool::core::Intelliswappable T_> void intelliswap(T_& first_, T_& second_) noexcept(
		noexcept(::dcool::core::detail_::intelliswapHelper_(first_, second_))
	) {
		::dcool::core::detail_::intelliswapHelper_(first_, second_);
	}
}

#endif
