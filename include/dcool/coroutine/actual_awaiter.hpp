#ifndef DCOOL_COROUTINE_ACTUAL_AWAITER_HPP_INCLUDED_
#	define DCOOL_COROUTINE_ACTUAL_AWAITER_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

namespace dcool::coroutine {
	namespace detail_ {
		template <typename T_> concept CoAwaitOverloaded_ = requires (T_&& awaited_) {
			{ ::dcool::core::forward<T_>(awaited_).operator co_await() };
		};
	}

	template <typename AwaiterT_> constexpr decltype(auto) actualAwaiter(AwaiterT_&& awaiter_) {
		return ::dcool::core::forward<AwaiterT_>(awaiter_);
	}

	template <
		::dcool::coroutine::detail_::CoAwaitOverloaded_ AwaiterT_
	> constexpr decltype(auto) actualAwaiter(AwaiterT_&& awaiter_) {
		return ::dcool::core::forward<AwaiterT_>(awaiter_).operator co_await();
	}

	template <typename AwaiterT_> using ActualAwaiterType = decltype(
		::dcool::coroutine::actualAwaiter(::dcool::core::declval<AwaiterT_>())
	);
}

#endif
