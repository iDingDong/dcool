#ifndef DCOOL_CONCURRENCY_SEMAPHORE_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_SEMAPHORE_HPP_INCLUDED_ 1

#	include <dcool/config.h>
#	include <dcool/core.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::concurrency::detail_, HasTypeCountForSemaphore_, ExtractedCountForSemaphoreType_, Count)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueMinimalSizeForSemaphore_, extractedMinimalSizeForSemaphoreValue_, minimalSize
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueTimedForSemaphore_, extractedTimedForSemaphoreValue_, timed
)

namespace dcool::concurrency {
	namespace detail_ {
		template <typename ConfigT_> class SemaphoreConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: using Count = ::dcool::concurrency::detail_::ExtractedCountForSemaphoreType_<Config, ::dcool::core::Size>;
			public: static constexpr ::dcool::core::Boolean minimalSize =
				::dcool::concurrency::detail_::extractedMinimalSizeForSemaphoreValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean timed =
				::dcool::concurrency::detail_::extractedTimedForSemaphoreValue_<Config>(false)
			;

			static_assert(::dcool::core::Integral<Count>);
		};
	}

	template <typename T_> concept SemaphoreConfig = requires {
		typename ::dcool::concurrency::detail_::SemaphoreConfigAdaptor_<T_>;
	};

	template <
		::dcool::concurrency::SemaphoreConfig ConfigT_
	> using SemaphoreConfigAdaptor = ::dcool::concurrency::detail_::SemaphoreConfigAdaptor_<ConfigT_>;

	template <::dcool::concurrency::SemaphoreConfig ConfigT_ = ::dcool::core::Empty<>> struct Semaphore {
		private: using Self_ = Semaphore<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::SemaphoreConfigAdaptor<Config>;
		public: using Count = ConfigAdaptor_::Count;
		public: static constexpr ::dcool::core::Boolean minimalSize = ConfigAdaptor_::minimalSize;
		public: static constexpr ::dcool::core::Boolean timed = ConfigAdaptor_::timed;
		private: static constexpr ::dcool::core::Boolean atomicOnly_ = minimalSize && (!timed);

		private: struct DefaultUnderlying_ {
			Count count_;
			::std::condition_variable blocker_;
			::std::mutex mutex_;

			constexpr DefaultUnderlying_(Count expected_) noexcept: count_(expected_) {
			}
		};

		private: using Underlying_ = ::dcool::core::ConditionalType<atomicOnly_, ::std::atomic<Count>, DefaultUnderlying_>;

#	if DCOOL_AGGRESSIVE_DEBUG_ENABLED
		private: struct OperationCounterConfig_ {
			using Underlying = ::dcool::core::Size;
			static constexpr ::dcool::core::CounterScenario scenario = ::dcool::core::CounterScenario::logicDependent;
		};

		private: using OperationCounter_ = ::dcool::core::Counter<OperationCounterConfig_>;
		private: using OperationToken_ = OperationCounter_::PositiveToken;
#	else
		private: using OperationToken_ = ::dcool::core::Empty<>;
#	endif

		private: Underlying_ m_underlying_;
#	if DCOOL_AGGRESSIVE_DEBUG_ENABLED
		private: OperationCounter_ m_operationCount_;
#	endif

		public: constexpr explicit Semaphore(Count expected_ = 0) noexcept: m_underlying_(expected_) {
			DCOOL_CORE_ASSERT(expected_ >= 0);
		}

		public: Semaphore(Self_ const&) = delete;
		public: Semaphore(Self_&&) = delete;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		public: constexpr ~Semaphore() noexcept {
#	if DCOOL_AGGRESSIVE_DEBUG_ENABLED
			DCOOL_CORE_ASSERT(this->m_operationCount_.value() == 0);
#	endif
		}

		private: constexpr auto onOperationBegin_() noexcept -> OperationToken_ {
#	if DCOOL_AGGRESSIVE_DEBUG_ENABLED
			return this->m_operationCount_.positiveBorrow();
#	else
			return OperationToken_();
#	endif
		}

		public: constexpr void release() {
			[[maybe_unused]] auto token_ = this->onOperationBegin_();
			if constexpr (atomicOnly_) {
				++(this->m_underlying_);
				this->m_underlying_.notify_one();
			} else {
				{
					::std::lock_guard locker_(this->m_underlying_.mutex_);
					++(this->m_underlying_.count_);
				}
				this->m_underlying_.blocker_.notify_one();
			}
		}

		public: constexpr void release(Count n_) {
			auto token_ = this->onOperationBegin_();
			if constexpr (atomicOnly_) {
				this->m_underlying_ += n_;
				this->m_underlying_.notify_all();
			} else {
				{
					::std::lock_guard locker_(this->m_underlying_.mutex_);
					this->m_underlying_.count_ += n_;
				}
				this->m_underlying_.blocker_.notify_all();
			}
		}

		public: constexpr auto tryAcquire(Count n_ = 1) noexcept -> ::dcool::core::Boolean {
			[[maybe_unused]] auto token_ = this->onOperationBegin_();
			if constexpr (atomicOnly_) {
				for (; ; ) {
					Count current_ = this->m_underlying_.load(::std::memory_order::relaxed);
					if (current_ < n_) {
						break;
					}
					if (this->m_underlying_.compare_exchange_weak(current_, current_ - n_)) {
						return true;
					}
				}
			} else {
				::std::lock_guard locker_(this->m_underlying_.mutex_);
				if (this->m_underlying_.count_ >= n_) {
					this->m_underlying_.count_ -= n_;
					return true;
				}
			}
			return false;
		}

		public: template <typename DurationT_> constexpr auto tryAcquireFor(
			DurationT_ duration_, Count n_ = 1
		) noexcept -> ::dcool::core::Boolean {
			[[maybe_unused]] auto token_ = this->onOperationBegin_();
			return this->tryAcquireUntil(::std::chrono::steady_clock::now() + duration_);
		}

		public: template <typename TimePointT_> constexpr auto tryAcquireUntil(
			TimePointT_ deadline_, Count n_ = 1
		) noexcept -> ::dcool::core::Boolean {
			[[maybe_unused]] auto token_ = this->onOperationBegin_();
			::std::unique_lock locker_(this->m_underlying_.mutex_);
			if (!(this->m_underlying_.blocker_.wait_until(locker_, deadline_, [this, n_]() -> ::dcool::core::Boolean {
				return this->m_underlying_.count_ >= n_;
			}))) {
				return false;
			}
			this->m_underlying_.count_ -= n_;
			return true;
		}

		public: constexpr void acquire(Count n_ = 1) {
			[[maybe_unused]] auto token_ = this->onOperationBegin_();
			if constexpr (atomicOnly_) {
				for (; ; ) {
					Count current_ = this->m_underlying_.load(::std::memory_order::relaxed);
					if (current_ < n_) {
						this->m_underlying_.wait(current_);
					} else if (this->m_underlying_.compare_exchange_weak(current_, current_ - n_)) {
						break;
					}
				}
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(locker_, [this, n_]() -> ::dcool::core::Boolean {
					return this->m_underlying_.count_ >= n_;
				});
				this->m_underlying_.count_ -= n_;
			}
		}
	};

	namespace detail_ {
		struct TimedSemaphoreConfig_ {
			static constexpr ::dcool::core::Boolean timed = true;
		};
	}

	using TimedSemaphore = ::dcool::concurrency::Semaphore<::dcool::concurrency::detail_::TimedSemaphoreConfig_>;
}

#endif
