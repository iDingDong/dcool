#ifndef DCOOL_BASIC_H_INCLUDED_
#	define DCOOL_BASIC_H_INCLUDED_ 1

#	define DCOOL_TO_STR_HELPER_(from_) #from_
#	define DCOOL_TO_STR_(from_) DCOOL_TO_STR_HELPER_(from_)
#	define DCOOL_GLUE_HELPER_(prefix_, str_) prefix_##str_
#	define DCOOL_GLUE_(prefix_, str_) DCOOL_GLUE_HELPER_(prefix_, str_)
#	define DCOOL_STR_TO_U8_(str_) DCOOL_GLUE_(u8, str_)

#	ifdef __cplusplus
#		define DCOOL_EXTERN_C extern "C"
#	else
#		define DCOOL_EXTERN_C
#	endif

#endif
