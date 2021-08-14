#ifndef DCOOL_CONCURRENCY_MUTEX_H_INCLUDED_
#	define DCOOL_CONCURRENCY_MUTEX_H_INCLUDED_ 1

#	include <dcool/basic.h>
#	include <dcool/core.h>

#	include <stdint.h>
#	include <time.h>

struct DCOOL_CONCURRENCY_DETAIL_MutexHolder_;

typedef struct {
	struct DCOOL_CONCURRENCY_DETAIL_MutexHolder_* holder_;
} DCOOL_CONCURRENCY_Mutex;

typedef uint32_t DCOOL_CONCURRENCY_MutexAttribute;

static DCOOL_CONCURRENCY_MutexAttribute const DCOOL_CONCURRENCY_mutexDefaulted = 0;
static DCOOL_CONCURRENCY_MutexAttribute const DCOOL_CONCURRENCY_mutexShareable = 0b1;
static DCOOL_CONCURRENCY_MutexAttribute const DCOOL_CONCURRENCY_mutexTimed = 0b10;
static DCOOL_CONCURRENCY_MutexAttribute const DCOOL_CONCURRENCY_mutexRecursive = 0b100;

DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_initializeMutex(
	DCOOL_CONCURRENCY_Mutex* mutex_, DCOOL_CONCURRENCY_MutexAttribute attribute_ = DCOOL_CONCURRENCY_mutexDefaulted
);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_uninitializeMutex(DCOOL_CONCURRENCY_Mutex* mutex_);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_lockMutex(DCOOL_CONCURRENCY_Mutex* mutex_);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_tryLockMutex(DCOOL_CONCURRENCY_Mutex* mutex_);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_tryLockMutexUntil(
	DCOOL_CONCURRENCY_Mutex* mutex_, DCOOL_CORE_TimePoint const* time_
);
DCOOL_EXTERN_C DCOOL_CORE_Result DCOOL_CONCURRENCY_unlockMutex(DCOOL_CONCURRENCY_Mutex* mutex_);

#endif
