#ifndef DCOOL_CONCURRENCY_MUTEX_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_MUTEX_HPP_INCLUDED_ 1

#	include <dcool/concurrency/atomic_flag.hpp>

#	include <dcool/config.hpp>
#	include <dcool/core.hpp>

#	include <atomic>
#	include <condition_variable>
#	include <chrono>
#	include <mutex>
#	include <thread>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueSpinnableForMutex_, extractedSpinnableForMutexValue_, spinnable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueTimedForMutex_, extractedTimedForMutexValue_, timed
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_,
	HasValueMaxShareForMutex_,
	extractedMaxShareForMutexValue_,
	maxShare
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueDowngradeableForMutex_, extractedDowngradeableForMutexValue_, downgradeable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueUpgradeableForMutex_, extractedUpgradeableForMutexValue_, upgradeable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_,
	HasValuePreferExclusiveForMutex_,
	extractedPreferExclusiveForMutexValue_,
	preferExclusive
)

namespace dcool::concurrency {
	constexpr ::dcool::core::UnsignedMaxInteger standardMutexMaxShare = 10000;

	namespace detail_ {
		template <typename ConfigT_> class MutexConfigAdaptor_ {
			private: using Self_ = MutexConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: static constexpr ::dcool::core::Boolean spinnable =
				::dcool::concurrency::detail_::extractedSpinnableForMutexValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean timed =
				::dcool::concurrency::detail_::extractedTimedForMutexValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::UnsignedMaxInteger maxShare =
				::dcool::concurrency::detail_::extractedMaxShareForMutexValue_<Config>(0)
			;
			public: static constexpr ::dcool::core::Boolean downgradeable =
				(maxShare > 0 && ::dcool::concurrency::detail_::extractedDowngradeableForMutexValue_<Config>(false))
			;
			public: static constexpr ::dcool::core::Boolean upgradeable =
				(maxShare > 0 && ::dcool::concurrency::detail_::extractedUpgradeableForMutexValue_<Config>(false))
			;
			public: static constexpr ::dcool::core::Triboolean preferExclusive =
				(
					maxShare > 0 &&
					::dcool::concurrency::detail_::extractedPreferExclusiveForMutexValue_<Config>(::dcool::core::indeterminate)
				)
			;
		};
	}

	template <typename T_> concept MutexConfig = requires {
		typename ::dcool::concurrency::detail_::MutexConfigAdaptor_<T_>;
	};

	template <typename ConfigT_> requires (::dcool::concurrency::MutexConfig<ConfigT_>) using MutexConfigAdaptor =
		::dcool::concurrency::detail_::MutexConfigAdaptor_<ConfigT_>
	;

