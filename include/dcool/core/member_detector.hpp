#ifndef DCOOL_CORE_MEMBER_DETECTOR_HPP_INCLUDED_
#	define DCOOL_CORE_MEMBER_DETECTOR_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	define DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(namespaceName_, ConceptName_, ExtractorName_, MemberName_) \
		namespace namespaceName_ { \
			template <typename T_> concept ConceptName_ = requires { \
				typename T_::MemberName_; \
			}; \
			\
			template <typename T_, typename DefaultT_> struct ExtractorName_##DcoolCoreHelper_ { \
				using Result_ = DefaultT_; \
			}; \
			\
			template <::namespaceName_::ConceptName_ T_, typename DefaultT_> struct ExtractorName_##DcoolCoreHelper_<T_, DefaultT_> { \
				using Result_ = T_::MemberName_; \
			}; \
			\
			template < \
				typename T_, typename DefaultT_ \
			> using ExtractorName_ = ::namespaceName_::ExtractorName_##DcoolCoreHelper_<T_, DefaultT_>::Result_; \
		}

#	define DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(namespaceName_, ConceptName_, extractorName_, memberName_) \
		namespace namespaceName_ { \
			template <typename T_> concept ConceptName_ = requires { \
				typename ::dcool::core::RequiresConstant<T_::memberName_>; \
			}; \
			\
			template <typename T_, typename ValueT_> consteval auto extractorName_(ValueT_ defaultValue_) -> ValueT_ { \
				return defaultValue_; \
			} \
			\
			template < \
				::namespaceName_::ConceptName_ T_, typename ValueT_ \
			> consteval auto extractorName_(ValueT_ defaultValue_) -> ValueT_ { \
				return T_::memberName_; \
			} \
		}

#	define DCOOL_CORE_DEFINE_STATIC_MEMBER_DETECTOR(namespaceName_, ConceptName_, extractorName_, memberName_) \
		namespace namespaceName_ { \
			template <typename T_> concept ConceptName_ = requires { \
				T_::memberName_; \
			}; \
			\
			template <typename T_, typename ValueT_> constexpr auto extractorName_(ValueT_ defaultValue_) { \
				return defaultValue_; \
			}; \
			\
			template <::namespaceName_::ConceptName_ T_, typename ValueT_> constexpr auto extractorName_(ValueT_ defaultValue_) { \
				return T_::memberName_; \
			}; \
		}

#	define DCOOL_CORE_DEFINE_MEMBER_CALLER(namespaceName_, ConceptName_, callerName_, expectedMemberName_) \
		namespace namespaceName_ { \
			template < \
				typename T_, typename... ArgumentTs_ \
			> concept ConceptName_ = requires (T_&& object_, ArgumentTs_&&... parameters_) { \
				::dcool::core::forward<T_>(object_).expectedMemberName_(::dcool::core::forward<ArgumentTs_>(parameters_)...); \
			}; \
			\
			template < \
				typename AlternativeHandlerT_, typename T_, typename... ArgumentTs_ \
			> constexpr auto callerName_(AlternativeHandlerT_&& alternativeHandler_, T_&& object_, ArgumentTs_&&... parameters_) { \
				if constexpr (::namespaceName_::ConceptName_<T_, ArgumentTs_...>) { \
					return ::dcool::core::forward<T_>(object_).expectedMemberName_(::dcool::core::forward<ArgumentTs_>(parameters_)...); \
				} else { \
					return ::dcool::core::forward<AlternativeHandlerT_>(alternativeHandler_)( \
						::dcool::core::forward<T_>(object_), ::dcool::core::forward<ArgumentTs_>(parameters_)... \
					); \
				} \
			} \
		}

#	define DCOOL_CORE_DEFINE_STATIC_MEMBER_CALLER(namespaceName_, ConceptName_, callerName_, expectedMemberName_) \
		namespace namespaceName_ { \
			template < \
				typename T_, typename... ArgumentTs_ \
			> concept ConceptName_ = requires (ArgumentTs_&&... parameters_) { \
				T_::expectedMemberName_(::dcool::core::forward<ArgumentTs_>(parameters_)...); \
			}; \
			\
			template < \
				typename T_, typename AlternativeHandlerT_, typename... ArgumentTs_ \
			> constexpr auto callerName_(AlternativeHandlerT_&& alternativeHandler_, ArgumentTs_&&... parameters_) { \
				if constexpr (::namespaceName_::ConceptName_<T_, ArgumentTs_...>) { \
					return T_::expectedMemberName_(::dcool::core::forward<ArgumentTs_>(parameters_)...); \
				} else { \
					return ::dcool::core::forward<AlternativeHandlerT_>(alternativeHandler_)( \
						::dcool::core::forward<ArgumentTs_>(parameters_)... \
					); \
				} \
			} \
		}

#endif
