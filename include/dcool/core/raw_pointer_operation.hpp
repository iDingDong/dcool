#ifndef DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/type_transformer.hpp>

#	include <cstring>

namespace dcool::core {
	template <typename T_> constexpr auto stepByByte(
		T_* pointer_, ::dcool::core::Difference step_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, T_>* {
		auto generalPointer_ = static_cast<::dcool::core::IdenticallyQualifiedType<void, T_>*>(pointer_);
		return static_cast<::dcool::core::IdenticallyQualifiedType<::dcool::core::Byte, T_>*>(generalPointer_) + step_;
	}

	template <typename T_> constexpr auto stepOver(T_* pointer_) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, T_>* {
		return ::dcool::core::stepByByte(pointer_, sizeof(*pointer_));
	}

	template <typename T_> constexpr auto stepOver(
		::dcool::core::IdenticallyQualifiedType<void, T_>* pointer_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, T_>* {
		return ::dcool::core::stepByByte(pointer_, sizeof(T_));
	}

	inline constexpr auto differenceByByte(
		void const volatile* left_, void const volatile* right_
	) noexcept -> ::dcool::core::Difference {
		return
			static_cast<::dcool::core::Byte const volatile*>(left_) -
			static_cast<::dcool::core::Byte const volatile*>(right_)
		;
	}

	template <::dcool::core::TriviallyCopyable T_> constexpr void writeToBuffer(T_ const& value_, void* buffer_) noexcept {
		::std::memcpy(buffer_, ::dcool::core::addressOf(value_), sizeof(value_));
	}

	template <::dcool::core::TriviallyCopyable T_> constexpr auto readFromBuffer(void const* buffer_) noexcept -> T_ {
		T_ result_;
		::std::memcpy(::dcool::core::addressOf(result_), buffer_, sizeof(result_));
		return result_;
	}
}

#endif
