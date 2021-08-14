#ifndef DCOOL_COROUTINE_TASK_HPP_INCLUDED_
#	define DCOOL_COROUTINE_TASK_HPP_INCLUDED_ 1

#	include <dcool/coroutine/basic.hpp>

#	include <dcool/core.hpp>

#	include <coroutine>
#	include <stdexcept>

namespace dcool::coroutine {
	template <typename ResultT_, typename ConfigT_ = ::dcool::core::Empty<>> class Task;

	namespace detail_ {
		template <typename ResultT_, typename ConfigT_> class TaskPromise_ {
			private: using Self_ = TaskPromise_<ResultT_, ConfigT_>;
			public: using Result = ResultT_;
			public: using Config = ConfigT_;

			public: using Handle_ = ::dcool::coroutine::Handle<Self_>;
			public: using Value_ = ::dcool::core::ReferenceRemovedType<Result>;
			private: using Task_ = ::dcool::coroutine::Task<Result, Config>;

			public: class FinalAwaiter_ {
				public: constexpr auto await_ready() noexcept -> ::dcool::core::Boolean {
					return false;
				}

				public: constexpr auto await_suspend(Handle_ handle_) noexcept -> ::dcool::coroutine::UniversalHandle {
					return handle_.promise().awaiting_;
				}

				public: void await_resume() noexcept {
				}
			};

			private: Value_* m_result_ = ::dcool::core::nullPointer;
			private: ::dcool::core::ExceptionPointer m_unhandledException_ = ::dcool::core::nullPointer;
			public: ::dcool::coroutine::UniversalHandle awaiting_ = ::dcool::core::nullPointer;

			public: constexpr auto result_() -> Result {
				::dcool::core::rethrowIfValid(this->m_unhandledException_);
				return static_cast<Result>(*(this->m_result_));
			}

			public: constexpr auto initial_suspend() const noexcept -> ::dcool::coroutine::SuspendAlways {
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr auto final_suspend() const noexcept -> FinalAwaiter_ {
				return FinalAwaiter_();
			}

			public: template <typename ValueT__> constexpr auto return_value(
				ValueT__&& value_
			) noexcept -> ::dcool::coroutine::SuspendAlways requires (::dcool::core::ConvertibleTo<ValueT__&&, Result>) {
				this->m_result_ = ::dcool::core::addressOf(value_);
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr void unhandled_exception() noexcept {
				this->m_unhandledException_ = ::std::current_exception();
			}

			public: constexpr auto get_return_object() noexcept -> Task_ {
				return Task_(Handle_::from_promise(*this));
			}
		};

		template <typename ConfigT_> class TaskPromise_<void, ConfigT_> {
			private: using Self_ = TaskPromise_<void, ConfigT_>;
			public: using Config = ConfigT_;

			public: using Result = void;
			public: using Handle_ = ::dcool::coroutine::Handle<Self_>;
			public: using Value_ = ::dcool::core::ReferenceRemovedType<Result>;
			private: using Task_ = ::dcool::coroutine::Task<Result, Config>;

			public: class FinalAwaiter_ {
				public: constexpr auto await_ready() noexcept -> ::dcool::core::Boolean {
					return false;
				}

				public: constexpr auto await_suspend(Handle_ handle_) noexcept -> ::dcool::coroutine::UniversalHandle {
					return handle_.promise().awaiting_;
				}

				public: void await_resume() noexcept {
				}
			};

			private: ::dcool::core::ExceptionPointer m_unhandledException_ = ::dcool::core::makeExceptionPointer(
				::dcool::coroutine::PrematureResume("Result not yet 'co_returned' by task.")
			);
			public: ::dcool::coroutine::UniversalHandle awaiting_ = ::dcool::core::nullPointer;

			public: constexpr auto result_() -> Result {
				::dcool::core::rethrowIfValid(this->m_unhandledException_);
			}

			public: constexpr auto initial_suspend() const noexcept -> ::dcool::coroutine::SuspendAlways {
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr auto final_suspend() const noexcept -> FinalAwaiter_ {
				return FinalAwaiter_();
			}

			public: constexpr void return_void() noexcept {
				this->m_unhandledException_ = ::dcool::core::nullPointer;
			}

			public: constexpr void unhandled_exception() noexcept {
				this->m_unhandledException_ = ::std::current_exception();
			}

			public: constexpr auto get_return_object() noexcept -> Task_ {
				return Task_(Handle_::from_promise(*this));
			}
		};

		template <typename ResultT_, typename ConfigT_> class TaskAwaiter_ {
			private: using Self_ = TaskAwaiter_<ResultT_, ConfigT_>;
			public: using Result = ResultT_;
			public: using Config = ConfigT_;

			public: using Promise_ = ::dcool::coroutine::detail_::TaskPromise_<Result, Config>;
			public: using Handle_ = Promise_::Handle_;
			private: static constexpr ::dcool::core::Boolean hasReturnValue_ = Promise_::hasReturnValue_;

			private: Handle_ m_handle_;

			public: constexpr TaskAwaiter_() noexcept = delete;

			public: constexpr explicit TaskAwaiter_(Handle_ handle_) noexcept: m_handle_(handle_) {
			}

			public: constexpr auto await_ready() noexcept -> ::dcool::core::Boolean {
				return this->m_handle_.done();
			}

			public:  constexpr auto await_suspend(::dcool::coroutine::UniversalHandle handle_) noexcept -> Handle_ {
				this->m_handle_.promise().awaiting_ = handle_;
				return this->m_handle_;
			}

			public: constexpr auto await_resume() -> Result {
				return this->m_handle_.promise().result_();
			}
		};
	}

	template <typename ResultT_, typename ConfigT_> class Task {
		private: using Self_ = Task<ResultT_, ConfigT_>;
		public: using Result = ResultT_;
		public: using Config = ConfigT_;

		private: using Awaiter_ = ::dcool::coroutine::detail_::TaskAwaiter_<Result, Config>;
		private: using Promise_ = Awaiter_::Promise_;
		private: using Handle_ = Awaiter_::Handle_;
		public: friend Promise_;

		public: using promise_type = Promise_;

		private: Handle_ m_handle_ = ::dcool::core::nullPointer;

		public: constexpr Task() noexcept = default;
		public: Task(Self_ const&) = delete;

		public: constexpr Task(Self_&& other_) noexcept: m_handle_(other_.m_handle_) {
			other_.m_handle_ = ::dcool::core::nullPointer;
		}

		private: constexpr Task(Handle_ handle_) noexcept: m_handle_(handle_) {
		}

		public: auto operator =(Self_ const&) -> Self_& = delete;

		private: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			::dcool::core::intelliSwap(this->m_handle_, other_.m_handle_);
		}

		public: constexpr ~Task() noexcept {
			if (this->m_handle_ != ::dcool::core::nullPointer) {
				this->m_handle_.destroy();
			}
		}

		public: constexpr auto operator co_await() noexcept {
			return Awaiter_(this->m_handle_);
		}
	};
}

#endif
