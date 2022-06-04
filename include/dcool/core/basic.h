#ifndef DCOOL_CORE_BASIC_H_INCLUDED_
#	define DCOOL_CORE_BASIC_H_INCLUDED_ 1

#	include <stdint.h>
#	include <time.h>

#	ifdef __cplusplus
#		define DCOOL_CORE_NO_RETURN [[noreturn]]
#	else
#		define DCOOL_CORE_NO_RETURN _Noreturn
#	endif

typedef enum {
	DCOOL_CORE_resultOk,
	DCOOL_CORE_resultUnknownError,
	DCOOL_CORE_resultOutOfMemory,
	DCOOL_CORE_resultBusy,
	DCOOL_CORE_resultTimedOut,
	DCOOL_CORE_resultInterrupted,
	DCOOL_CORE_resultUnsupported
} DCOOL_CORE_Result;

#	define DCOOL_CORE_FLAG_ENABLED(value, flag) (((value)& (flag)) != 0)

typedef struct {
	time_t seconds;
	uint32_t nanoseconds;
} DCOOL_CORE_Duration;

typedef struct {
	DCOOL_CORE_Duration sinceEpoch;
} DCOOL_CORE_TimePoint;

#endif
