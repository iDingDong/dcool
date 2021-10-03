#ifndef DCOOL_CONCURRENCY_BASIC_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_BASIC_HPP_INCLUDED_ 1

#	ifdef DCOOLER_CONCURRENCY_PREFER_WAIT_WITH_PREDICATE
#		if DCOOLER_CONCURRENCY_PREFER_WAIT_WITH_PREDICATE
#			define DCOOLER_CONCURRENCY_WAIT_WITH_PREDICATE_PREFERRED 1
#		else
#			define DCOOLER_CONCURRENCY_WAIT_WITH_PREDICATE_PREFERRED 0
#		endif
#	else
#		define DCOOLER_CONCURRENCY_WAIT_WITH_PREDICATE_PREFERRED 1
#	endif

#	include <dcool/core.hpp>

#	include <condition_variable>
#	include <mutex>
#	include <thread>

namespace dcool::concurrency {
	const ::std::thread::id nullThreadId = ::std::thread::id();

	template <typename PredicateT_> constexpr void permissiveWait(
		::std::condition_variable& blocker_, ::std::unique_lock<::std::mutex>& locker_, PredicateT_&& predicate_
	) {
#	if DCOOLER_CONCURRENCY_WAIT_WITH_PREDICATE_PREFERRED
		blocker_.wait(locker_, ::dcool::core::forward<PredicateT_>(predicate_));
#	else
		do {
			blocker_.wait(locker_);
		} while (!::dcool::core::forward<PredicateT_>(predicate_)());
#	endif
	}
}

#endif
