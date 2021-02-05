#ifndef DCOOL_CORE_DEREFERENCE_HPP_INCLUDED_
#	define DCOOL_CORE_DEREFERENCE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <memory>

namespace dcool::core {
	template <::dcool::core::Dereferenceable PointerT_> struct DefaultDereferencerFor {
		constexpr decltype(auto) operator()(const PointerT_& pointer_) const noexcept(noexcept(*pointer_)) {
			return *pointer_;
		}
	};

	template <typename PointerT_> constexpr ::dcool::core::DefaultDereferencerFor<PointerT_> defaultDereferencerFor;

	struct DefaultDereferencer {
		template <typename PointerT_> constexpr decltype(auto) operator()(const PointerT_& pointer_) const noexcept(
			noexcept(::dcool::core::defaultDereferencerFor<PointerT_>(pointer_))
		) {
			return ::dcool::core::defaultDereferencerFor<PointerT_>(pointer_);
		}
	};

	constexpr ::dcool::core::DefaultDereferencer defaultDereferencer;

	template <typename PointerT_> constexpr decltype(auto) dereference(
		const PointerT_& pointer_
	) noexcept(noexcept(::dcool::core::defaultDereferencer(pointer_))) {
		return ::dcool::core::defaultDereferencer(pointer_);
	}

	template <typename PointerT_> constexpr auto rawPointerToLivingOf(const PointerT_& pointer_) noexcept(
		noexcept(::dcool::core::addressOf(::dcool::core::dereference(pointer_)))
	) {
		return ::dcool::core::addressOf(::dcool::core::dereference(pointer_));
	}

	namespace detail_ {
		template <typename T_> concept RawPointerGettableByMember = requires (T_ const& toGetFrom_) {
			{ toGetFrom_.rawPointer() } -> ::dcool::core::Pointer;
		};
	}

	template <
		::dcool::core::detail_::RawPointerGettableByMember PointerT_
	> constexpr auto rawPointerOf(const PointerT_& pointer_) noexcept(noexcept(pointer_.rawPointer())) {
		return pointer_.rawPointer();
	}

	template <typename PointerT_> constexpr auto rawPointerOf(const PointerT_& pointer_) noexcept(
		noexcept(::dcool::core::rawPointerToLivingOf(pointer_))
	) {
		return ::std::to_address(pointer_);
	}
}

#endif