	template <::dcool::concurrency::MutexConfig ConfigT_ = ::dcool::core::Empty<>> class Mutex {
		private: using Self_ = Mutex<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::MutexConfigAdaptor<Config>;
		public: static constexpr ::dcool::core::Boolean spinnable = ConfigAdaptor_::spinnable;
		public: static constexpr ::dcool::core::Boolean timed = ConfigAdaptor_::timed;
		public: static constexpr ::dcool::core::Length spinTimeCheckInterval = ConfigAdaptor_::spinTimeCheckInterval;
		public: static constexpr ::dcool::core::UnsignedMaxInteger maxShare = ConfigAdaptor_::maxShare;
		public: static constexpr ::dcool::core::Boolean shareable = (maxShare > 0);
		public: static constexpr ::dcool::core::Boolean downgradeable = ConfigAdaptor_::downgradeable;
		public: static constexpr ::dcool::core::Boolean upgradeable = ConfigAdaptor_::upgradeable;
		public: static constexpr ::dcool::core::Triboolean preferExclusive = ConfigAdaptor_::preferExclusive;
		private: static constexpr ::dcool::core::Boolean flexibleShareRequired_ = (
			shareable &&
			(
				maxShare > ::dcool::concurrency::standardMutexMaxShare ||
				preferExclusive.isDeterminate() ||
				downgradeable ||
				upgradeable
			)
		);

		private: using ShareCount_ = ::dcool::core::IntegerType<
			(preferExclusive.isDeterminateTrue()) ? maxShare - 1 + maxShare : maxShare
		>;

		private: static constexpr ShareCount_ exclusive_ = static_cast<ShareCount_>(maxShare);

		private: using SpinnableUnderlying_ = ::dcool::core::ConditionalType<
			shareable, ::std::atomic<ShareCount_>, ::dcool::concurrency::AtomicFlag
		>;

		private: struct FlexibleSharedUnderlying_ {
			::std::mutex mutex_;
			ShareCount_ shareCount_ = 0;
			::std::condition_variable shareBlocker_;
			::std::condition_variable blocker_;
		};

		private: using CommonUnderlying_ = ::dcool::core::ConditionalType<
			shareable,
			::dcool::core::ConditionalType<
				flexibleShareRequired_,
				FlexibleSharedUnderlying_,
				::dcool::core::ConditionalType<timed, ::std::shared_timed_mutex, ::std::shared_mutex>
			>,
			::dcool::core::ConditionalType<timed, ::std::timed_mutex, ::std::mutex>
		>;

		private: using Underlying_ = ::dcool::core::ConditionalType<spinnable, SpinnableUnderlying_, CommonUnderlying_>;

		private: Underlying_ m_underlying_;

		public: auto tryLock()& noexcept -> ::dcool::core::Boolean {
			if constexpr (spinnable) {
				if constexpr (shareable) {
					ShareCount_ expected_ = 0;
					if (
						!(
							this->m_underlying_.compare_exchange_strong(
								expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					) {
						return false;
					}
				} else {
					if (this->m_underlying_.testAndSet(::std::memory_order::acquire)) {
						return false;
					}
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
					if ((!(locker_.owns_lock())) || this->m_underlying_.shareCount_ != 0) {
						return false;
					}
					this->m_underlying_.shareCount_ = exclusive_;
				} else {
					if (!(this->m_underlying_.try_lock())) {
						return false;
					}
				}
			}
			return true;
		}

		public: template <typename ::dcool::core::Boolean strongAttemptC_ = true> auto tryLockShared(
		)& noexcept -> ::dcool::core::Boolean requires (shareable) {
			if constexpr (spinnable) {
				ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				do {
					if (old_ >= exclusive_) {
						return false;
					}
				} while (
					!(
						this->m_underlying_.compare_exchange_weak(
							old_, old_ + 1, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				);
			} else {
				if constexpr (flexibleShareRequired_) {
					if constexpr (strongAttemptC_) {
						::std::lock_guard guard_(this->m_underlying_.mutex_);
						if (this->m_underlying_.shareCount_ >= exclusive_) {
							return false;
						}
						++(this->m_underlying_.shareCount_);
					} else {
						// May fail spuriously.
						::std::unique_lock locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
						if ((!(locker_.owns_lock())) || this->m_underlying_.shareCount_ >= exclusive_) {
							return false;
						}
						++(this->m_underlying_.shareCount_);
					}
				} else {
					if (!(this->m_underlying_.try_lock_shared())) {
						return false;
					}
				}
			}
			return true;
		}

		public: template <typename TimePointT__> auto tryLockUntil(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed) {
			if constexpr (spinnable) {
				if constexpr (shareable) {
					ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					ShareCount_ nextValue_;
					do {
						while (old_ >= exclusive_) {
							if (TimePointT__::clock::now() > deadline_) {
								return false;
							}
							::std::this_thread::yield();
							old_ = this->m_underlying_.load(::std::memory_order::relaxed);
						}
						nextValue_ = old_ + exclusive_;
					} while (
						!(
							this->m_underlying_.compare_exchange_weak(
								old_, nextValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					);
					while (nextValue_ != exclusive_) {
						if (TimePointT__::clock::now() > deadline_) {
							ShareCount_ previousCount_ = this->m_underlying_.fetch_sub(exclusive_, ::std::memory_order::relaxed);
							DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
							return false;
						}
						DCOOL_CORE_ASSERT(nextValue_ > exclusive_);
						::std::this_thread::yield();
						nextValue_ = this->m_underlying_.load(::std::memory_order::acquire);
					}
				} else {
					while (!(this->tryLock())) {
						if (TimePointT__::clock::now() > deadline_) {
							return false;
						}
						::std::this_thread::yield();
#		if DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION >= DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION_FOR_P1135R6
						while (this->m_locked_.test(::std::memory_order::relaxed)) {
							if (TimePointT__::clock::now() > deadline_) {
								return false;
							}
							::std::this_thread::yield();
						}
#		endif
					}
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if constexpr (preferExclusive.isDeterminateTrue()) {
						if (
							!(
								this->m_underlying_.blocker_.wait_until(
									locker_,
									deadline_,
									[this]() {
										return this->m_underlying_.shareCount_ < exclusive_;
									}
								)
							)
						) {
							return false;
						}
						this->m_underlying_.shareCount_ += exclusive_;
						if (
							!(
								this->m_underlying_.blocker_.wait_until(
									locker_,
									deadline_,
									[this]() {
										DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
										return this->m_underlying_.shareCount_ == exclusive_;
									}
								)
							)
						) {
							DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ > exclusive_);
							this->m_underlying_.shareCount_ -= exclusive_;
							ShareCount_ countAfter_ = this->m_underlying_.shareCount_;
							locker_.unlock();
							this->m_underlying_.blocker_.notify_one();
							if (countAfter_ == 0) {
								this->m_underlying_.shareBlocker.notify_all();
							}
							return false;
						}
					} else {
						if (
							!(
								this->m_underlying_.blocker_.wait_until(
									locker_,
									deadline_,
									[this]() {
										this->m_underlying_.shareCount_ == 0;
									}
								)
							)
						) {
							return false;
						}
						this->m_underlying_.shareCount_ = exclusive_;
					}
				} else {
					return this->m_underlying_.try_lock_until(deadline_);
				}
			}
			return true;
		}

		public: template <typename TimePointT__> auto tryLockSharedUntil(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed && shareable) {
			if constexpr (spinnable) {
				while (!(this->tryLockShared())) {
					do {
						if (TimePointT__::clock::now() > deadline_) {
							return false;
						}
						::std::this_thread::yield();
					} while (this->m_underlying_.load(::std::memory_order::relaxed) >= exclusive_);
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									this->m_underlying_.shareCount_ < exclusive_;
								}
							)
						)
					) {
						return false;
					}
					++(this->m_underlying_.shareCount_);
				} else {
					return this->m_underlying_.try_lock_shared_until(deadline_);
				}
			}
			return true;
		}

		public: template <typename DurationT_> auto tryLockFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockUntil(::std::chrono::steady_clock::now() + duration_);
		}

		public: template <typename DurationT_> auto tryLockSharedFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockSharedUntil(::std::chrono::steady_clock::now() + duration_);
		}

