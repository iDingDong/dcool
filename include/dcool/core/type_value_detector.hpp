#ifndef DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/type_transformer.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeValue, ExtractedValueType, Value)

namespace dcool::core {
	namespace detail_ {
		template <typename T_> struct ValueHelper_ {
			using Result_ = ::dcool::core::ExtractedValueType<T_, void>;
		};

		template <typename T_> struct ValueHelper_<T_*> {
			using Result_ = T_;
		};

		template <typename T_> struct ValueHelper_<T_[]> {
			using Result_ = T_;
		};

		template <typename T_, ::dcool::core::Length lengthC_> struct ValueHelper_<T_[lengthC_]> {
			using Result_ = T_;
		};
	}

	template <typename T_> using ValueType = ::dcool::core::detail_::ValueHelper_<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>::Result_;
}

#endif
