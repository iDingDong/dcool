#ifndef DCOOL_COROUTINE_BASIC_HPP_INCLUDED_
#	define DCOOL_COROUTINE_BASIC_HPP_INCLUDED_ 1

#	include <coroutine>
#	include <stdexcept>

namespace dcool::coroutine {
	using SuspendAlways = ::std::suspend_always;
	using SuspendNever = ::std::suspend_never;

	inline constexpr ::dcool::coroutine::SuspendAlways suspendAlways;
	inline constexpr ::dcool::coroutine::SuspendNever suspendNever;

	template <typename PromiseT_> using Handle = ::std::coroutine_handle<PromiseT_>;

	using UniversalHandle = ::dcool::coroutine::Handle<void>;

	class PrematureResume: public ::std::logic_error {
		private: using Super_ = ::std::logic_error;

		public: using Super_::Super_;
	};
}

#endif
