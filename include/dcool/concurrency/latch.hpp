#ifndef DCOOL_CONCURRENCY_LATCH_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_LATCH_HPP_INCLUDED_ 1

#	include <dcool/concurrency/atomic_flag.hpp>
#	include <dcool/concurrency/basic.hpp>

#	include <dcool/core.hpp>

#	include <atomic>
#	include <chrono>
#	include <condition_variable>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::concurrency::detail_, HasTypeCountForLatch_, ExtractedCountForLatchType_, Count)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueMinimalSizeForLatch_, extractedMinimalSizeForLatchValue_, minimalSize
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueTimedForLatch_, extractedTimedForLatchValue_, timed
)

namespace dcool::concurrency {
	namespace detail_ {
		template <typename ConfigT_> class LatchConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: using Count = ::dcool::concurrency::detail_::ExtractedCountForLatchType_<Config, ::dcool::core::Size>;
			public: static constexpr ::dcool::core::Boolean minimalSize =
				::dcool::concurrency::detail_::extractedMinimalSizeForLatchValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean timed =
				::dcool::concurrency::detail_::extractedTimedForLatchValue_<Config>(false)
			;

			static_assert(::dcool::core::Integral<Count>);
		};
	}

	template <typename T_> concept LatchConfig = requires {
		typename ::dcool::concurrency::detail_::LatchConfigAdaptor_<T_>;
	};

	template <
		::dcool::concurrency::LatchConfig ConfigT_
	> using LatchConfigAdaptor = ::dcool::concurrency::detail_::LatchConfigAdaptor_<ConfigT_>;

	template <::dcool::concurrency::LatchConfig ConfigT_ = ::dcool::core::Empty<>> struct Latch {
		private: using Self_ = Latch<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::LatchConfigAdaptor<Config>;
		public: using Count = ConfigAdaptor_::Count;
		public: static constexpr ::dcool::core::Boolean minimalSize = ConfigAdaptor_::minimalSize;
		public: static constexpr ::dcool::core::Boolean timed = ConfigAdaptor_::timed;
		private: static constexpr ::dcool::core::Boolean atomicOnly_ = minimalSize && (!timed);
		private: static constexpr ::dcool::core::Boolean booleanCount_ = ::dcool::core::isSame<Count, ::dcool::core::Boolean>;
		private: static constexpr Count expectedTermination_ = static_cast<Count>(0);

		private: struct DefaultUnderlying_ {
			Count count_;
			mutable ::std::condition_variable blocker_;
			mutable ::std::mutex mutex_;

			constexpr DefaultUnderlying_(Count expected_) noexcept: count_(expected_) {
			}
		};

		private: using AtomicUnderlying_ = ::dcool::core::ConditionalType<
			booleanCount_, ::dcool::concurrency::NegativeAtomicFlag, ::std::atomic<Count>
		>;
		private: using Underlying_ = ::dcool::core::ConditionalType<atomicOnly_, ::std::atomic<Count>, DefaultUnderlying_>;

		private: Underlying_ m_underlying_;

		public: constexpr explicit Latch(Count expected_) noexcept: m_underlying_(expected_) {
			if constexpr (!booleanCount_) {
				DCOOL_CORE_ASSERT(expected_ >= expectedTermination_);
			}
		}

		public: Latch(Self_ const&) = delete;
		public: Latch(Self_&&) = delete;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		public: constexpr void countDown() {
			if constexpr (booleanCount_) {
				// Boolean latch works differently 
				if constexpr (atomicOnly_) {
					DCOOL_CORE_ASSERT(this->m_underlying_.testAndClear());
					this->m_underlying_.notify_all();
				} else {
					{
						::std::lock_guard locker_(this->m_underlying_.mutex_);
						DCOOL_CORE_ASSERT(this->m_underlying_.count_);
						this->m_underlying_.count_ = false;
					}
					this->m_underlying_.blocker_.notify_all();
				}
			} else {
				this->countDown(1);
			}
		}

		public: constexpr void countDown(Count n_) requires (!booleanCount_) {
			if constexpr (atomicOnly_) {
				Count previousValue_ = this->m_underlying_.fetch_sub(n_);
				DCOOL_CORE_ASSERT(n_ <= previousValue_);
				if (n_ == previousValue_) {
					this->m_underlying_.notify_all();
				}
			} else {
				Count newCount_;
				{
					::std::lock_guard locker_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(n_ <= this->m_underlying_.count_);
					this->m_underlying_.count_ -= n_;
					newCount_ = this->m_underlying_.count_;
				}
				if (newCount_ == expectedTermination_) {
					this->m_underlying_.blocker_.notify_all();
				}
			}
		}

		public: constexpr auto tryWait() const noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (atomicOnly_) {
				result_ = (this->m_underlying_.load() == expectedTermination_);
			} else {
				::std::lock_guard locker_(this->m_underlying_.mutex_);
				result_ = (this->m_underlying_.count_ == expectedTermination_);
			}
			return result_;
		}

		public: template <typename DurationT__> constexpr auto tryWaitFor(
			DurationT__ duration_
		) -> ::dcool::core::Boolean requires (timed) {
			return this->tryWaitUntil(::std::chrono::steady_clock::now() + duration_);
		}

		public: template <typename TimePointT__> constexpr auto tryWaitUntil(
			TimePointT__ deadline_
		) -> ::dcool::core::Boolean requires (timed) {
			::std::unique_lock locker_(this->m_underlying_.mutex_);
			return this->m_underlying_.blocker_.wait_until(
				locker_,
				deadline_,
				[this]() -> ::dcool::core::Boolean {
					return this->m_underlying_.count_ == expectedTermination_;
				}
			);
		}

		public: constexpr void wait() const {
			if constexpr (atomicOnly_) {
				for (; ; ) {
					Count current_ = this->m_underlying_.load();
					if (current_ == expectedTermination_) {
						break;
					}
					this->m_underlying_.wait(current_);
				}
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() -> ::dcool::core::Boolean {
						return this->m_underlying_.count_ == expectedTermination_;
					}
				);
			}
		}

		public: constexpr auto arriveAndTryWait() -> ::dcool::core::Boolean {
			if constexpr (booleanCount_) {
				this->countDown();
			} else {
				return this->arriveAndTryWait(1);
			}
			return true;
		}

		public: constexpr auto arriveAndTryWait(Count n_) -> ::dcool::core::Boolean requires (!booleanCount_)  {
			if constexpr (atomicOnly_) {
				Count previousValue_ = this->m_underlying_.fetch_sub(n_);
				DCOOL_CORE_ASSERT(n_ <= previousValue_);
				if (n_ < previousValue_) {
					return false;
				}
				this->m_underlying_.notify_all();
			} else {
				{
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(n_ <= this->m_underlying_.count_);
					this->m_underlying_.count_ -= n_;
					if (this->m_underlying_.count_ > expectedTermination_) {
						return false;
					}
				}
				this->m_underlying_.blocker_.notify_all();
			}
			return true;
		}

		public: constexpr void arriveAndWait() {
			if constexpr (booleanCount_) {
				this->countDown();
			} else {
				this->arriveAndWait(1);
			}
		}

		public: constexpr void arriveAndWait(Count n_) {
			if constexpr (atomicOnly_) {
				Count previousValue_ = this->m_underlying_.fetch_sub(n_);
				DCOOL_CORE_ASSERT(n_ <= previousValue_);
				if (n_ == previousValue_) {
					this->m_underlying_.notify_all();
				} else {
					Count current_ = previousValue_ - n_;
					for (; ; ) {
						this->m_underlying_.wait(current_);
						current_ = this->m_underlying_.load();
						if (current_ == expectedTermination_) {
							break;
						}
					}
				}
			} else {
				{
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(n_ <= this->m_underlying_.count_);
					this->m_underlying_.count_ -= n_;
					if (this->m_underlying_.count_ > expectedTermination_) {
						::dcool::concurrency::permissiveWait(
							this->m_underlying_.blocker_,
							locker_,
							[this]() -> ::dcool::core::Boolean {
								return this->m_underlying_.count_ == expectedTermination_;
							}
						);
						return;
					}
				}
				this->m_underlying_.blocker_.notify_all();
			}
		}
	};

	namespace detail_ {
		template <typename CountT_> struct TimedLatchConfig_ {
			using Count = CountT_;
			static constexpr ::dcool::core::Boolean timed = true;
		};
	}

	template <
		typename CountT_ = ::dcool::core::Length
	> using TimedLatchOf = ::dcool::concurrency::Latch<::dcool::concurrency::detail_::TimedLatchConfig_<CountT_>>;
	using TimedLatch = ::dcool::concurrency::TimedLatchOf<>;
}

#endif
