#ifndef DCOOL_BINARY_ENDIAN_HPP_INCLUDED_
#	define DCOOL_BINARY_ENDIAN_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <algorithm>
#	include <bit>
#	include <type_traits>

namespace dcool::binary {
	using StandardEndianUnderlying = ::std::underlying_type_t<::std::endian>;

	enum class Endian: StandardEndianUnderlying {
		little = static_cast<::dcool::binary::StandardEndianUnderlying>(::std::endian::little),
		big = static_cast<::dcool::binary::StandardEndianUnderlying>(::std::endian::big),
		native = static_cast<::dcool::binary::StandardEndianUnderlying>(::std::endian::native),
		network = ::dcool::binary::Endian::big
	};

	inline constexpr ::dcool::core::Boolean littleEndian = (::dcool::binary::Endian::native == ::dcool::binary::Endian::little);
	inline constexpr ::dcool::core::Boolean bigEndian = (::dcool::binary::Endian::native == ::dcool::binary::Endian::big);
	inline constexpr ::dcool::core::Boolean mixedEndian = !(::dcool::binary::littleEndian || ::dcool::binary::bigEndian);

	template <
		::dcool::binary::Endian targetEndianC_, ::dcool::core::UnsignedIntegral T_
	> requires (targetEndianC_ == ::dcool::binary::Endian::native || !mixedEndian) constexpr auto writeAs(
		T_ from_, void* to_
	) noexcept {
		::dcool::core::Byte const* startOfFrom_ = reinterpret_cast<::dcool::core::Byte const*>(::dcool::core::addressOf(from_));
		::dcool::core::Byte const* endOfFrom_ = startOfFrom_ + sizeof(T_);
		::dcool::core::Byte* startOfTo_ = reinterpret_cast<::dcool::core::Byte*>(to_);
		if constexpr (targetEndianC_ == ::dcool::binary::Endian::native) {
			::dcool::core::batchCopyConstruct(startOfFrom_, endOfFrom_, startOfTo_);
		} else {
			::dcool::core::batchCopyConstruct(
				::dcool::core::makeReverseIterator(endOfFrom_), ::dcool::core::makeReverseIterator(startOfFrom_), startOfTo_
			);
		}
	}

	template <
		::dcool::binary::Endian sourceEndianC_, ::dcool::core::UnsignedIntegral T_
	> requires (sourceEndianC_ == ::dcool::binary::Endian::native || !mixedEndian) constexpr auto readAs(
		void const* from_
	) noexcept -> T_ {
		T_ result_;
		::dcool::core::Byte const* startOfFrom_ = reinterpret_cast<::dcool::core::Byte const*>(from_);
		::dcool::core::Byte const* endOfFrom_ = startOfFrom_ + sizeof(T_);
		::dcool::core::Byte* startOfTo_ = reinterpret_cast<::dcool::core::Byte*>(::dcool::core::addressOf(result_));
		if constexpr (sourceEndianC_ == ::dcool::binary::Endian::native) {
			::dcool::core::batchCopyConstruct(startOfFrom_, endOfFrom_, startOfTo_);
		} else {
			::dcool::core::batchCopyConstruct(
				::dcool::core::makeReverseIterator(endOfFrom_), ::dcool::core::makeReverseIterator(startOfFrom_), startOfTo_
			);
		}
		return result_;
	}
}

#endif
