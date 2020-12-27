#ifndef DCOOL_CORE_TYPE_SIZE_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_SIZE_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/member_detector.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeSize, ExtractedSizeType, Size)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeLength, ExtractedLengthType, Size)

namespace dcool::core {
	template <typename T_> using SizeType = ::dcool::core::ExtractedSizeType<T_, ::dcool::core::Size>;
	template <typename T_> using LengthType = ::dcool::core::ExtractedLengthType<
		T_, ::dcool::core::ExtractedSizeType<T_, ::dcool::core::Length>
	>;
}

#endif
