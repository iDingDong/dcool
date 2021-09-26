#ifndef DCOOL_CORE_DEBUG_INCLUDED_
#	define DCOOL_CORE_DEBUG_INCLUDED_ 1

#	include <dcool/config.h>
#	include <dcool/config.h>

#	if DCOOL_DEBUG_ENABLED
#		include <cassert>
#	endif

#	if DCOOL_DEBUG_ENABLED
#		define DCOOL_CORE_ASSERT(...) (assert(bool(__VA_ARGS__)))
#	else
#		define DCOOL_CORE_ASSERT(...) (static_cast<void>(0))
#	endif

#	if DCOOL_AGGRESSIVE_DEBUG_ENABLED
#		define DCOOL_CORE_AGGRESSIVE_ASSERT(...) DCOOL_CORE_ASSERT(__VA_ARGS__)
#	else
#		define DCOOL_CORE_AGGRESSIVE_ASSERT(...) (static_cast<void>(0))
#	endif

#endif
