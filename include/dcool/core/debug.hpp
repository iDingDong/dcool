#ifndef DCOOL_CORE_DEBUG_INCLUDED_
#	define DCOOL_CORE_DEBUG_INCLUDED_ 1

#	include <cassert>

#	if defined(NDEBUG)
#		define DCOOL_CORE_DEBUG_ENABLED 0
#	else
#		ifdef DCOOLER_CORE_ENABLE_DEBUG
#			if DCOOLER_CORE_ENABLE_DEBUG
#				define DCOOL_CORE_DEBUG_ENABLED 1
#			else
#				define DCOOL_CORE_DEBUG_ENABLED 0
#			endif
#		else
#			define DCOOL_CORE_DEBUG_ENABLED 1
#		endif
#	endif

#	ifdef DCOOLER_CORE_ENABLE_AGGRESSIVE_DEBUG
#		if DCOOLER_CORE_ENABLE_AGGRESSIVE_DEBUG
#			define DCOOL_CORE_AGGRESSIVE_DEBUG_ENABLED DCOOLER_CORE_ENABLE_DEBUG
#		else
#			define DCOOL_CORE_AGGRESSIVE_DEBUG_ENABLED 0
#		endif
#	else
#		define DCOOL_CORE_AGGRESSIVE_DEBUG_ENABLED 0
#	endif

#	if DCOOL_CORE_DEBUG_ENABLED
#		define DCOOL_CORE_ASSERT(...) (assert(bool(__VA_ARGS__)))
#	else
#		define DCOOL_CORE_ASSERT(...) (static_cast<void>(0))
#	endif

#endif
