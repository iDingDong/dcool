#ifndef DCOOL_CONFIG_HPP_INCLUDED_
#	define DCOOL_CONFIG_HPP_INCLUDED_ 1

#	include <dcool/config.h>

#	include <version>

#	ifdef __cpp_lib_atomic_wait
#		define DCOOL_CPP_ATOMIC_WAIT_VERSION __cpp_lib_atomic_wait
#	else
#		define DCOOL_CPP_ATOMIC_WAIT_VERSION 0
#	endif

#	ifdef __cpp_lib_semaphore
#		define DCOOL_CPP_SEMAPHORE_VERSION __cpp_lib_semaphore
#	else
#		define DCOOL_CPP_SEMAPHORE_VERSION 0
#	endif

#	ifdef __cpp_lib_latch
#		define DCOOL_CPP_LATCH_VERSION __cpp_lib_latch
#	else
#		define DCOOL_CPP_LATCH_VERSION 0
#	endif

#	ifdef __cpp_lib_barrier
#		define DCOOL_CPP_BARRIER_VERSION __cpp_lib_barrier
#	else
#		define DCOOL_CPP_BARRIER_VERSION 0
#	endif

#	ifdef __cpp_lib_atomic_flag_test
#		define DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION __cpp_lib_atomic_flag_test
#	else
#		define DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION 0
#	endif

#	ifdef __cpp_lib_atomic_lock_free_type_aliases
#		define DCOOL_CPP_ATOMIC_LOCK_FREE_TYPE_ALIASES_VERSION __cpp_lib_atomic_lock_free_type_aliases
#	else
#		define DCOOL_CPP_ATOMIC_LOCK_FREE_TYPE_ALIASES_VERSION 0
#	endif

#	ifdef __cpp_lib_execution
#		define DCOOL_CPP_EXECUTION_VERSION __cpp_lib_execution
#	else
#		define DCOOL_CPP_EXECUTION_VERSION 0
#	endif

#	ifdef __cpp_lib_parallel_algorithm
#		define DCOOL_CPP_PARALLEL_ALGORITHM_VERSION __cpp_lib_parallel_algorithm
#	else
#		define DCOOL_CPP_PARALLEL_ALGORITHM_VERSION 0
#	endif

#	ifdef __cpp_modules
#		define DCOOL_CPP_MODULES_VERSION __cpp_modules
#	else
#		define DCOOL_CPP_MODULES_VERSION 0
#	endif

#	ifdef DCOOLER_ENABLE_CPP_2020
#		if DCOOLER_ENABLE_CPP_2020
#			define DCOOL_CPP_2020_ENABLED 1
#		else
#			define DCOOL_CPP_2020_ENABLED 0
#		endif
#	else
#		if __cplusplus >= 202002L
#			define DCOOL_CPP_2020_ENABLED 1
#		else
#			define DCOOL_CPP_2020_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P0024R2
#		if DCOOLER_ENABLE_CPP_P0024R2
#			define DCOOL_CPP_P0024R2_ENABLED 1
#		else
#			define DCOOL_CPP_P0024R2_ENABLED 0
#		endif
#	else
#		if DCOOL_CPP_EXECUTION_VERSION >= 201603L && DCOOL_CPP_PARALLEL_ALGORITHM_VERSION >= 201603L
#			define DCOOL_CPP_P0024R2_ENABLED 1
#		else
#			define DCOOL_CPP_P0024R2_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P1001R2
#		if DCOOLER_ENABLE_CPP_P1001R2
#			define DCOOL_CPP_P1001R2_ENABLED 1
#		else
#			define DCOOL_CPP_P1001R2_ENABLED 0
#		endif
#	else
#		if DCOOL_CPP_EXECUTION_VERSION >= 201902L
#			define DCOOL_CPP_P1001R2_ENABLED 1
#		else
#			define DCOOL_CPP_P1001R2_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P1103R3
#		if DCOOLER_ENABLE_CPP_P1103R3
#			define DCOOL_CPP_P1103R3_ENABLED 1
#		else
#			define DCOOL_CPP_P1103R3_ENABLED 0
#		endif
#	else
#		if DCOOL_CPP_MODULES_VERSION >= 201907L
#			define DCOOL_CPP_P1103R3_ENABLED 1
#		else
#			define DCOOL_CPP_P1103R3_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P1135R6
#		if DCOOLER_ENABLE_CPP_P1135R6
#			define DCOOL_CPP_P1135R6_ENABLED 1
#		else
#			define DCOOL_CPP_P1135R6_ENABLED 0
#		endif
#	else
#		if \
			DCOOL_CPP_ATOMIC_WAIT_VERSION >= 201907L && \
			DCOOL_CPP_SEMAPHORE_VERSION >= 201907L && \
			DCOOL_CPP_LATCH_VERSION >= 201907L && \
			DCOOL_CPP_BARRIER_VERSION >= 201907L && \
			DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION >= 201907L && \
			DCOOL_CPP_ATOMIC_LOCK_FREE_TYPE_ALIASES_VERSION >= 201907L
#			define DCOOL_CPP_P1135R6_ENABLED 1
#		else
#			define DCOOL_CPP_P1135R6_ENABLED 0
#		endif
#	endif

#	define DCOOL_CONFIG_NATIVE_DESCRIPTION_HELPER(seperator_, ending_) \
	DCOOL_C_CONFIG_NATIVE_DESCRIPTION_HELPER(seperator_, seperator_) \
	DCOOL_MACRO_DESCRIPTION(DCOOL_CPP_P0024R2_ENABLED) seperator_ \
	DCOOL_MACRO_DESCRIPTION(DCOOL_CPP_P1001R2_ENABLED) seperator_ \
	DCOOL_MACRO_DESCRIPTION(DCOOL_CPP_P1103R3_ENABLED) seperator_ \
	DCOOL_MACRO_DESCRIPTION(DCOOL_CPP_P1135R6_ENABLED) ending_

#	define DCOOL_CONFIG_NATIVE_DESCRIPTION DCOOL_CONFIG_NATIVE_DESCRIPTION_HELPER("\n", "\n")
#	define DCOOL_CONFIG_DESCRIPTION DCOOL_STR_TO_U8_(DCOOL_CONFIG_NATIVE_DESCRIPTION)

#	define DCOOL_SELF_NATIVE_DESCRIPTION \
	"dcool " DCOOL_VERSION_NATIVE_NAME "\n\t" DCOOL_CONFIG_NATIVE_DESCRIPTION_HELPER("\n\t", "\n")
#	define DCOOL_SELF_DESCRIPTION DCOOL_STR_TO_U8_(DCOOL_SELF_NATIVE_DESCRIPTION)

#endif