		public: void lock()& noexcept(spinnable) {
			if constexpr (spinnable) {
				if constexpr (shareable) {
					if constexpr (preferExclusive.isDeterminateTrue()) {
						ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
						ShareCount_ newValue_;
						do {
							while (old_ >= exclusive_) {
								::std::this_thread::yield();
								old_ = this->m_underlying_.load(::std::memory_order::relaxed);
							}
							newValue_ = old_ + exclusive_;
						} while (
							!(
								this->m_underlying_.compare_exchange_weak(
									old_, newValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
								)
							)
						);
						while (newValue_ != exclusive_) {
							DCOOL_CORE_ASSERT(newValue_ > exclusive_);
							::std::this_thread::yield();
							newValue_ = this->m_underlying_.load(::std::memory_order::acquire);
						}
					} else {
						for (; ; ) {
							ShareCount_ expected_ = 0;
							if (
								this->m_underlying_.compare_exchange_weak(
									expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
								)
							) {
								break;
							}
							::std::this_thread::yield();
							while (this->m_underlying_.load(::std::memory_order::relaxed) > 0) {
								::std::this_thread::yield();
							}
						}
					}
				} else {
					while (!(this->tryLock())) {
						::std::this_thread::yield();
#		if DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION >= DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION_FOR_P1135R6
						while (this->m_underlying_.test(::std::memory_order::relaxed)) {
							::std::this_thread::yield();
						}
#		endif
					}
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if constexpr (preferExclusive.isDeterminateTrue()) {
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								return this->m_underlying_.shareCount_ < exclusive_;
							}
						);
						this->m_underlying_.shareCount_ += exclusive_;
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
								return this->m_underlying_.shareCount_ == exclusive_;
							}
						);
					} else {
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								return this->m_underlying_.shareCount_ == 0;
							}
						);
						this->m_underlying_.shareCount_ = exclusive_;
					}
				} else {
					this->m_underlying_.lock();
				}
			}
		}

		public: void lockShared()& noexcept(spinnable) requires (shareable) {
			if constexpr (spinnable) {
				while (!(this->tryLockShared())) {
					do {
						::std::this_thread::yield();
					} while (this->m_underlying_.load(::std::memory_order::relaxed) >= exclusive_);
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					this->m_underlying_.shareBlocker_.wait(
						locker_,
						[this]() {
							return this->m_underlying_.shareCount_ < exclusive_;
						}
					);
					++(this->m_underlying_.shareCount_);
				} else {
					this->m_underlying_.lock_shared();
				}
			}
		}

		public: void unlock()& noexcept {
			if constexpr (spinnable) {
				if constexpr (shareable) {
#	if DCOOL_DEBUG_ENABLED
					ShareCount_ previousCount_ = this->m_underlying_.exchange(0, ::std::memory_order::release);
					DCOOL_CORE_ASSERT(previousCount_ == exclusive_);
#	else
					this->m_underlying_.store(0, ::std::memory_order::release)
#	endif
				} else {
					this->m_underlying_.clear(::std::memory_order::release);
				}
			} else {
				if constexpr (flexibleShareRequired_) {
					{
						::std::lock_guard guard_(this->m_underlying_.mutex_);
						DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ == exclusive_);
						this->m_underlying_.shareCount_ = 0;
					}
					this->m_underlying_.blocker_.notify_one();
					this->m_underlying_.shareBlocker_.notify_all();
				} else {
					this->m_underlying_.unlock();
				}
			}
		}

		public: void unlockShared()& noexcept requires (shareable) {
			if constexpr (spinnable) {
				ShareCount_ previousCount_ = this->m_underlying_.fetch_sub(1, ::std::memory_order::release);
				DCOOL_CORE_ASSERT(previousCount_ > 0);
			} else {
				if constexpr (flexibleShareRequired_) {
					ShareCount_ countAfterUnlock_;
					{
						::std::lock_guard guard_(this->m_underlying_.mutex_);
						DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ > 0);
						countAfterUnlock_ = --(this->m_underlying_.shareCount_);
					}
					if (countAfterUnlock_ == 0) {
						this->m_underlying_.blocker_.notify_one();
					} else if (countAfterUnlock_ == exclusive_ - 1) {
						this->m_underlying_.shareBlocker_.notify_one();
					} else if constexpr (preferExclusive.isDeterminateTrue()) {
						if (countAfterUnlock_ == exclusive_) {
							this->m_underlying_.blocker_.notify_one();
						}
					} else if constexpr (upgradeable) {
						if (countAfterUnlock_ == 1) {
							this->m_underlying_.blocker_.notify_one();
						}
					}
				} else {
					this->m_underlying_.unlock_shared();
				}
			}
		}

		public: auto tryUpgrade()& noexcept(spinnable) -> ::dcool::core::Boolean requires (upgradeable) {
			if constexpr (spinnable) {
				ShareCount_ expected_ = 1;
				if (
					!(
						this->m_underlying_.compare_exchange_strong(
							expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				) {
					return false;
				}
				DCOOL_CORE_ASSERT(expected_ >= 1);
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
				if ((!(locker_.owns_lock())) || this->m_underlying_.shareCount_ != 1) {
					DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
					return false;
				}
				this->m_underlying_.shareCount_ = exclusive_;
			}
			return true;
		}

		public: template <typename TimePointT__> auto tryUpgradeUntil(
			TimePointT__ const& deadline_
		)& noexcept(spinnable) -> ::dcool::core::Boolean requires (timed && upgradeable) {
			if constexpr (spinnable) {
				if constexpr (preferExclusive.isDeterminateTrue()) {
					ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					ShareCount_ newValue_;
					do {
						// Another thread is locking. This would be a deadlock.
						DCOOL_CORE_ASSERT(old_ <= exclusive_);
						while (old_ >= exclusive_) {
							if (TimePointT__::clock::now() > deadline_) {
								return false;
							}
							::std::this_thread::yield();
							old_ = this->m_underlying_.load(::std::memory_order::relaxed);
						}
						DCOOL_CORE_ASSERT(old_ >= 1);
						newValue_ = old_ + (exclusive_ - 1);
					} while (
						!(this->m_underlying_.compare_exchange_weak(
							old_, newValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					);
					while (newValue_ != exclusive_) {
						if (TimePointT__::clock::now() > deadline_) {
							ShareCount_ previousCount_ = this->m_underlying_.fetch_sub(::std::memory_order::relaxed);
							DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
							return false;
						}
						DCOOL_CORE_ASSERT(newValue_ > exclusive_);
						::std::this_thread::yield();
						newValue_ = this->m_underlying_.load(::std::memory_order::acquire);
					}
				} else {
					for (; ; ) {
						ShareCount_ expected_ = 1;
						if (
							this->m_underlying_.compare_exchange_weak(
								expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						) {
							DCOOL_CORE_ASSERT(expected_ >= 1);
							break;
						}
						if (TimePointT__::clock::now() > deadline_) {
							return false;
						}
						::std::this_thread::yield();
						while (this->m_underlying_.load(::std::memory_order::relaxed) > 1) {
							if (TimePointT__::clock::now() > deadline_) {
								return false;
							}
							::std::this_thread::yield();
						}
					}
				}
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				if constexpr (preferExclusive.isDeterminateTrue()) {
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
									// Another thread is locking. This would be a deadlock.
									DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ <= exclusive_);
									return this->m_underlying_.shareCount_ < exclusive_;
								}
							)
						)
					) {
						return false;
					}
					this->m_underlying_.shareCount_ += (exclusive_ - 1);
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
									return this->m_underlying_.shareCount_ == exclusive_;
								}
							)
						)
					) {
						return false;
					}
				} else {
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
									return this->m_underlying_.shareCount_ == 1;
								}
							)
						)
					) {
						return false;
					}
					this->m_underlying_.shareCount_ = exclusive_;
				}
			}
			return true;
		}

		public: template <typename DurationT_> auto tryUpgradeFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed && upgradeable) {
			return this->tryUpgradeUntil(::std::chrono::steady_clock::now() + duration_);
		}

		public: void upgrade()& noexcept(spinnable) requires (upgradeable) {
			if constexpr (spinnable) {
				if constexpr (preferExclusive.isDeterminateTrue()) {
					ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					ShareCount_ newValue_;
					do {
						// Another thread is locking. This would be a deadlock.
						DCOOL_CORE_ASSERT(old_ <= exclusive_);
						while (old_ >= exclusive_) {
							::std::this_thread::yield();
							old_ = this->m_underlying_.load(::std::memory_order::relaxed);
						}
						DCOOL_CORE_ASSERT(old_ >= 1);
						newValue_ = old_ + (exclusive_ - 1);
					} while (
						!(this->m_underlying_.compare_exchange_weak(
							old_, newValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					);
					while (newValue_ != exclusive_) {
						DCOOL_CORE_ASSERT(newValue_ > exclusive_);
						::std::this_thread::yield();
						newValue_ = this->m_underlying_.load(::std::memory_order::acquire);
					}
				} else {
					for (; ; ) {
						ShareCount_ expected_ = 1;
						if (
							this->m_underlying_.compare_exchange_weak(
								expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						) {
							DCOOL_CORE_ASSERT(expected_ >= 1);
							break;
						}
						::std::this_thread::yield();
						while (this->m_underlying_.load(::std::memory_order::relaxed) > 1) {
							::std::this_thread::yield();
						}
					}
				}
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				if constexpr (preferExclusive.isDeterminateTrue()) {
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
							// Another thread is locking. This would be a deadlock.
							DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ <= exclusive_);
							return this->m_underlying_.shareCount_ < exclusive_;
						}
					);
					this->m_underlying_.shareCount_ += (exclusive_ - 1);
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
							return this->m_underlying_.shareCount_ == exclusive_;
						}
					);
				} else {
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
							return this->m_underlying_.shareCount_ == 1;
						}
					);
					this->m_underlying_.shareCount_ = exclusive_;
				}
			}
		}

		public: void downgrade() noexcept requires (downgradeable) {
			if constexpr (spinnable) {
#	if DCOOL_DEBUG_ENABLED
				ShareCount_ previousCount_ = this->m_underlying_.exchange(1, ::std::memory_order::release);
				DCOOL_CORE_ASSERT(previousCount_ == exclusive_);
#	else
				this->m_underlying_.store(1, ::std::memory_order::release)
#	endif
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ == exclusive_);
					this->m_underlying_.shareCount_ = 1;
				}
				if constexpr (preferExclusive.isDeterminateTrue()) {
					this->m_underlying_.blocker_.notify_one();
				}
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		public: auto tryOrderLock()& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				do {
					if (old_ >= exclusive_) {
						return false;
					}
				} while (
					!(
						this->m_underlying_.compare_exchange_weak(
							old_, old_ + exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				);
			} else {
				::std::lock_guard guard_(this->m_underlying_.mutex_);
				if (this->m_underlying_.shareCount_ >= exclusive_) {
					return false;
				}
				this->m_underlying_.shareCount_ += exclusive_;
			}
			return true;
		};

		public: auto tryCompleteLockOrder()& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			return this->tryCompleteExclusiveOrder_();
		}

		public: void orderLock()& noexcept requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				do {
					while (old_ >= exclusive_) {
						::std::this_thread::yield();
						old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					};
				} while (
					!(
						this->m_underlying_.compare_exchange_weak(
							old_, old_ + exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				);
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						return this->m_underlying_.shareCount_ < exclusive_;
					}
				);
				this->m_underlying_.shareCount_ += exclusive_;
			}
		};

		public: void completeLockOrder()& noexcept requires (preferExclusive.isDeterminateTrue()) {
			this->completeExclusiveOrder_();
		}

		public: void cancelLockOrder() noexcept requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ previousCount_ = this->m_underlying_.fetch_sub(exclusive_, ::std::memory_order::relaxed);
				DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
					this->m_underlying_.shareCount_ -= exclusive_;
				}
				this->m_underlying_.blocker_.notify_one();
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		public: auto tryOrderUpgrade(
		)& noexcept -> ::dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				for (; ; ) {
					if (old_ >= exclusive_) {
						return false;
					}
					DCOOL_CORE_ASSERT(old_ >= 1);
				} while (
					!(
						this->m_underlying_.compare_exchange_weak(
							old_, old_ + (exclusive_ - 1), ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				);
			} else {
				::std::lock_guard guard_(this->m_underlying_.mutex_);
				if (this->m_underlying_.shareCount_ >= exclusive_) {
					return false;
				}
				DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
				this->m_underlying_.shareCount_ += (exclusive_ - 1);
			}
			return true;
		};

		public: auto tryCompleteUpgradeOrder(
		)& noexcept -> ::dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			return this->tryCompleteExclusiveOrder_();
		}

		public: void orderUpgrade()& noexcept requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				do {
					while (old_ >= exclusive_) {
						::std::this_thread::yield();
						old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					};
					DCOOL_CORE_ASSERT(old_ >= 1);
				} while (
					!(
						this->m_underlying_.compare_exchange_weak(
							old_, old_ + (exclusive_ - 1), ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				);
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= 1);
						return this->m_underlying_.shareCount_ < exclusive_;
					}
				);
				this->m_underlying_.shareCount_ += (exclusive_ - 1);
			}
		};

		public: void completeUpgradeOrder()& noexcept requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			this->completeExclusiveOrder_();
		}

		public: void cancelUpgradeOrder() noexcept requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				ShareCount_ previousCount_ = this->m_underlying_.fetch_sub((exclusive_ - 1), ::std::memory_order::relaxed);
				DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
					this->m_underlying_.shareCount_ -= (exclusive_ - 1);
				}
				this->m_underlying_.blocker_.notify_one();
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		private: auto tryCompleteExclusiveOrder_(
		)& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			ShareCount_ currentShareCount_;
			if constexpr (spinnable) {
				currentShareCount_ = this->m_underlying_.load(::std::memory_order::acquire);
			} else {
				::std::lock_guard guard_(this->m_underlying_.mutex_);
				currentShareCount_ = this->m_underlying_.shareCount_;
			}
			DCOOL_CORE_ASSERT(currentShareCount_ >= exclusive_);
			return currentShareCount_ == exclusive_;
		}

		private: void completeExclusiveOrder_()& noexcept requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				while (!(this->tryCompleteExclusiveOrder_())) {
					::std::this_thread::yield();
				}
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						DCOOL_CORE_ASSERT(this->m_underlying_.shareCount_ >= exclusive_);
						return this->m_underlying_.shareCount_ == exclusive_;
					}
				);
			}
		}

		public: auto try_lock()& noexcept -> ::dcool::core::Boolean {
			return this->tryLock();
		}

		public: auto try_lock_shared()& noexcept -> ::dcool::core::Boolean requires (shareable) {
			return this->tryLockShared();
		}

		public: template <typename TimePointT__> auto try_lock_until(
			TimePointT__ const deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockUntil(deadline_);
		}

		public: template <typename TimePointT__> auto try_lock_shared_until(
			TimePointT__ const deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed && shareable) {
			return this->tryLockSharedUntil(deadline_);
		}

		public: template <typename DurationT_> auto try_lock_for(
			DurationT_ const duration_
		)& noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockFor(duration_);
		}

		public: template <typename DurationT_> auto try_lock_shared_for(
			DurationT_ const duration_
		)& noexcept -> ::dcool::core::Boolean requires (timed && shareable) {
			return this->tryLockSharedFor(duration_);
		}

		public: void lock_shared()& noexcept(spinnable) requires (shareable) {
			return this->lock_shared();
		}

		public: void unlock_shared()& noexcept requires (shareable) {
			return this->unlock_shared();
		}
	};

	template <typename MutexT_> struct Locker {
		private: using Self_ = Locker<MutexT_>;
		public: using Mutex = MutexT_;

		public: enum class Status {
			idle,
			lockOrdered,
			sharedLocked,
			upgradeOrdered,
			locked
		};

#	if DCOOL_DEBUG_ENABLED
		private: Mutex* m_mutex_ = ::dcool::core::nullPointer;
#	else
		private: Mutex* m_mutex_;
#	endif
		private: Status m_status_ = Status::idle;

		public: constexpr Locker() noexcept = default;

		public: constexpr Locker(Mutex& mutex_): m_mutex_(::dcool::core::addressOf(mutex_)) {
			this->lock();
		}

		public: constexpr Locker(
			Mutex& mutex_, Status status_
		) noexcept: m_mutex_(::dcool::core::addressOf(mutex_)), m_status_(status_) {
		}

		public: Locker(Self_ const&) = delete;

		public: constexpr Locker(Self_&& other_) noexcept: m_mutex_(other_.m_mutex_), m_status_(other_.m_status_) {
#	if DCOOL_DEBUG_ENABLED
			other_.m_mutex_ = ::dcool::core::nullPointer;
#	endif
			other_.m_status_ = Status::idle;
		}

		public: constexpr ~Locker() noexcept {
			switch (this->m_status_) {
				case Status::lockOrdered:
				if constexpr (Mutex::preferExclusive.isDeterminateTrue()) {
					this->cancelLockOrder();
				} else {
					::dcool::core::terminate();
				}
				break;
				case Status::upgradeOrdered:
				if constexpr (Mutex::upgradeable && Mutex::preferExclusive.isDeterminateTrue()) {
					this->cancelUpgradeOrder();
				} else {
					::dcool::core::terminate();
				}
				[[fallthrough]];
				case Status::sharedLocked:
				if constexpr (Mutex::shareable) {
					this->unlockShared();
				} else {
					::dcool::core::terminate();
				}
				break;
				case Status::locked:
				this->unlock();
				break;
				default:
				break;
			}
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
		}

		public: constexpr auto status() noexcept -> Status {
			DCOOL_CORE_ASSERT(this->m_mutex_ != ::dcool::core::nullPointer);
			return this->m_status_;
		}

		public: constexpr auto tryLockShared() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			if (this->m_mutex_->tryLockShared()) {
				this->m_status_ = Status::sharedLocked;
				return true;
			}
			return false;
		}

		public: constexpr void lockShared() {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			this->m_mutex_->lockShared();
			this->m_status_ = Status::sharedLocked;
		}

		public: constexpr void unlockShared() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::sharedLocked);
			this->m_mutex_->unlockShared();
			this->m_status_ = Status::idle;
		}

		public: constexpr auto tryLock() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			if (this->m_mutex_->tryLock()) {
				this->m_status_ = Status::locked;
				return true;
			}
			return false;
		}

		public: constexpr auto tryOrderLock() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			if (this->m_mutex_->tryOrderLock()) {
				this->m_status_ = Status::lockOrdered;
				return true;
			}
			return false;
		}

		public: constexpr void orderLock() {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			this->m_mutex_->orderLock();
			this->m_status_ = Status::lockOrdered;
		}

		public: constexpr auto tryCompleteLockOrder() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::lockOrdered);
			if (this->m_mutex_->tryCompleteLockOrder()) {
				this->m_status_ = Status::locked;
				return true;
			}
			return false;
		}

		public: constexpr void completeLockOrder() {
			DCOOL_CORE_ASSERT(this->status() == Status::lockOrdered);
			this->m_mutex_->completeLockOrder();
			this->m_status_ = Status::locked;
		}

		public: constexpr void cancelLockOrder() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::lockOrdered);
			this->m_mutex_->cancelLockOrder();
			this->m_status_ = Status::idle;
		}

		public: constexpr void lock() {
			DCOOL_CORE_ASSERT(this->status() == Status::idle);
			this->m_mutex_->lock();
			this->m_status_ = Status::locked;
		}

		public: constexpr void unlock() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::locked);
			this->m_mutex_->unlock();
			this->m_status_ = Status::idle;
		}

		public: constexpr auto tryUpgrade() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::sharedLocked);
			if (this->m_mutex_->tryUpgrade()) {
				this->m_status_ = Status::locked;
				return true;
			}
			return false;
		}

		public: constexpr auto upgrade() {
			DCOOL_CORE_ASSERT(this->status() == Status::sharedLocked);
			this->m_mutex_->upgrade();
			this->m_status_ = Status::locked;
		}

		public: constexpr auto tryOrderUpgrade() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::sharedLocked);
			if (this->m_mutex_->tryOrderUpgrade()) {
				this->m_status_ = Status::upgradeOrdered;
				return true;
			}
			return false;
		}

		public: constexpr void orderUpgrade() {
			DCOOL_CORE_ASSERT(this->status() == Status::sharedLocked);
			this->m_mutex_->orderUpgrade();
			this->m_status_ = Status::upgradeOrdered;
		}

		public: constexpr auto tryCompleteUpgradeOrder() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::upgradeOrdered);
			if (this->m_mutex_->tryCompleteUpgradeOrder()) {
				this->m_status_ = Status::locked;
				return true;
			}
			return false;
		}

		public: constexpr void completeUpgradeOrder() {
			DCOOL_CORE_ASSERT(this->status() == Status::upgradeOrdered);
			this->m_mutex_->completeUpgradeOrder();
			this->m_status_ = Status::locked;
		}

		public: constexpr void cancelUpgradeOrder() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::upgradeOrdered);
			this->m_mutex_->cancelUpgradeOrder();
			this->m_status_ = Status::sharedLocked;
		}

		public: constexpr auto downgrade() noexcept {
			DCOOL_CORE_ASSERT(this->status() == Status::locked);
			this->m_mutex_->downgrade();
			this->m_status_ = Status::sharedLocked;
		}
	};
}

#endif
