#ifndef DCOOL_VERSRION_H_INCLUDED_
#	define DCOOL_VERSRION_H_INCLUDED_ 1

#	define DCOOL_TO_STR_HELPER_(from_) #from_
#	define DCOOL_TO_STR_(from_) DCOOL_TO_STR_HELPER_(from_)
#	define DCOOL_GLUE_HELPER_(prefix_, str_) prefix_##str_
#	define DCOOL_GLUE_(prefix_, str_) DCOOL_GLUE_HELPER_(prefix_, str_)
#	define DCOOL_STR_TO_U8_(str_) DCOOL_GLUE_(u8, str_)

#	define DCOOL_MAJOR_VERSION 0
#	define DCOOL_MINOR_VERSION 11
#	define DCOOL_PATCH_VERSION 1
#	define DCOOL_MAJOR_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_MAJOR_VERSION)
#	define DCOOL_MINOR_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_MINOR_VERSION)
#	define DCOOL_PATCH_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_PATCH_VERSION)
#	define DCOOL_VERSION_NATIVE_NAME \
	DCOOL_MAJOR_VERSION_NATIVE_NAME "." DCOOL_MINOR_VERSION_NATIVE_NAME "." DCOOL_PATCH_VERSION_NATIVE_NAME
#	define DCOOL_VERSION_NAME DCOOL_STR_TO_U8_(DCOOL_VERSION_NATIVE_NAME)

#endif
