#ifndef DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/type_transformer.hpp>

namespace dcool::core {
	template <typename T_> constexpr auto stepByByte(
		T_* pointer_, ::dcool::core::Difference step_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, T_>* {
		auto generalPointer_ = static_cast<::dcool::core::IdenticallyQualifiedType<void, T_>*>(pointer_);
		return static_cast<::dcool::core::IdenticallyQualifiedType<::dcool::core::Byte, T_>*>(generalPointer_) + step_;
	}

	inline constexpr auto differenceByByte(
		void const volatile* left_, void const volatile* right_
	) noexcept -> ::dcool::core::Difference {
		return
			static_cast<::dcool::core::Byte const volatile*>(left_) -
			static_cast<::dcool::core::Byte const volatile*>(right_)
		;
	}
}

#endif
