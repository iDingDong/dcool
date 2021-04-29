#ifndef DCOOL_COROUTINE_SYNC_WAIT_HPP_INCLUDED_
#	define DCOOL_COROUTINE_SYNC_WAIT_HPP_INCLUDED_ 1

#	include <dcool/coroutine/basic.hpp>
#	include <dcool/coroutine/actual_awaiter.hpp>

#	include <dcool/core.hpp>

#	include <future>

namespace dcool::coroutine {
	namespace detail_ {
		template <typename AwaitedT_> class SyncWaitFuture_;

		template <typename AwaitedT_> class SyncWaitFuturePromise_ {
			private: using Self_ = SyncWaitFuturePromise_<AwaitedT_>;
			public: using Awaited_ = AwaitedT_;

			public: using Result_ = decltype(
				::dcool::core::declval<::dcool::coroutine::ActualAwaiterType<Awaited_>>().await_resume()
			);
			public: using Handle_ = ::dcool::coroutine::Handle<Self_>;
			private: using Future_ = ::dcool::coroutine::detail_::SyncWaitFuture_<AwaitedT_>;
			public: static constexpr ::dcool::core::Boolean hasReturnValue_ = ::dcool::core::NonVoid<Result_>;

			private: using StoredResult_ = ::dcool::core::ConditionalType<hasReturnValue_, ::dcool::core::Relay<Result_>, void>;
			private: using Underlying_ = ::std::promise<StoredResult_>;
			public: using ResultFuture_ = ::std::future<StoredResult_>;

			private: struct YieldAwaiter_ {
				public: friend ::dcool::coroutine::detail_::SyncWaitFuturePromise_<Awaited_>;

				private: [[no_unique_address]] ::dcool::core::StaticOptional<StoredResult_, hasReturnValue_> m_result_;

				private: template <typename ValueT__> constexpr YieldAwaiter_(
					ValueT__&& value_
				): m_result_{ .value = ::dcool::core::forward<ValueT__>(value_) } {
				}

				public: constexpr auto await_ready() const noexcept {
					return false;
				}

				public: constexpr auto await_suspend(Handle_ handle_) const noexcept {
					try {
						if constexpr (hasReturnValue_) {
							handle_.promise().m_result_.set_value(::dcool::core::move(m_result_.value));
						} else {
							handle_.promise().m_result_.set_value();
						}
					} catch (...) {
						handle_.promise().m_result_.set_exception(::dcool::core::currentException());
					}
				}

				public: constexpr void await_resume() const noexcept {
					DCOOL_CORE_ASSERT(false);
				}
			};

			private: struct FinalAwaiter_ {
				public: friend ::dcool::coroutine::detail_::SyncWaitFuturePromise_<Awaited_>;

				private: constexpr FinalAwaiter_() noexcept = default;

				public: constexpr auto await_ready() const noexcept {
					return false;
				}

				public: constexpr auto await_suspend(Handle_ handle_) const noexcept {
					handle_.promise().m_result_.set_exception(handle_.promise().m_unhandledException_);
				}

				public: constexpr void await_resume() const noexcept {
					DCOOL_CORE_ASSERT(false);
				}
			};

			public: friend struct YieldAwaiter_;
			public: friend struct FinalAwaiter_;

			private: Underlying_ m_result_;
			private: ::dcool::core::ExceptionPointer m_unhandledException_;

			public: constexpr auto initial_suspend() const noexcept -> ::dcool::coroutine::SuspendAlways {
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr auto final_suspend() const noexcept -> FinalAwaiter_ {
				DCOOL_CORE_ASSERT(this->m_unhandledException_);
				return FinalAwaiter_();
			}

			public: template <typename ValueT__> constexpr auto yield_value(ValueT__&& value_) noexcept -> YieldAwaiter_ requires (
				(!hasReturnValue_) || ::dcool::core::ConvertibleTo<ValueT__&&, Result_>
			) {
				return YieldAwaiter_(::dcool::core::forward<ValueT__&&>(value_));
			}

			public: constexpr auto get_return_object() noexcept -> Future_ {
				return Future_(Handle_::from_promise(*this));
			}

			public: constexpr void return_void() const noexcept {
				DCOOL_CORE_ASSERT(false);
			}

			public: constexpr void unhandled_exception() noexcept {
				this->m_unhandledException_ = ::dcool::core::currentException();
			}

			public: constexpr auto resultFuture_() -> ResultFuture_ {
				return this->m_result_.get_future();
			}
		};

		template <typename AwaitedT_> class SyncWaitFuture_ {
			private: using Self_ = SyncWaitFuture_();
			public: using Awaited = AwaitedT_;

			private: using Promise_ = ::dcool::coroutine::detail_::SyncWaitFuturePromise_<Awaited>;
			public: using Result_ = Promise_::Result_;
			public: using ResultFuture_ = Promise_::ResultFuture_;
			private: using Handle_ = Promise_::Handle_;
			private: static constexpr ::dcool::core::Boolean hasReturnValue_ = Promise_::hasReturnValue_;
			public: friend Promise_;

			public: using promise_type = Promise_;

			private: Handle_ m_handle_;
			private: ResultFuture_ m_resultFuture_;

			public: SyncWaitFuture_() = default;
			public: SyncWaitFuture_(Self_ const&) = delete;
			public: SyncWaitFuture_(Self_&& other_) = delete;

			private: constexpr SyncWaitFuture_(Handle_ handle_) noexcept:
				m_handle_(handle_), m_resultFuture_(handle_.promise().resultFuture_())
			{
			}

			public: constexpr ~SyncWaitFuture_() noexcept {
				this->m_handle_.destroy();
			}

			public: auto operator =(Self_ const&) -> Self_& = delete;
			private: auto operator =(Self_&& other_) -> Self_& = delete;

			public: auto result_() -> Result_ {
				this->m_handle_.resume();
				if constexpr (hasReturnValue_) {
					return this->m_resultFuture_.get().value();
				} else {
					this->m_resultFuture_.get();
				}
			}
		};

		template <typename AwaitedT_> auto syncWaitCoroutine(
			AwaitedT_&& awaiter_
		) noexcept -> ::dcool::coroutine::detail_::SyncWaitFuture_<AwaitedT_&&> {
			if constexpr (::dcool::core::NonVoid<::dcool::coroutine::detail_::SyncWaitFuture_<AwaitedT_&&>>) {
				co_yield co_await ::dcool::core::forward<AwaitedT_>(awaiter_);
			} else {
				co_await ::dcool::core::forward<AwaitedT_>(awaiter_);
				co_yield ::dcool::core::Empty<>();
			}
		}
	}

	template <typename AwaitedT_> decltype(auto) syncWait(AwaitedT_&& awaiter_) {
		auto result_ = ::dcool::coroutine::detail_::syncWaitCoroutine(::dcool::core::forward<AwaitedT_>(awaiter_));
		return result_.result_();
	}
}

#endif
