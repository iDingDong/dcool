#ifndef DCOOL_CONTAINER_BASIC_HPP_INCLUDED_
#	define DCOOL_CONTAINER_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

DCOOL_CORE_DEFINE_MEMBER_CALLER(
	dcool::container::detail_,
	DirectlyBackPushable_,
	generalPushBackTo_,
	pushBack,
	[](auto& container_, auto&& value_) {
		container_.push_back(::dcool::core::forward<decltype(value_)>(value_));
	}
)

namespace dcool::container {
	template <typename T_, typename ValueT_> concept CompatibleBackPushable = requires (T_ container_, ValueT_&& value_) {
		::dcool::container::detail_::generalPushBackTo_(container_, ::dcool::core::forward<ValueT_>(value_));
	};

	template <
		typename ValueT_, ::dcool::container::CompatibleBackPushable<ValueT_&&> ContainerT_
	> constexpr void generalPushBackTo(ContainerT_& container_, ValueT_&& value_) noexcept(
		noexcept(::dcool::container::detail_::generalPushBackTo_(container_, ::dcool::core::forward<ValueT_>(value_)))
	) {
		::dcool::container::detail_::generalPushBackTo_(container_, ::dcool::core::forward<ValueT_>(value_));
	}
}

DCOOL_CORE_DEFINE_MEMBER_CALLER(
	dcool::container::detail_,
	DirectlyFrontPoppable_,
	generalPopFrontOf_,
	popFront,
	[](auto& container_) {
		container_.pop_front();
	}
)

namespace dcool::container {
	template <typename T_> concept CompatibleFrontPoppable = requires (T_ container_) {
		::dcool::container::detail_::generalPopFrontOf_(container_);
	};

	template <
		::dcool::container::CompatibleFrontPoppable ContainerT_
	> constexpr void generalPopFrontOf(ContainerT_& container_) noexcept(
		noexcept(::dcool::container::detail_::generalPopFrontOf_(container_))
	) {
		::dcool::container::detail_::generalPopFrontOf_(container_);
	}
}

#endif
