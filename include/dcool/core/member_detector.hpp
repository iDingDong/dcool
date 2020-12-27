#ifndef DCOOL_CORE_MEMBER_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_MEMBER_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	define DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(namespaceName, ConceptName_, ExtractorName_, MemberName_) \
	namespace namespaceName { \
		template <typename T_> concept ConceptName_ = requires { \
			typename T_::MemberName_; \
		}; \
		\
		template <typename T_, typename DefaultT_> struct ExtractorName_##DcmCoreHelper_ { \
			using Result_ = DefaultT_; \
		}; \
		\
		template <::namespaceName::ConceptName_ T_, typename DefaultT_> struct ExtractorName_##DcmCoreHelper_<T_, DefaultT_> { \
			using Result_ = T_::MemberName_; \
		}; \
		\
		template < \
			typename T_, typename DefaultT_ \
		> using ExtractorName_ = ::namespaceName::ExtractorName_##DcmCoreHelper_<T_, DefaultT_>::Result_; \
	}

#	define DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(namespaceName, ConceptName_, extractorName_, memberName_) \
	namespace namespaceName { \
		template <typename T_> concept ConceptName_ = requires { \
			typename ::dcool::core::RequiresConsteval<T_::memberName_>; \
		}; \
		\
		template <typename T_, typename ValueT_> consteval auto extractorName_(ValueT_ defaultValue_) { \
			return defaultValue_; \
		}; \
		\
		template <::namespaceName::ConceptName_ T_, typename ValueT_> consteval auto extractorName_(ValueT_ defaultValue_) { \
			return T_::memberName_; \
		}; \
	}

#	define DCOOL_CORE_DEFINE_STATIC_MEMBER_DETECTOR(namespaceName, ConceptName_, extractorName_, memberName_) \
	namespace namespaceName { \
		template <typename T_> concept ConceptName_ = requires { \
			T_::memberName_; \
		}; \
		\
		template <typename T_, typename ValueT_> constexpr auto extractorName_(ValueT_ defaultValue_) { \
			return defaultValue_; \
		}; \
		\
		template <::namespaceName::ConceptName_ T_, typename ValueT_> constexpr auto extractorName_(ValueT_ defaultValue_) { \
			return T_::memberName_; \
		}; \
	}

#endif
