#ifndef DCOOL_CORE_DESTRUCT_HPP_INCLUDED_
#	define DCOOL_CORE_DESTRUCT_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

namespace dcool::core {
	template <typename T_> constexpr void destruct(T_& toDestruct_) noexcept {
		toDestruct_.~T_();
	}

	template <typename T_, ::dcool::core::Length lengthC_> constexpr void destruct(T_ (&toDestruct_)[lengthC_]) noexcept {
		if constexpr (!::dcool::core::isTriviallyDestructible<T_>) {
			for (auto& item_: toDestruct_) {
				::dcool::core::destruct(item_);
			}
		}
	}
}

#endif
