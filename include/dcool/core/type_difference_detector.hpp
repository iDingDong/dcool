#ifndef DCOOL_CORE_TYPE_DIFFERENCE_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_DIFFERENCE_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/member_detector.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeDifference, ExtractedDifferenceType, Difference)

namespace dcool::core {
	template <typename T_> using DifferenceType = ::dcool::core::ExtractedDifferenceType<T_, ::dcool::core::Difference>;
}

#endif
