#ifndef DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/member_detector.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeValue, ExtractedValueType, Value)

namespace dcool::core {
	template <typename ValueT_> struct ValueNested {
		using Value = ValueT_;
	};
}

#endif
