#ifndef DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_TYPE_VALUE_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/type_transformer.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeValue, ExtractedValueType, Value)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasStandardTypeValue, ExtractedStandardValueType, value_type)

namespace dcool::core {
	namespace detail_ {
		template <typename T_, typename OtherwiseT_> struct ValueHelper_ {
			using Result_ = ::dcool::core::ExtractedValueType<T_, OtherwiseT_>;
		};

		template <typename T_, typename OtherwiseT_> struct ValueHelper_<T_*, OtherwiseT_> {
			using Result_ = T_;
		};

		template <typename T_, typename OtherwiseT_> struct ValueHelper_<T_[], OtherwiseT_> {
			using Result_ = T_;
		};

		template <
			typename T_, ::dcool::core::Length lengthC_, typename OtherwiseT_
		> struct ValueHelper_<T_[lengthC_], OtherwiseT_> {
			using Result_ = T_;
		};
	}

	template <typename T_> using ValueType = ::dcool::core::detail_::ValueHelper_<
		::dcool::core::QualifiedReferenceRemovedType<T_>, void
	>::Result_;

	template <typename T_> using CompatibleValueType = ::dcool::core::detail_::ValueHelper_<
		::dcool::core::QualifiedReferenceRemovedType<T_>,
		::dcool::core::ExtractedStandardValueType<::dcool::core::QualifiedReferenceRemovedType<T_>, void>
	>::Result_;
}

#endif
