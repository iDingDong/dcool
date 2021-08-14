#ifndef DCOOL_CONFIG_HPP_INCLUDED_
#	define DCOOL_CONFIG_HPP_INCLUDED_ 1

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

#	ifdef DCOOLER_ENABLE_CPP_2020
#		if DCOOLER_ENABLE_CPP_2020
#			define DCOOLER_CPP_2020_ENABLED 1
#		else
#			define DCOOLER_CPP_2020_ENABLED 0
#		endif
#	else
#		if __cplusplus >= 202002L
#			define DCOOLER_CPP_2020_ENABLED 1
#		else
#			define DCOOLER_CPP_2020_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P1135R6
#		if DCOOLER_ENABLE_CPP_P1135R6
#			define DCOOLER_CPP_P1135R6_ENABLED 1
#		else
#			define DCOOLER_CPP_P1135R6_ENABLED 0
#		endif
#	else
#		if \
			DCOOL_CPP_ATOMIC_WAIT_VERSION >= 201907 && \
			DCOOL_CPP_SEMAPHORE_VERSION >= 201907 && \
			DCOOL_CPP_LATCH_VERSION >= 201907 && \
			DCOOL_CPP_BARRIER_VERSION >= 201907 && \
			DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION >= 201907 && \
			DCOOL_CPP_ATOMIC_LOCK_FREE_TYPE_ALIASES_VERSION >= 201907
#			define DCOOLER_CPP_P1135R6_ENABLED 1
#		else
#			define DCOOLER_CPP_P1135R6_ENABLED 0
#		endif
#	endif

#	ifdef DCOOLER_ENABLE_CPP_P0024R2
#		if DCOOLER_ENABLE_CPP_P0024R2
#			define DCOOL_ENABLE_CPP_P0024R2 1
#		else
#			define DCOOL_ENABLE_CPP_P0024R2 0
#		endif
#	else
#		define DCOOLER_CPP_P1135R6_ENABLED 0
#	endif

#endif
