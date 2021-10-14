#ifndef DCOOL_CONCURRENCY_MUTEX_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_MUTEX_HPP_INCLUDED_ 1

#	include <dcool/concurrency/atom.hpp>
#	include <dcool/concurrency/atomic_flag.hpp>
#	include <dcool/concurrency/basic.hpp>

#	include <dcool/config.hpp>
#	include <dcool/core.hpp>

#	include <atomic>
#	include <condition_variable>
#	include <chrono>
#	include <mutex>
#	include <stdexcept>
#	include <thread>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueSpinnableForMutex_, extractedSpinnableForMutexValue_, spinnable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueTimedForMutex_, extractedTimedForMutexValue_, timed
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueMaxRecursionForMutex_, extractedMaxRecursionForMutexValue_, maxRecursion
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_,
	HasValueMaxShareForMutex_,
	extractedMaxShareForMutexValue_,
	maxShare
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueRecursiveSharedForMutex_, extractedRecursiveSharedForMutexValue_, recursiveShared
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
	class ExcessRecursiveLockDepth: public ::std::runtime_error {
		private: using Super_ = ::std::runtime_error;

		public: using Super_::Super_;
	};

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
			public: static constexpr ::dcool::core::UnsignedMaxInteger maxRecursion =
				::dcool::concurrency::detail_::extractedMaxRecursionForMutexValue_<Config>(0)
			;
			public: static constexpr ::dcool::core::UnsignedMaxInteger maxShare =
				::dcool::concurrency::detail_::extractedMaxShareForMutexValue_<Config>(0)
			;
			public: static constexpr ::dcool::core::Boolean recursiveShared =
				(maxShare > 0 && ::dcool::concurrency::detail_::extractedRecursiveSharedForMutexValue_<Config>(false))
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
		public: static constexpr ::dcool::core::UnsignedMaxInteger maxRecursion = ConfigAdaptor_::maxRecursion;
		public: static constexpr ::dcool::core::Boolean recursive = (maxRecursion > 0);
		public: static constexpr ::dcool::core::UnsignedMaxInteger maxShare = ConfigAdaptor_::maxShare;
		public: static constexpr ::dcool::core::Boolean shareable = (maxShare > 0);
		public: static constexpr ::dcool::core::Boolean recursiveShared = ConfigAdaptor_::recursiveShared;
		public: static constexpr ::dcool::core::Boolean downgradeable = ConfigAdaptor_::downgradeable;
		public: static constexpr ::dcool::core::Boolean upgradeable = ConfigAdaptor_::upgradeable;
		public: static constexpr ::dcool::core::Triboolean preferExclusive = ConfigAdaptor_::preferExclusive;
		private: static constexpr ::dcool::core::Boolean flexibleRequired_ = (
			shareable &&
			(
				recursive ||
				maxShare > ::dcool::concurrency::standardMutexMaxShare ||
				recursiveShared ||
				downgradeable ||
				upgradeable ||
				preferExclusive.isDeterminate()
			)
		);
		private: static constexpr ::dcool::core::UnsignedMaxInteger exclusiveValue_ = maxShare;
		private: static constexpr ::dcool::core::UnsignedMaxInteger maxStatusValue_ =
			recursive ?
				::dcool::core::max(maxRecursion, maxShare) :
				exclusiveValue_ + ((preferExclusive.isDeterminateTrue()) ? exclusiveValue_ - 1 : 0)
		;

		private: using StatusCount_ = ::dcool::core::IntegerType<maxStatusValue_>;

		// Value `exclusive_` only makes sense when not `recursive`.
		private: static constexpr StatusCount_ exclusive_ = static_cast<StatusCount_>(exclusiveValue_);
		private: static constexpr StatusCount_ maxStatus_ = static_cast<StatusCount_>(maxStatusValue_);

		private: struct RecursiveSpinnableSharedAtomUnderlying_ {
			StatusCount_ statusCount_;
			::std::thread::id exclusiveOwner_;
		};

		private: using SpinnableUnderlying_ = ::dcool::core::ConditionalType<
			recursive,
			::dcool::concurrency::Atom<RecursiveSpinnableSharedAtomUnderlying_>,
			::dcool::core::ConditionalType<
				shareable, ::dcool::concurrency::Atom<StatusCount_>, ::dcool::concurrency::AtomicFlag
			>
		>;

		private: struct FlexibleUnderlying_ {
			::std::mutex mutex_;
			StatusCount_ statusCount_ = 0;
			::std::condition_variable shareBlocker_;
			::std::condition_variable blocker_;
			[[no_unique_address]] ::dcool::core::StaticOptional<::std::thread::id, recursive> exclusiveOwner_;
		};

		private: using CommonUnderlying_ = ::dcool::core::ConditionalType<
			shareable,
			::dcool::core::ConditionalType<
				flexibleRequired_,
				FlexibleUnderlying_,
				::dcool::core::ConditionalType<timed, ::std::shared_timed_mutex, ::std::shared_mutex>
			>,
			::dcool::core::ConditionalType<
				recursive,
				::dcool::core::ConditionalType<timed, ::std::recursive_timed_mutex, ::std::recursive_mutex>,
				::dcool::core::ConditionalType<timed, ::std::timed_mutex, ::std::mutex>
			>
		>;

		private: using Underlying_ = ::dcool::core::ConditionalType<spinnable, SpinnableUnderlying_, CommonUnderlying_>;

		private: Underlying_ m_underlying_;

		private: auto tryRecursiveSpinningLock_()& noexcept -> ::dcool::core::Boolean requires (spinnable && recursive) {
			RecursiveSpinnableSharedAtomUnderlying_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			RecursiveSpinnableSharedAtomUnderlying_ new_ = {
				.exclusiveOwner_ = ::std::this_thread::get_id()
			};
			if (old_.exclusiveOwner_ == new_.exclusiveOwner_) {
				if (old_.statusCount_ == maxRecursion) {
					return false;
				}
				new_.statusCount_ = old_.statusCount_ + 1;
				this->m_underlying_.store(new_, ::std::memory_order::relaxed);
			} else {
				for (; ; ) {
					if (old_.exclusiveOwner_ != ::dcool::concurrency::nullThreadId) {
						return false;
					}
					if (old_.statusCount_ > 0) {
						DCOOL_CORE_ASSERT(old_.statusCount_ < maxShare);
						return false;
					}
					if (
						::dcool::concurrency::atomicallyCompareExchangeWeak(
							old_, new_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					) {
						break;
					}
				}
			}
			return true;
		}

		private: auto trySpinningLock_()& noexcept -> ::dcool::core::Boolean requires (spinnable) {
			::dcool::core::Boolean result_;
			if constexpr (recursive) {
				result_ = this->tryRecursiveSpinningLock_();
			} else if constexpr (shareable) {
				StatusCount_ expected_ = 0;
				result_ = this->m_underlying_.compare_exchange_strong(
					expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
				);
			} else {
				result_ = !(this->m_underlying_.testAndSet(::std::memory_order::acquire));
			}
			return result_;
		}

		public: auto tryLock()& noexcept -> ::dcool::core::Boolean {
			if constexpr (spinnable) {
				return this->trySpinningLock_();
			} else if constexpr (flexibleRequired_) {
				::std::unique_lock locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
				if (!(locker_.owns_lock())) {
					return false;
				}
				if constexpr (recursive) {
					::std::thread::id myId_ = ::std::this_thread::get_id();
					if (this->m_underlying_.exclusiveOwner_.value == ::dcool::concurrency::nullThreadId) {
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ <= maxShare);
						if (this->m_underlying_.statusCount_ > 0) {
							return false;
						}
						this->m_underlying_.exclusiveOwner_.value = myId_;
					} else {
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ <= maxRecursion);
						if (this->m_underlying_.exclusiveOwner_.value == myId_) {
							if (this->m_underlying_.statusCount_ == maxRecursion) {
								return false;
							}
							++(this->m_underlying_.statusCount_);
						} else {
							return false;
						}
					}
				} else {
					if (this->m_underlying_.statusCount_ > 0) {
						return false;
					}
					this->m_underlying_.statusCount_ = exclusive_;
				}
			} else {
				return this->m_underlying_.try_lock();
			}
			return true;
		}

		private: auto tryRecursiveSpinningLockShared_(
		)& noexcept -> ::dcool::core::Boolean requires (spinnable && recursive && shareable) {
			RecursiveSpinnableSharedAtomUnderlying_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			RecursiveSpinnableSharedAtomUnderlying_ new_ {
				.exclusiveOwner_ = ::dcool::concurrency::nullThreadId
			};
			do {
				if (old_.exclusiveOwner_ != ::dcool::concurrency::nullThreadId) {
					DCOOL_CORE_ASSERT(old_.statusCount_ <= maxRecursion);
					return false;
				}
				DCOOL_CORE_ASSERT(old_.statusCount_ <= maxShare);
				if (old_.statusCount_ == maxShare) {
					return false;
				}
				new_.statusCount_ = old_.statusCount_ + 1;
			} while (
				!(this->m_underlying_.compare_exchange_weak(old_, new_, ::std::memory_order::acquire, ::std::memory_order::relaxed))
			);
			return true;
		}

		private: auto trySpinningLockShared_(
		)& noexcept -> ::dcool::core::Boolean requires (spinnable && shareable) {
			if constexpr (recursive) {
				return this->tryRecursiveSpinningLockShared_();
			} else {
				StatusCount_ old_ = this->m_underlying_.fetchTransformOrLoad(
					[](StatusCount_ old_) noexcept -> ::dcool::core::Optional<StatusCount_> {
						if (old_ >= exclusive_) {
							return ::dcool::core::nullOptional;
						}
						return old_ + 1;
					},
					::std::memory_order::acquire,
					::std::memory_order::relaxed
				);
				if (old_ >= exclusive_) {
					return false;
				}
			}
			return true;
		}

		public: template <typename ::dcool::core::Boolean strongAttemptC_ = true> auto tryLockShared(
		)& noexcept -> ::dcool::core::Boolean requires (shareable) {
			if constexpr (spinnable) {
				return this->trySpinningLockShared_();
			} else if constexpr (flexibleRequired_) {
				using Guard_ = ::dcool::core::ConditionalType<strongAttemptC_, ::std::lock_guard<::std::mutex>, ::dcool::core::Pit>;
				Guard_ guard_(this->m_underlying_.mutex_);
				using Locker_ = ::dcool::core::ConditionalType<strongAttemptC_, ::std::unique_lock<::std::mutex>, ::dcool::core::Pit>;
				Locker_ locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
				if constexpr (!strongAttemptC_) {
					if (!(locker_.owns_lock())) {
						return false;
					}
				}
				if constexpr (recursive) {
					if (this->m_underlying_.exclusiveOwner_.value != ::dcool::concurrency::nullThreadId) {
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ <= maxRecursion);
						return false;
					}
					if (this->m_underlying_.statusCount_ >= maxShare) {
						return false;
					}
				} else {
					if (this->m_underlying_.statusCount_ >= exclusive_) {
						return false;
					}
				}
				++(this->m_underlying_.statusCount_);
			} else {
				return this->m_underlying_.try_lock_shared();
			}
			return true;
		}

		private: template <typename TimePointT__> auto tryRecursiveSpinningLockUntil_(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (spinnable && timed && recursive) {
			RecursiveSpinnableSharedAtomUnderlying_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			RecursiveSpinnableSharedAtomUnderlying_ new_ = {
				.exclusiveOwner_ = ::std::this_thread::get_id()
			};
			for (; ; ) {
				if (old_.exclusiveOwner_ == new_.exclusiveOwner_) {
					DCOOL_CORE_ASSERT(old_.statusCount_ <= maxRecursion);
					if (old_.statusCount_ == maxRecursion) {
						return false;
					}
					new_.statusCount_ = old_.statusCount_ + 1;
					this->m_underlying_.store(new_, ::std::memory_order::relaxed);
					break;
				}
				if (old_.exclusiveOwner_ == ::dcool::concurrency::nullThreadId) {
					DCOOL_CORE_ASSERT(old_.statusCount_ < maxShare);
					if constexpr (preferExclusive.isDeterminateTrue()) {
						new_.statusCount_ = old_.statusCount_;
						if (
							this->m_underlying_.compareExchangeWeak(
								old_, new_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						) {
							while (new_.statusCount_ != 0) {
								if (TimePointT__::clock::now() > deadline_) {
									RecursiveSpinnableSharedAtomUnderlying_ reverted_ = {
										.exclusiveOwner_ = ::dcool::concurrency::nullThreadId,
										.statusCount_ = new_.statusCount_
									};
									while ((!this->m_underlying_.compareExchangeWeak(new_, reverted_, ::std::memory_order::relaxed))) {
										DCOOL_CORE_ASSERT(new_.statusCount_ < maxShare);
										reverted_.statusCount_ = new_.statusCount_;
									}
									return false;
								}
								::std::this_thread::yield();
								new_ = this->m_underlying_.load(::std::memory_order::acquire);
								DCOOL_CORE_ASSERT(new_.statusCount_ < maxShare);
							}
							break;
						}
					} else {
						if constexpr (shareable) {
							if (old_.statusCount_ > 0) {
								if (TimePointT__::clock::now() > deadline_) {
									return false;
								}
								::std::this_thread::yield();
								continue;
							}
							new_.statusCount_ = 0;
						}
						DCOOL_CORE_ASSERT(old_.statusCount_ == 0);
						if (
							this->m_underlying_.compare_exchange_weak(
								old_, new_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						) {
							break;
						}
					}
					continue;
				}
				if (TimePointT__::clock::now() > deadline_) {
					return false;
				}
				::std::this_thread::yield();
				old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			}
			return true;
		}

		private: template <typename TimePointT__> auto trySpinningLockUntil_(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (spinnable && timed) {
			if constexpr (recursive) {
				return this->tryRecursiveSpinningLockUntil_(deadline_);
			} else if constexpr (shareable) {
				if constexpr (preferExclusive.isDeterminateTrue()) {
					StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					StatusCount_ nextValue_;
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
							this->m_underlying_.compareExchangeWeak(
								old_, nextValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					);
					while (nextValue_ != exclusive_) {
						if (TimePointT__::clock::now() > deadline_) {
							StatusCount_ previousCount_ = this->m_underlying_.fetchSubtract(exclusive_, ::std::memory_order::relaxed);
							this->m_underlying_.notifyAll();
							DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
							return false;
						}
						DCOOL_CORE_ASSERT(nextValue_ > exclusive_);
						::std::this_thread::yield();
						nextValue_ = this->m_underlying_.load(::std::memory_order::acquire);
					}
				} else {
					StatusCount_ expected_ = 0;
					while (
						!(
							this->m_underlying_.compareExchangeStrong(
								expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					) {
						if (TimePointT__::clock::now() > deadline_) {
							return false;
						}
						::std::this_thread::yield();
						while (this->m_underlying_.load(::std::memory_order::relaxed) > 0) {
							if (TimePointT__::clock::now() > deadline_) {
								return false;
							}
							::std::this_thread::yield();
						}
					}
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
			return true;
		}

		public: template <typename TimePointT__> auto tryLockUntil(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed) {
			if constexpr (spinnable) {
				return this->trySpinningLockUntil_(deadline_);
			} else {
				if constexpr (flexibleRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if constexpr (recursive) {
						::std::thread::id myId_ = ::std::this_thread::get_id();
						if (this->m_underlying_.exclusiveOwner_.value == myId_) {
							if (this->m_underlying_.statusCount_ == maxRecursion) {
								return false;
							}
							++(this->m_underlying_.statusCount_);
						} else if constexpr (preferExclusive.isDeterminateTrue()) {
							::std::cv_status lastWaitResult_ = ::std::cv_status::no_timeout;
							for (; ; ) {
								if (this->m_underlying_.exclusiveOwner_.value == ::dcool::concurrency::nullThreadId) {
									this->m_underlying_.exclusiveOwner_.value = myId_;
									if (
										!(
											this->m_underlying_.blocker_.wait_until(
												locker_,
												deadline_,
												[this]() {
													DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ < maxShare);
													return this->m_underlying_.statusCount_ == 0;
												}
											)
										)
									) {
										this->m_underlying_.exclusiveOwner_.value = ::dcool::concurrency::nullThreadId;
										return false;
									}
									break;
								}
								if (lastWaitResult_ == ::std::cv_status::timeout) {
									return false;
								}
								lastWaitResult_ = this->m_underlying_.blocker_.wait_until(locker_, deadline_);
							}
						} else {
							if (
								!(
									this->m_underlying_.blocker_.wait_until(
										locker_,
										deadline_,
										[this]() {
											if (this->m_underlying_.exclusiveOwner_.value != ::dcool::concurrency::nullThreadId) {
												return false;
											}
											if constexpr (shareable) {
												return this->m_underlying_.statusCount_ == 0;
											}
											DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ == 0);
											return true;
										}
									)
								)
							) {
								return false;
							}
							this->m_underlying_.exclusiveOwner_.value = myId_;
						}
					} else {
						if constexpr (preferExclusive.isDeterminateTrue()) {
							if (
								!(
									this->m_underlying_.blocker_.wait_until(
										locker_,
										deadline_,
										[this]() {
											return this->m_underlying_.statusCount_ < exclusive_;
										}
									)
								)
							) {
								return false;
							}
							this->m_underlying_.statusCount_ += exclusive_;
							if (
								!(
									this->m_underlying_.blocker_.wait_until(
										locker_,
										deadline_,
										[this]() {
											DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
											return this->m_underlying_.statusCount_ == exclusive_;
										}
									)
								)
							) {
								DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ > exclusive_);
								this->m_underlying_.statusCount_ -= exclusive_;
								StatusCount_ countAfter_ = this->m_underlying_.statusCount_;
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
											return this->m_underlying_.statusCount_ == 0;
										}
									)
								)
							) {
								return false;
							}
							this->m_underlying_.statusCount_ = exclusive_;
						}
					}
				} else {
					return this->m_underlying_.try_lock_until(deadline_);
				}
			}
			return true;
		}

		public: template <typename DurationT_> auto tryLockFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockUntil(::std::chrono::steady_clock::now() + duration_);
		}

		private: template <typename TimePointT__> auto tryRecursiveSpinningLockSharedUntil_(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (spinnable && timed && recursive && shareable) {
			while (!(this->tryLockShared())) {
				RecursiveSpinnableSharedAtomUnderlying_ old_;
				for (; ; ) {
					if (TimePointT__::clock::now() > deadline_) {
						return false;
					}
					::std::this_thread::yield();
					old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					if (old_.exclusiveOwner_ == ::dcool::concurrency::nullThreadId) {
						DCOOL_CORE_ASSERT(old_.statusCount_ <= maxShare);
						if (old_.statusCount_ < maxShare) {
							break;
						}
					}
				}
			}
			return true;
		}

		private: template <typename TimePointT__> auto trySpinningLockSharedUntil_(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (spinnable &&timed && shareable) {
			if constexpr (recursive) {
				return this->tryRecursiveSpinningLockSharedUntil_(deadline_);
			} else {
				while (!(this->tryLockShared())) {
					do {
						if (TimePointT__::clock::now() > deadline_) {
							return false;
						}
						::std::this_thread::yield();
					} while (this->m_underlying_.load(::std::memory_order::relaxed) >= exclusive_);
				}
			}
			return true;
		}

		public: template <typename TimePointT__> auto tryLockSharedUntil(
			TimePointT__ const& deadline_
		)& noexcept -> ::dcool::core::Boolean requires (timed && shareable) {
			if constexpr (spinnable) {
				return this->trySpinningLockSharedUntil_(deadline_);
			} else {
				if constexpr (flexibleRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									::dcool::core::Boolean result_;
									if constexpr (recursive) {
										if (this->m_underlying_.exclusiveOwner_.value != ::dcool::concurrency::nullThreadId) {
											return false;
										}
										if (this->m_underlying_.statusCount_ >= maxShare) {
											return false;
										}
									} else {
										if (this->m_underlying_.statusCount_ >= exclusive_) {
											return false;
										}
									}
									return true;
								}
							)
						)
					) {
						return false;
					}
					++(this->m_underlying_.statusCount_);
				} else {
					return this->m_underlying_.try_lock_shared_until(deadline_);
				}
			}
			return true;
		}

		public: template <typename DurationT_> auto tryLockSharedFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed) {
			return this->tryLockSharedUntil(::std::chrono::steady_clock::now() + duration_);
		}

		private: void recursiveSpinningLock_()& noexcept requires (spinnable && recursive) {
			RecursiveSpinnableSharedAtomUnderlying_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			RecursiveSpinnableSharedAtomUnderlying_ new_ = {
				.exclusiveOwner_ = ::std::this_thread::get_id()
			};
			if (old_.exclusiveOwner_ == new_.exclusiveOwner_) {
				DCOOL_CORE_ASSERT(old_.statusCount_ <= maxRecursion);
				if (old_.statusCount_ == maxRecursion) {
					throw ::dcool::concurrency::ExcessRecursiveLockDepth("Excessing resursive lock depth limit.");
				}
				new_.statusCount_ = old_.statusCount_ + 1;
				this->m_underlying_.store(new_, ::std::memory_order::relaxed);
			} else {
				new_.statusCount_ = 0;
				for (; ; ) {
					while (old_.exclusiveOwner_ == ::dcool::concurrency::nullThreadId && old_.statusCount_ == 0) {
						if (
							this->m_underlying_.compare_exchange_weak(old_, new_, ::std::memory_order::acquire, ::std::memory_order::relaxed)
						) {
							return;
						}
					}
					DCOOL_CORE_ASSERT(old_.exclusiveOwner_ != new_.exclusiveOwner_);
					::std::this_thread::yield();
				}
			}
		}

		private: void spinningLock_()& noexcept requires (spinnable) {
			if constexpr (recursive) {
				this->recursiveSpinningLock_();
			} if constexpr (shareable) {
				if constexpr (preferExclusive.isDeterminateTrue()) {
					StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					StatusCount_ newValue_;
					do {
						while (old_ >= exclusive_) {
							::std::this_thread::yield();
							old_ = this->m_underlying_.waitNew(old_, ::std::memory_order::relaxed);
						}
						newValue_ = old_ + exclusive_;
					} while (
						!(
							this->m_underlying_.compareExchangeWeak(
								old_, newValue_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						)
					);
					while (newValue_ != exclusive_) {
						DCOOL_CORE_ASSERT(newValue_ > exclusive_);
						::std::this_thread::yield();
						newValue_ = this->m_underlying_.waitNew(newValue_, ::std::memory_order::acquire);
					}
				} else {
					StatusCount_ expected_ = 0;
					for (; ; ) {
						if (
							this->m_underlying_.compareExchangeStrong(
								expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
							)
						) {
							break;
						}
						do {
							::std::this_thread::yield();
							expected_ = this->m_underlying_.waitNew(expected_, ::std::memory_order::relaxed);
						} while (expected_ != 0);
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
		}

		// All the following member needs `recursive` support.
		public: void lock()& noexcept(spinnable) {
			if constexpr (spinnable) {
				this->spinningLock_();
			} else {
				if constexpr (flexibleRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					if constexpr (preferExclusive.isDeterminateTrue()) {
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								return this->m_underlying_.statusCount_ < exclusive_;
							}
						);
						this->m_underlying_.statusCount_ += exclusive_;
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
								return this->m_underlying_.statusCount_ == exclusive_;
							}
						);
					} else {
						this->m_underlying_.blocker_.wait(
							locker_,
							[this]() {
								return this->m_underlying_.statusCount_ == 0;
							}
						);
						this->m_underlying_.statusCount_ = exclusive_;
					}
				} else {
					this->m_underlying_.lock();
				}
			}
		}

		private: void spinningLockShared_()& noexcept requires (spinnable) {
			for (; ; ) {
				StatusCount_ previous_ = this->m_underlying_.fetchTransformOrLoad(
					[](StatusCount_ old_) noexcept -> ::dcool::core::Optional<StatusCount_> {
						if (old_ >= exclusive_) {
							return ::dcool::core::nullOptional;
						}
						return old_ + 1;
					},
					::std::memory_order::acquire,
					::std::memory_order::relaxed
				);
				if (previous_ < exclusive_) {
					break;
				}
				::std::this_thread::yield();
				this->m_underlying_.wait(previous_, ::std::memory_order::relaxed);
			}
		}

		public: void lockShared()& noexcept(spinnable) requires (shareable) {
			if constexpr (spinnable) {
				this->spinningLockShared_();
			} else {
				if constexpr (flexibleRequired_) {
					::std::unique_lock locker_(this->m_underlying_.mutex_);
					this->m_underlying_.shareBlocker_.wait(
						locker_,
						[this]() {
							return this->m_underlying_.statusCount_ < exclusive_;
						}
					);
					++(this->m_underlying_.statusCount_);
				} else {
					this->m_underlying_.lock_shared();
				}
			}
		}

		public: void unlock()& noexcept {
			if constexpr (spinnable) {
				if constexpr (shareable) {
#	if DCOOL_DEBUG_ENABLED
					StatusCount_ previousCount_ = this->m_underlying_.exchange(0, ::std::memory_order::release);
					DCOOL_CORE_ASSERT(previousCount_ == exclusive_);
#	else
					this->m_underlying_.store(0, ::std::memory_order::release)
#	endif
					this->m_underlying_.notifyAll();
				} else {
					this->m_underlying_.clear(::std::memory_order::release);
				}
			} else {
				if constexpr (flexibleRequired_) {
					{
						::std::lock_guard guard_(this->m_underlying_.mutex_);
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ == exclusive_);
						this->m_underlying_.statusCount_ = 0;
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
				StatusCount_ previousCount_ = this->m_underlying_.fetchSubtract(1, ::std::memory_order::release);
				if (previousCount_ < exclusive_) {
					this->m_underlying_.notifyOne();
				}
				DCOOL_CORE_ASSERT(previousCount_ > 0);
			} else {
				if constexpr (flexibleRequired_) {
					StatusCount_ countAfterUnlock_;
					{
						::std::lock_guard guard_(this->m_underlying_.mutex_);
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ > 0);
						countAfterUnlock_ = --(this->m_underlying_.statusCount_);
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
				StatusCount_ expected_ = 1;
				if (
					!(
						this->m_underlying_.compareExchangeStrong(
							expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					)
				) {
					return false;
				}
				DCOOL_CORE_ASSERT(expected_ >= 1);
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_, ::std::try_to_lock);
				if ((!(locker_.owns_lock())) || this->m_underlying_.statusCount_ != 1) {
					DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
					return false;
				}
				this->m_underlying_.statusCount_ = exclusive_;
			}
			return true;
		}

		public: template <typename TimePointT__> auto tryUpgradeUntil(
			TimePointT__ const& deadline_
		)& noexcept(spinnable) -> ::dcool::core::Boolean requires (timed && upgradeable) {
			if constexpr (spinnable) {
				if constexpr (preferExclusive.isDeterminateTrue()) {
					StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
					StatusCount_ newValue_;
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
							StatusCount_ previousCount_ = this->m_underlying_.fetchSubtract(exclusive_ - 1, ::std::memory_order::relaxed);
							this->m_underlying_.notifyAll();
							DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
							return false;
						}
						DCOOL_CORE_ASSERT(newValue_ > exclusive_);
						::std::this_thread::yield();
						newValue_ = this->m_underlying_.load(::std::memory_order::acquire);
					}
				} else {
					for (; ; ) {
						StatusCount_ expected_ = 1;
						if (
							this->m_underlying_.compare_exchange_strong(
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
									DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
									// Another thread is locking. This would be a deadlock.
									DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ <= exclusive_);
									return this->m_underlying_.statusCount_ < exclusive_;
								}
							)
						)
					) {
						return false;
					}
					this->m_underlying_.statusCount_ += (exclusive_ - 1);
					if (
						!(
							this->m_underlying_.blocker_.wait_until(
								locker_,
								deadline_,
								[this]() {
									DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
									return this->m_underlying_.statusCount_ == exclusive_;
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
									DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
									return this->m_underlying_.statusCount_ == 1;
								}
							)
						)
					) {
						return false;
					}
					this->m_underlying_.statusCount_ = exclusive_;
				}
			}
			return true;
		}

		public: template <typename DurationT_> auto tryUpgradeFor(
			DurationT_ const& duration_
		) noexcept -> ::dcool::core::Boolean requires (timed && upgradeable) {
			return this->tryUpgradeUntil(::std::chrono::steady_clock::now() + duration_);
		}

		private: void spinningUpgrade_()& noexcept requires (spinnable && upgradeable) {
			if constexpr (preferExclusive.isDeterminateTrue()) {
				StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
				StatusCount_ newValue_;
				do {
					DCOOL_CORE_ASSERT(old_ <= exclusive_); // Another thread is locking. This would be a deadlock.
					while (old_ >= exclusive_) {
						::std::this_thread::yield();
						old_ = this->m_underlying_.waitNew(old_, ::std::memory_order::relaxed);
					}
					DCOOL_CORE_ASSERT(old_ >= 1);
					newValue_ = old_ + (exclusive_ - 1);
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
					newValue_ = this->m_underlying_.waitNew(newValue_, ::std::memory_order::acquire);
				}
			} else {
				for (; ; ) {
					StatusCount_ expected_ = 1;
					if (
						this->m_underlying_.compare_exchange_strong(
							expected_, exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
						)
					) {
						break;
					}
					do {
						DCOOL_CORE_ASSERT(expected_ > 1);
						::std::this_thread::yield();
						expected_ = this->m_underlying_.waitNew(expected_, ::std::memory_order::relaxed);
					} while (expected_ != 1);
				}
			}
		}

		public: void upgrade()& noexcept(spinnable) requires (upgradeable) {
			if constexpr (spinnable) {
				this->spinningUpgrade_();
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				if constexpr (preferExclusive.isDeterminateTrue()) {
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
							// Another thread is locking. This would be a deadlock.
							DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ <= exclusive_);
							return this->m_underlying_.statusCount_ < exclusive_;
						}
					);
					this->m_underlying_.statusCount_ += (exclusive_ - 1);
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
							return this->m_underlying_.statusCount_ == exclusive_;
						}
					);
				} else {
					this->m_underlying_.blocker_.wait(
						locker_,
						[this]() {
							DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
							return this->m_underlying_.statusCount_ == 1;
						}
					);
					this->m_underlying_.statusCount_ = exclusive_;
				}
			}
		}

		public: void downgrade() noexcept requires (downgradeable) {
			if constexpr (spinnable) {
#	if DCOOL_DEBUG_ENABLED
				StatusCount_ previousCount_ = this->m_underlying_.exchange(1, ::std::memory_order::release);
				this->m_underlying_.notifyAll();
				DCOOL_CORE_ASSERT(previousCount_ == exclusive_);
#	else
				this->m_underlying_.store(1, ::std::memory_order::release)
#	endif
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ == exclusive_);
					this->m_underlying_.statusCount_ = 1;
				}
				if constexpr (preferExclusive.isDeterminateTrue()) {
					this->m_underlying_.blocker_.notify_one();
				}
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		public: auto tryOrderLock()& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				StatusCount_ previous_ = this->m_underlying_.fetchTransformOrLoad(
					[](StatusCount_ value_) noexcept -> ::dcool::core::Optional<StatusCount_> {
						if (value_ >= exclusive_) {
							return ::dcool::core::nullOptional;
						}
						return value_ + exclusive_;
					},
					::std::memory_order::acquire,
					::std::memory_order::relaxed
				);
				return previous_ < exclusive_;
			} else {
				::std::lock_guard guard_(this->m_underlying_.mutex_);
				if (this->m_underlying_.statusCount_ >= exclusive_) {
					return false;
				}
				this->m_underlying_.statusCount_ += exclusive_;
			}
			return true;
		};

		public: auto tryCompleteLockOrder()& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			return this->tryCompleteExclusiveOrder_();
		}

		private: void spinningOrderLock_()& noexcept requires (spinnable && preferExclusive.isDeterminateTrue()) {
			StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			do {
				while (old_ >= exclusive_) {
					::std::this_thread::yield();
					old_ = this->m_underlying_.waitNew(old_, ::std::memory_order::relaxed);
				};
			} while (
				!(
					this->m_underlying_.compareExchangeWeak(
						old_, old_ + exclusive_, ::std::memory_order::acquire, ::std::memory_order::relaxed
					)
				)
			);
		}

		public: void orderLock()& noexcept(spinnable) requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				this->spinningOrderLock_();
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						return this->m_underlying_.statusCount_ < exclusive_;
					}
				);
				this->m_underlying_.statusCount_ += exclusive_;
			}
		};

		public: void completeLockOrder()& noexcept requires (preferExclusive.isDeterminateTrue()) {
			this->completeExclusiveOrder_();
		}

		public: void cancelLockOrder() noexcept requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				StatusCount_ previousCount_ = this->m_underlying_.fetchSubtract(exclusive_, ::std::memory_order::relaxed);
				this->m_underlying_.notifyAll();
				DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
					this->m_underlying_.statusCount_ -= exclusive_;
				}
				this->m_underlying_.blocker_.notify_one();
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		public: auto tryOrderUpgrade(
		)& noexcept -> ::dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
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
				if (this->m_underlying_.statusCount_ >= exclusive_) {
					return false;
				}
				DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
				this->m_underlying_.statusCount_ += (exclusive_ - 1);
			}
			return true;
		};

		public: auto tryCompleteUpgradeOrder(
		)& noexcept -> ::dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			return this->tryCompleteExclusiveOrder_();
		}

		private: void spinningOrderUpgrade_()& noexcept requires (spinnable && upgradeable && preferExclusive.isDeterminateTrue()) {
			StatusCount_ old_ = this->m_underlying_.load(::std::memory_order::relaxed);
			do {
				while (old_ >= exclusive_) {
					::std::this_thread::yield();
					old_ = this->m_underlying_.waitNew(old_, ::std::memory_order::relaxed);
				};
				DCOOL_CORE_ASSERT(old_ >= 1);
			} while (
				!(
					this->m_underlying_.compare_exchange_weak(
						old_, old_ + (exclusive_ - 1), ::std::memory_order::acquire, ::std::memory_order::relaxed
					)
				)
			);
		}

		public: void orderUpgrade()& noexcept(spinnable) requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				this->spinningOrderUpgrade_();
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= 1);
						return this->m_underlying_.statusCount_ < exclusive_;
					}
				);
				this->m_underlying_.statusCount_ += (exclusive_ - 1);
			}
		};

		public: void completeUpgradeOrder()& noexcept requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			this->completeExclusiveOrder_();
		}

		public: void cancelUpgradeOrder() noexcept requires (upgradeable && preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				StatusCount_ previousCount_ = this->m_underlying_.fetchSubtract((exclusive_ - 1), ::std::memory_order::relaxed);
				this->m_underlying_.notifyAll();
				DCOOL_CORE_ASSERT(previousCount_ >= exclusive_);
			} else {
				{
					::std::lock_guard guard_(this->m_underlying_.mutex_);
					DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
					this->m_underlying_.statusCount_ -= (exclusive_ - 1);
				}
				this->m_underlying_.blocker_.notify_one();
				this->m_underlying_.shareBlocker_.notify_all();
			}
		}

		private: auto tryCompleteExclusiveOrder_(
		)& noexcept -> ::dcool::core::Boolean requires (preferExclusive.isDeterminateTrue()) {
			StatusCount_ currentStatusCount_;
			if constexpr (spinnable) {
				currentStatusCount_ = this->m_underlying_.load(::std::memory_order::acquire);
			} else {
				::std::lock_guard guard_(this->m_underlying_.mutex_);
				currentStatusCount_ = this->m_underlying_.statusCount_;
			}
			DCOOL_CORE_ASSERT(currentStatusCount_ >= exclusive_);
			return currentStatusCount_ == exclusive_;
		}

		private: void completeExclusiveOrder_()& noexcept requires (preferExclusive.isDeterminateTrue()) {
			if constexpr (spinnable) {
				this->m_underlying_.waitPredicate(
					[](StatusCount_ value_) noexcept -> ::dcool::core::Boolean {
						DCOOL_CORE_ASSERT(value_ >= exclusive_);
						return value_ == exclusive_;
					},
					::std::memory_order::acquire
				);
			} else {
				::std::unique_lock locker_(this->m_underlying_.mutex_);
				this->m_underlying_.blocker_.wait(
					locker_,
					[this]() {
						DCOOL_CORE_ASSERT(this->m_underlying_.statusCount_ >= exclusive_);
						return this->m_underlying_.statusCount_ == exclusive_;
					}
				);
			}
		}

		// For standard compatibility
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
