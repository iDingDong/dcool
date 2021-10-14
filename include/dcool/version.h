#ifndef DCOOL_VERSRION_H_INCLUDED_
#	define DCOOL_VERSRION_H_INCLUDED_ 1

#	include <dcool/basic.h>

#	define DCOOL_MAJOR_VERSION 0
#	define DCOOL_MINOR_VERSION 15
#	define DCOOL_PATCH_VERSION 0
#	define DCOOL_MAJOR_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_MAJOR_VERSION)
#	define DCOOL_MINOR_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_MINOR_VERSION)
#	define DCOOL_PATCH_VERSION_NATIVE_NAME DCOOL_TO_STR_(DCOOL_PATCH_VERSION)
#	define DCOOL_VERSION_NATIVE_NAME \
	DCOOL_MAJOR_VERSION_NATIVE_NAME "." DCOOL_MINOR_VERSION_NATIVE_NAME "." DCOOL_PATCH_VERSION_NATIVE_NAME
#	define DCOOL_VERSION_NAME DCOOL_STR_TO_U8_(DCOOL_VERSION_NATIVE_NAME)

#endif
