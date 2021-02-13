#ifndef DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/type_transformer.hpp>

namespace dcool::core {
	template <typename T_> constexpr auto stepByByte(
		T_* pointer_, ::dcool::core::Difference step_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, T_>* {
		return reinterpret_cast<::dcool::core::IdenticallyQualifiedType<::dcool::core::Byte, T_>*>(pointer_) + step_;
	}
}

#endif
