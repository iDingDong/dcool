#ifndef DCOOL_CONTAINER_BASIC_HPP_INCLUDED_
#	define DCOOL_CONTAINER_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

namespace dcool::container {
	namespace detail_ {
		template <typename T_, typename ValueT_> concept DirectlyBackPushable_ = requires (T_ container_, ValueT_&& value_) {
			container_.pushBack(container_, ::dcool::core::forward<ValueT_>(value_));
		};

		template <typename ContainerT_, typename ValueT_> constexpr void generalPushBackTo_(
			ContainerT_& container_, ValueT_&& value_
		) noexcept(noexcept(container_.push_back(::dcool::core::forward<ValueT_>(value_)))) {
			container_.push_back(::dcool::core::forward<ValueT_>(value_));
		}

		template <
			typename ValueT_, ::dcool::container::detail_::DirectlyBackPushable_<ValueT_&&> ContainerT_
		> constexpr void generalPushBackTo_(ContainerT_& container_, ValueT_&& value_) noexcept(
			noexcept(container_.pushBack(::dcool::core::forward<ValueT_>(value_)))
		) {
			container_.pushBack(::dcool::core::forward<ValueT_>(value_));
		}
	}

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

	namespace detail_ {
		template <typename T_> concept DirectlyFrontPoppable_ = requires (T_ container_) {
			container_.popFront(container_);
		};

		template <typename ContainerT_> constexpr void generalPopFrontOf_(ContainerT_& container_) noexcept(
			noexcept(container_.pop_front())
		) {
			container_.pop_front();
		}

		template <
			::dcool::container::detail_::DirectlyFrontPoppable_ ContainerT_
		> constexpr void generalPopFrontOf_(ContainerT_& container_) noexcept(noexcept(container_.popFront())) {
			container_.popFront();
		}
	}

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
