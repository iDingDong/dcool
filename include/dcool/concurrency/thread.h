#ifndef DCOOL_CONCURRENCY_C_THREAD_H_INCLUDED_
#	define DCOOL_CONCURRENCY_C_THREAD_H_INCLUDED_ 1

#	include <dcool/concurrency/thead_basic.h>

#	include <dcool/basic.h>
#	include <dcool/core.h>

struct DCOOL_CONCURRENCY_DETAIL_ThreadHolder_;

typedef struct {
	struct DCOOL_CONCURRENCY_DETAIL_ThreadHolder_* holder_;
} DCOOL_CONCURRENCY_Thread;

DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_initializeThread(
	DCOOL_CONCURRENCY_Thread* thread_, DCOOL_CONCURRENCY_ThreadProcedure procedure_, void* extraData_
);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_joinThread(DCOOL_CONCURRENCY_Thread* thread, void** outputResult);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_detachThread(DCOOL_CONCURRENCY_Thread* thread);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_sleepCurrentCThreadUntil(DCOOL_CORE_TimePoint const* deadline);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_yieldCurrentThread(void);

#endif
