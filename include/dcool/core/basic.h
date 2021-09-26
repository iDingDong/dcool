#ifndef DCOOL_CORE_BASIC_H_INCLUDED_
#	define DCOOL_CORE_BASIC_H_INCLUDED_ 1

#	include <cstdint>
#	include <ctime>

#	ifdef __cplusplus
#		define DCOOL_CORE_NO_RETURN [[noreturn]]
#	else
#		define DCOOL_CORE_NO_RETURN _Noreturn
#	endif

typedef enum {
	DCOOL_CORE_Ok,
	DCOOL_CORE_Error,
	DCOOL_CORE_OutOfMemory,
	DCOOL_CORE_Busy,
	DCOOL_CORE_TimedOut,
	DCOOL_CORE_Interrupted,
	DCOOL_CORE_Unsupported
} DCOOL_CORE_Result;

#	define DCOOL_CORE_FLAG_ENABLED(value, flag) (((value)& (flag)) != 0)

typedef struct {
	::std::time_t seconds;
	::std::uint32_t nanoseconds;
} DCOOL_CORE_Duration;

typedef struct {
	DCOOL_CORE_Duration sinceEpoch;
} DCOOL_CORE_TimePoint;

#endif
