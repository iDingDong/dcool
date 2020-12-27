#ifndef DCOOL_CORE_DEREF_HPP_INCLUDED_
#	define DCOOL_CORE_DEREF_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

namespace dcool::core {
	template <::dcool::core::Dereferenceable PointerT_> struct Dereferencer {
		constexpr decltype(auto) operator()(const PointerT_& pointer_) const noexcept(noexcept(*pointer_)) {
			return *pointer_;
		}
	};

	template <typename PointerT_> constexpr ::dcool::core::Dereferencer<PointerT_> dereferencer;

	struct CommonDereferencer {
		template <typename PointerT_> constexpr decltype(auto) operator()(const PointerT_& pointer_) const noexcept(
			noexcept(::dcool::core::dereferencer<PointerT_>(pointer_))
		) {
			return ::dcool::core::dereferencer<PointerT_>(pointer_);
		}
	};

	constexpr ::dcool::core::CommonDereferencer commonDereferencer;

	template <typename PointerT_> constexpr decltype(auto) dereference(
		const PointerT_& pointer_
	) noexcept(noexcept(::dcool::core::commonDereferencer(pointer_))) {
		return ::dcool::core::commonDereferencer(pointer_);
	}
}

#endif
