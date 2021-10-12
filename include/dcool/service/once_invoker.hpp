#ifndef DCOOL_SERVICE_ONCE_INVOKER_HPP_INCLUDED_
#	define DCOOL_SERVICE_ONCE_INVOKER_HPP_INCLUDED_ 1

#	include <dcool/concurrency.hpp>
#	include <dcool/config.hpp>
#	include <dcool/core.hpp>

#	include <atomic>
#	include <thread>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::service::detail_,
	HasValueSynchronizationStrategyForOnceInvoker,
	extractedSynchronizationStrategyForOnceInvokerValue_,
	synchronizationStrategy
)

namespace dcool::service {
	enum class OnceInvokerSynchronizationStrategy {
		standard,
		regular,
		busy,
		none
	};

	namespace detail_ {
		template <::dcool::core::Complete ConfigT_> class OnceInvokerConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: static constexpr ::dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy =
				::dcool::service::detail_::extractedSynchronizationStrategyForOnceInvokerValue_<Config>(
					::dcool::service::OnceInvokerSynchronizationStrategy::standard
				);
			;
		};
	}

	template <typename T_> concept OnceInvokerConfig = requires {
		typename ::dcool::service::detail_::OnceInvokerConfigAdaptor_<T_>;
	};

	template <::dcool::service::OnceInvokerConfig ConfigT_> using OnceInvokerConfigAdaptor =
		::dcool::service::detail_::OnceInvokerConfigAdaptor_<ConfigT_>
	;

	template <::dcool::service::OnceInvokerConfig ConfigT_ = ::dcool::core::Empty<>> struct OnceInvoker {
		private: using Self_ = OnceInvoker<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::service::OnceInvokerConfigAdaptor<Config>;
		public: static constexpr ::dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy =
			ConfigAdaptor_::synchronizationStrategy
		;

		private: enum class BusyStatus_ {
			idle_,
			invoking_,
			done_
		};

		private: using StandardFlag_ = ::dcool::core::ConditionalType<
			synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::standard,
			::std::once_flag,
			::dcool::core::Pit
		>;

		private: using RegularFlag_ = ::dcool::core::ConditionalType<
			synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::regular,
			::std::atomic<::dcool::core::Boolean>,
			::dcool::core::Pit
		>;

		private: using Mutex_ = ::dcool::core::ConditionalType<
			synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::regular,
			::dcool::concurrency::Mutex<>,
			::dcool::core::Empty<>
		>;

		private: using BusyFlag_ = ::dcool::core::ConditionalType<
			synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::busy,
			::std::atomic<BusyStatus_>,
			::dcool::core::Pit
		>;

		private: using TrivialFlag_ = ::dcool::core::ConditionalType<
			synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::none,
			::dcool::core::Boolean,
			::dcool::core::Pit
		>;

		private: [[no_unique_address]] StandardFlag_ m_standardFlag_;
		private: [[no_unique_address]] RegularFlag_ m_regularFlag_;
		private: [[no_unique_address]] BusyFlag_ m_busyFlag_;
		private: [[no_unique_address]] TrivialFlag_ m_trivialFlag_;
		private: [[no_unique_address]] Mutex_ m_mutex_;

		public: constexpr OnceInvoker(
		) noexcept: m_regularFlag_(false), m_busyFlag_(BusyStatus_::idle_), m_trivialFlag_(false) {
		}

		public: OnceInvoker(Self_ const&) = delete;
		public: OnceInvoker(Self_&&) = delete;

		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		public: constexpr auto done() const noexcept -> ::dcool::core::Boolean requires (
			synchronizationStrategy != ::dcool::service::OnceInvokerSynchronizationStrategy::standard
		) {
			::dcool::core::Boolean result_;
			if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::regular) {
				result_ = this->m_regularFlag_.load(::std::memory_order::acquire);
			} else if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::busy) {
				result_ = (this->m_busyFlag_.load(::std::memory_order::acquire) == BusyStatus_::done_);
			} else {
				result_ = this->m_trivialFlag_;
			}
			return result_;
		}

		private: template <typename FunctionT__, typename... ArgumentTs__> void standardInvoke_(
			FunctionT__&& function_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				::dcool::core::invoke(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				)
			)
		) requires (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::standard) {
			::std::call_once(
				this->m_standardFlag_,
				// Workaround for a standard library bug.
				// See document/dependency_bugs#Bug_4 for more details.
#	if DCOOL_DEPENDENCY_BUG_4
				[](FunctionT__&& function_, ArgumentTs__&&... parameters_) {
					try {
						::dcool::core::invoke(
							::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
						);
					} catch (...) {
						::dcool::core::terminate();
					}
				},
#	endif
				::dcool::core::forward<FunctionT__>(function_),
				::dcool::core::forward<ArgumentTs__>(parameters_)...
			);

			::std::call_once(
				this->m_standardFlag_,
				::dcool::core::forward<FunctionT__>(function_),
				::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		private: template <typename FunctionT__, typename... ArgumentTs__> void commonInvoke_(
			FunctionT__&& function_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				::dcool::core::invoke(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				)
			)
		) requires (synchronizationStrategy != ::dcool::service::OnceInvokerSynchronizationStrategy::standard) {
			if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::busy) {
				BusyStatus_ old_ = this->m_busyFlag_.load(::std::memory_order::acquire);
				do {
					while (old_ == BusyStatus_::invoking_) [[unlikely]] {
						::std::this_thread::yield();
						old_ = this->m_busyFlag_.load(::std::memory_order::acquire);
					}
					if (old_ == BusyStatus_::done_) {
						return;
					}
				} while (
					!(
						this->m_busyFlag_.compare_exchange_weak(
							old_, BusyStatus_::invoking_, ::std::memory_order::relaxed, ::std::memory_order::acquire
						)
					)
				);
			} else if (this->done()) {
				return;
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_mutex_);
			if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::regular) {
				if (this->m_regularFlag_.load(::std::memory_order::relaxed)) {
					return;
				}
			}
			try {
				::dcool::core::invoke(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			} catch (...) {
				if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::busy) {
					this->m_busyFlag_.store(BusyStatus_::idle_, ::std::memory_order::release);
				}
				throw;
			}
			if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::regular) {
				this->m_regularFlag_.store(true, ::std::memory_order::release);
			} else if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::busy) {
				this->m_busyFlag_.store(BusyStatus_::done_, ::std::memory_order::release);
			} else {
				this->m_trivialFlag_ = true;
			}
		}

		public: template <typename FunctionT__, typename... ArgumentTs__> void invoke(
			FunctionT__&& function_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				::dcool::core::invoke(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				)
			)
		) {
			if constexpr (synchronizationStrategy == ::dcool::service::OnceInvokerSynchronizationStrategy::standard) {
				this->standardInvoke_(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			} else {
				this->commonInvoke_(
					::dcool::core::forward<FunctionT__>(function_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}
		}
	};
}

#endif
