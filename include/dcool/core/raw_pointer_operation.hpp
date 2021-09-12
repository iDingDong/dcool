#ifndef DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RAW_POINTER_OPERATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/type_transformer.hpp>

#	include <cstdint>
#	include <cstring>

namespace dcool::core {
	template <typename ValueT_> constexpr auto toPointerInteger(ValueT_* pointer_) noexcept -> ::dcool::core::PointerInteger {
		::dcool::core::IdenticallyQualifiedType<void, ValueT_>* universalPointer_ = pointer_;
		return reinterpret_cast<::dcool::core::PointerInteger>(universalPointer_);
	}


	template <typename ValueT_> constexpr auto fromPointerInteger(
		::dcool::core::PointerInteger integer_
	) noexcept -> ValueT_* {
		auto universalPointer_ = reinterpret_cast<::dcool::core::IdenticallyQualifiedType<void, ValueT_>*>(integer_);
		return reinterpret_cast<ValueT_*>(universalPointer_);
	}

	template <typename ValueT_> constexpr auto stepByByte(
		ValueT_* pointer_, ::dcool::core::Difference step_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, ValueT_>* {
		auto generalPointer_ = static_cast<::dcool::core::IdenticallyQualifiedType<void, ValueT_>*>(pointer_);
		return static_cast<::dcool::core::IdenticallyQualifiedType<::dcool::core::Byte, ValueT_>*>(generalPointer_) + step_;
	}

	template <typename ValueT_> constexpr auto stepOver(
		ValueT_* pointer_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, ValueT_>* {
		return ::dcool::core::stepByByte(pointer_, sizeof(*pointer_));
	}

	template <typename ValueT_> constexpr auto stepOver(
		::dcool::core::IdenticallyQualifiedType<void, ValueT_>* pointer_
	) noexcept -> ::dcool::core::IdenticallyQualifiedType<void, ValueT_>* {
		return ::dcool::core::stepByByte(pointer_, sizeof(ValueT_));
	}

	inline constexpr auto differenceByByte(
		void const volatile* left_, void const volatile* right_
	) noexcept -> ::dcool::core::Difference {
		return
			static_cast<::dcool::core::Byte const volatile*>(left_) -
			static_cast<::dcool::core::Byte const volatile*>(right_)
		;
	}

	template <::dcool::core::TriviallyCopyable ValueT_> constexpr void writeToBuffer(
		ValueT_ const& value_, void* buffer_
	) noexcept {
		::std::memcpy(buffer_, ::dcool::core::addressOf(value_), sizeof(value_));
	}

	template <::dcool::core::TriviallyCopyable ValueT_> constexpr auto readFromBuffer(void const* buffer_) noexcept -> ValueT_ {
		ValueT_ result_;
		::std::memcpy(::dcool::core::addressOf(result_), buffer_, sizeof(result_));
		return result_;
	}
}

#endif
