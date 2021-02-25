#ifndef DCOOL_CORE_TYPE_LIST_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	template <typename... Ts_> struct Types {
		template <typename T_> using PushFront = ::dcool::core::Types<T_, Ts_...>;
		template <typename T_> using PushBack = ::dcool::core::Types<Ts_..., T_>;
	};

	template <typename T_> constexpr ::dcool::core::Boolean isTypeList = false;

	template <typename... Ts_> constexpr ::dcool::core::Boolean isTypeList<::dcool::core::Types<Ts_...>> = true;

	template <typename T_> concept TypeList = ::dcool::core::isTypeList<T_>;
}

#endif
