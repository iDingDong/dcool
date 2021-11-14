#ifndef DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_ 1

#	include <dcool/concurrency/atom_basic.hpp>

#	include <dcool/core.hpp>

#	include <atomic>
#	include <condition_variable>
#	include <mutex>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueAtomicForAtom_, extractedAtomicForAtomValue_, atomic
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueStampWidthForAtom_, extractedStampWidthForAtomValue_, stampWidth
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueWaitableForAtom_, extractedWaitableForAtomValue_, waitable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueLockFullForAtom_, extractedLockFullForAtomValue_, lockFull
)

namespace dcool::concurrency {
	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class AtomConfigAdaptor_ {
			private: using Self_ = AtomConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;
			static_assert(::dcool::core::TriviallyCopyable<Value>);

			public: static constexpr ::dcool::core::Triboolean atomic =
				::dcool::concurrency::detail_::extractedAtomicForAtomValue_<Config>(::dcool::core::determinateTrue)
			;
			public: static constexpr ::dcool::core::Length stampWidth =
				::dcool::concurrency::detail_::extractedStampWidthForAtomValue_<Config>(::dcool::core::UnsignedMaxInteger(0))
			;
			public: static constexpr ::dcool::core::Boolean waitable =
				(!(atomic.isDeterminateFalse())) && ::dcool::concurrency::detail_::extractedWaitableForAtomValue_<Config>(true)
			;
			public: static constexpr ::dcool::core::Boolean lockFull =
				(!(atomic.isDeterminateFalse())) && ::dcool::concurrency::detail_::extractedLockFullForAtomValue_<Config>(false)
			;
		};
	}

	template <typename T_, typename ValueT_> concept AtomConfig = requires {
		typename ::dcool::concurrency::detail_::AtomConfigAdaptor_<T_, ValueT_>;
	};

	template <typename ConfigT_, typename ValueT_> requires ::dcool::concurrency::AtomConfig<
		ConfigT_, ValueT_
	> using AtomConfigAdaptor = ::dcool::concurrency::detail_::AtomConfigAdaptor_<ConfigT_, ValueT_>;

	namespace detail_ {
		enum class AtomUnderlyingCategory_ {
			atomic_,
			atomicReadyPlain_,
			plain_
		};

		template <
			::dcool::concurrency::detail_::AtomUnderlyingCategory_ categoryC_, typename UnderlyingValueT_
		> struct AtomUnderlying_ {
			static constexpr ::dcool::core::Triboolean lockFree =
				(
					categoryC_ == ::dcool::concurrency::detail_::AtomUnderlyingCategory_::plain_ ||
					::dcool::concurrency::lockFreeInStandardAtomic<UnderlyingValueT_>
				)
			;

			using Underlying_ = ::dcool::core::ConditionalType<
				categoryC_ == ::dcool::concurrency::detail_::AtomUnderlyingCategory_::plain_,
				UnderlyingValueT_,
				::std::atomic<UnderlyingValueT_>
			>;

			Underlying_ underlying_;
		};

		template <typename UnderlyingValueT_> struct AtomUnderlying_<
			::dcool::concurrency::detail_::AtomUnderlyingCategory_::atomicReadyPlain_, UnderlyingValueT_
		> {
			static constexpr ::dcool::core::Triboolean lockFree = ::dcool::concurrency::lockFreeInStandardAtomicRef<
				UnderlyingValueT_
			>;

			alignas(::dcool::concurrency::requiredAtomAlignment<UnderlyingValueT_>) mutable UnderlyingValueT_ underlying_;
		};

		template <typename ValueT_, ::dcool::core::Length stampWidthC_> struct StampedValue_ {
			private: using Self_ = StampedValue_<ValueT_, stampWidthC_>;
			public: using Value = ValueT_;
			public: static constexpr ::dcool::core::Length stampWidth = stampWidthC_;

			public: using Stamp = ::dcool::core::UnsignedInteger<stampWidth>;

			public: Value value;
			public: [[no_unique_address]] Stamp stamp;

			friend constexpr auto intelliHasEqualValueRepresentation(Self_ const& left_, Self_ const& right_) {
				//::dcool::core::terminate();
				return left_.stamp == right_.stamp && ::dcool::core::intelliHasEqualValueRepresentation(left_.value, right_.value);
			}
		};
	}

	template <typename ValueT_, ::dcool::concurrency::AtomConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct Atom {
		private: using Self_ = Atom<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::AtomConfigAdaptor<Config, Value>;
		public: static constexpr ::dcool::core::Triboolean atomic = ConfigAdaptor_::atomic;
		public: static constexpr ::dcool::core::Length stampWidth = ConfigAdaptor_::stampWidth;
		public: static constexpr ::dcool::core::Boolean waitable = ConfigAdaptor_::waitable;
		public: static constexpr ::dcool::core::Boolean lockFull = ConfigAdaptor_::lockFull;
		public: static constexpr ::dcool::core::Boolean stamped = stampWidth > 0;
		private: static constexpr ::dcool::core::Boolean usingStandardAtomic_ = (atomic.isDeterminateTrue() && (!lockFull));
		private: static constexpr ::dcool::core::Boolean usingStandardAtomicRef_ = !(atomic.isDeterminate() || lockFull);
		private: static constexpr ::dcool::core::Boolean usingAtomicBasicOperation_ =
			(usingStandardAtomic_ || usingStandardAtomicRef_)
		;
		private: static constexpr ::dcool::core::Boolean supportAtomicOperation_ = !(atomic.isDeterminateFalse());
		private: static constexpr ::dcool::concurrency::detail_::AtomUnderlyingCategory_ underlyingCategory_ =
			usingStandardAtomic_ ?
				::dcool::concurrency::detail_::AtomUnderlyingCategory_::atomic_ :
				usingStandardAtomicRef_ ?
					::dcool::concurrency::detail_::AtomUnderlyingCategory_::atomicReadyPlain_ :
					::dcool::concurrency::detail_::AtomUnderlyingCategory_::plain_
		;

		public: using StampedValue = ::dcool::concurrency::detail_::StampedValue_<Value, stampWidth>;
		public: using Stamp = StampedValue::Stamp;
		public: using UnderlyingValue = ::dcool::core::ConditionalType<stamped, StampedValue, Value>;
		private: using UnderlyingHolder_ = ::dcool::concurrency::detail_::AtomUnderlying_<underlyingCategory_, UnderlyingValue>;
		public: using Mutex = ::std::mutex;

		public: static constexpr ::dcool::core::Triboolean lockFree = (!lockFull) && UnderlyingHolder_::lockFree;

		// For standard compatibility
		public: static constexpr ::dcool::core::Boolean is_always_lock_free = lockFree.isDeterminateTrue();

		private: struct Locker_ {
			mutable Mutex mutex_;
			[[no_unique_address]] mutable ::dcool::core::ConditionalType<
				waitable, ::std::condition_variable, ::dcool::core::Empty<>
			> blocker_;
		};

		private: struct PhoneyLocker_ {
			[[no_unique_address]] ::dcool::core::Empty<> mutex_;
		};

		private: UnderlyingHolder_ m_holder_;
		private: [[no_unique_address]] ::dcool::core::ConditionalType<lockFull, Locker_, PhoneyLocker_> m_locker_;

		public: constexpr Atom() noexcept: Atom(Value()) {
		}

		public: Atom(Self_ const&) = delete;
		public: Atom(Self_&&) = delete;

		public: constexpr Atom(Value const& desired_) noexcept: m_holder_ {
			.underlying_ = toUnderlying_(desired_)
		} {
		}

		public: auto operator =(Self_ const&)& -> Self_& = delete;

		public: constexpr auto operator =(Value const& desired_)& noexcept -> Self_& {
			this->store(desired_);
			return *this;
		}

		private: static constexpr auto toUnderlying_(Value const& value_) noexcept -> UnderlyingValue {
			UnderlyingValue result_;
			if constexpr (stamped) {
				result_.value = value_;
				result_.stamp = Stamp(0);
			} else {
				result_ = value_;
			}
			return result_;
		}

		public: constexpr auto lockFreeOnExecution() const noexcept -> ::dcool::core::Boolean {
			if constexpr (lockFree.isDeterminateTrue()) {
				return true;
			}
			if constexpr (lockFree.isDeterminateFalse()) {
				return false;
			}
			return ::dcool::concurrency::lockFreeAtomicityOnExecution(this->m_holder_.underlying_);
		}

		public: constexpr auto mutex() const noexcept -> Mutex& requires (lockFull) {
			return this->m_locker_.mutex_;
		}

		// Underlying operations
		private: constexpr auto atomicallyLoadUnderlying_(
			::std::memory_order order_
		) const noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyLoad(this->m_holder_.underlying_, order_);
			}
			::dcool::core::PhoneySharedLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedLoadUnderlying_();
		}

		private: constexpr auto sequencedLoadUnderlying_() const noexcept -> UnderlyingValue {
			if constexpr (usingStandardAtomic_) {
				return this->atomicallyLoadUnderlying_(::std::memory_order::relaxed);
			}
			return this->m_holder_.underlying_;
		}

		private: constexpr void atomicallyStoreUnderlying_(
			UnderlyingValue const& desired_, ::std::memory_order order_
		)& noexcept requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				::dcool::concurrency::atomicallyStore(this->m_holder_.underlying_, desired_, order_);
			} else {
				::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
				this->sequencedStoreUnderlying_(desired_);
			}
		}

		private: constexpr void sequencedStoreUnderlying_(UnderlyingValue const& desired_)& noexcept {
			if constexpr (usingStandardAtomic_) {
				this->atomicallyStoreUnderlying_(desired_, ::std::memory_order::relaxed);
			} else {
				this->m_holder_.underlying_ = desired_;
			}
		}

		private: constexpr auto atomicallyExchangeUnderlying_(
			UnderlyingValue const& desired_, ::std::memory_order order_
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyExchange(this->m_holder_.underlying_, desired_, order_);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedExchangeUnderlying_(desired_);
		}

		private: constexpr auto sequencedExchangeUnderlying_(UnderlyingValue const& desired_) noexcept -> UnderlyingValue {
			if constexpr (usingStandardAtomic_) {
				return this->atomicallyExchangeUnderlying_(desired_, ::std::memory_order::relaxed);
			}
			UnderlyingValue result_ = this->m_holder_.underlying_;
			this->m_holder_.underlying_ = desired_;
			return result_;
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingWeak_(
			UnderlyingValue& expected_,
			UnderlyingValue const& desired_,
			::std::memory_order successOrder_,
			::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyCompareExchangeWeak(
					this->m_holder_.underlying_, expected_, desired_, successOrder_, failureOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedCompareExchangeUnderlyingWeak_(expected_, desired_);
		}

		private: constexpr auto sequencedCompareExchangeUnderlyingWeak_(
			UnderlyingValue& expected_, UnderlyingValue const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (usingStandardAtomic_) {
				result_ = ::dcool::concurrency::atomicallyCompareExchangeWeak(
					this->m_holder_.underlying_, expected_, desired_, ::std::memory_order::relaxed
				);
			} else {
				// Non-atomic operations will never fail spuriously.
				result_ = this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
			}
			return result_;
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingStrong_(
			UnderlyingValue& expected_,
			UnderlyingValue const& desired_,
			::std::memory_order successOrder_,
			::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyCompareExchangeStrong(
					this->m_holder_.underlying_, expected_, desired_, successOrder_, failureOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
		}

		private: constexpr auto sequencedCompareExchangeUnderlyingStrong_(
			UnderlyingValue& expected_, UnderlyingValue const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (usingStandardAtomic_) {
				result_ = ::dcool::concurrency::atomicallyCompareExchangeStrong(
					this->m_holder_.underlying_, expected_, desired_, ::std::memory_order::relaxed
				);
			} else {
				if (!(::dcool::core::intelliHasEqualValueRepresentation(this->m_holder_.underlying_, expected_))) {
					expected_ = this->m_holder_.underlying_;
					return false;
				}
				this->m_holder_.underlying_ = desired_;
				result_ = true;
			}
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyFetchTransform(
					this->m_holder_.underlying_, ::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
		}

		private: template <typename TransformerT__> constexpr auto sequencedFetchTransformUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue {
			if constexpr (usingStandardAtomic_) {
				return this->atomicallyFetchTransformUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), ::std::memory_order::relaxed, ::std::memory_order::relaxed
				);
			}
			UnderlyingValue result_ = this->m_holder_.underlying_;
			this->m_holder_.underlying_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoadUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyFetchTransformOrLoad(
					this->m_holder_.underlying_, ::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedFetchTransformOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
		}

		private: template <typename TransformerT__> constexpr auto sequencedFetchTransformOrLoadUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue {
			UnderlyingValue result_ = this->sequencedLoadUnderlying_();
			auto new_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
			if (new_.valid()) {
				this->sequencedStoreUnderlying_(new_.access());
			}
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyTransformFetchUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyTransformFetch(
					this->m_holder_.underlying_, ::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedTransformFetchUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
		}

		private: template <typename TransformerT__> constexpr auto sequencedTransformFetchUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue {
			if constexpr (usingStandardAtomic_) {
				return this->atomicallyTransformFetchUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), ::std::memory_order::relaxed, ::std::memory_order::relaxed
				);
			}
			UnderlyingValue result_ = this->m_holder_.underlying_;
			this->m_holder_.underlying_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyTransformFetchOrLoadUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyTransformFetchOrLoad(
					this->m_holder_.underlying_, ::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
			return this->sequencedTransformFetchOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
		}

		private: template <typename TransformerT__> constexpr auto sequencedTransformFetchOrLoadUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue {
			UnderlyingValue result_ = this->sequencedLoadUnderlying_();
			auto new_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
			if (new_.valid()) {
				this->sequencedStoreUnderlying_(new_.access());
			}
			return result_;
		}

		private: constexpr void atomicallyWaitUnderlying_(
			UnderlyingValue const& old_, ::std::memory_order order_
		) const noexcept requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				::dcool::concurrency::atomicallyWait(this->m_holder_.underlying_, old_, order_);
			} else {
				this->atomicallyWaitFetchWeakUnderlying_(old_, order_);
			}
		}

		private: template <typename TimePointT__> constexpr auto atomicallyWaitUnderlyingUntil_(
			UnderlyingValue const& old_, TimePointT__ const& deadline_, ::std::memory_order order_
		) const noexcept -> ::dcool::core::Boolean requires (waitable) {
			::dcool::core::Boolean result_;
			if constexpr (usingAtomicBasicOperation_) {
				result_ = ::dcool::concurrency::atomicallyWaitUntil(this->m_holder_.underlying_, old_, deadline_, order_);
			} else {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				result_ = this->m_locker_.blocker_.wait_until(
					locker_,
					deadline_,
					[this, &old_]() noexcept -> ::dcool::core::Boolean {
						return !(::dcool::core::intelliHasEqualValueRepresentation(this->sequencedLoadUnderlying_(), old_));
					}
				);
			}
			return result_;
		}

		private: constexpr auto atomicallyWaitFetchWeakUnderlying_(
			UnderlyingValue const& old_, ::std::memory_order order_
		) const noexcept -> UnderlyingValue requires (waitable) {
			UnderlyingValue result_;
			if constexpr (usingAtomicBasicOperation_) {
				result_ = ::dcool::concurrency::atomicallyWaitFetchWeak(this->m_holder_.underlying_, old_, order_);
			} else {
				// Lockfull wait fetch will not wake spuriously.
				result_ = this->atomicallyWaitFetchStrongUnderlying_(old_, order_);
			}
			return result_;
		}

		private: constexpr auto atomicallyWaitFetchStrongUnderlying_(
			UnderlyingValue const& old_, ::std::memory_order order_
		) const noexcept -> UnderlyingValue requires (waitable) {
			UnderlyingValue result_;
			if constexpr (usingAtomicBasicOperation_) {
				result_ = ::dcool::concurrency::atomicallyWaitFetchStrong(this->m_holder_.underlying_, old_, order_);
			} else {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				this->m_locker_.blocker_.wait(
					locker_,
					[this, &old_]() noexcept -> ::dcool::core::Boolean {
						return !(::dcool::core::intelliHasEqualValueRepresentation(this->sequencedLoadUnderlying_(), old_));
					}
				);
				result_ = this->sequencedLoadUnderlying_();
			}
			return result_;
		}

		private: template <typename PredicateT_> constexpr auto atomicallyWaitPredicateFetchUnderlying_(
			PredicateT_&& predicate_, ::std::memory_order order_
		) const noexcept -> UnderlyingValue requires (waitable) {
			UnderlyingValue result_;
			if constexpr (usingAtomicBasicOperation_) {
				result_ = ::dcool::concurrency::atomicallyWaitPredicateFetch(
					this->m_holder_.underlying_, ::dcool::core::forward<PredicateT_>(predicate_), order_
				);
			} else {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				this->m_locker_.blocker_.wait(
					locker_,
					[this, &predicate_]() noexcept -> ::dcool::core::Boolean {
						return ::dcool::core::invoke(predicate_, this->sequencedLoadUnderlying_());
					}
				);
				result_ = this->sequencedLoadUnderlying_();
			}
			return result_;
		}

		private: constexpr void atomicallyWaitEqualityUnderlying_(
			UnderlyingValue const& expected_, ::std::memory_order order_
		) const noexcept requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				::dcool::concurrency::atomicallyWaitEquality(this->m_holder_.underlying_, expected_, order_);
			} else {
				this->atomicallyWaitPredicateFetchUnderlying_(
					[&expected_](UnderlyingValue const& value_) noexcept -> ::dcool::core::Boolean {
						return ::dcool::core::intelliHasEqualValueRepresentation(value_, expected_);
					},
					order_
				);
			}
		}

		private: template <typename TaskT__> constexpr auto atomicallyHintedFetchExecuteUnderlying_(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyHintedFetchExecute(
					this->m_holder_.underlying_, hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			}
			return this->atomicallyFetchExecuteUnderlying_(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		private: template <typename TaskT__> constexpr auto atomicallyFetchExecuteUnderlying_(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (waitable) {
			UnderlyingValue old_;
			if constexpr (usingAtomicBasicOperation_) {
				old_ = ::dcool::concurrency::atomicallyFetchExecute(
					this->m_holder_.underlying_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			} else {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				for (; ; ) {
					old_ = this->sequencedLoadUnderlying_();
					auto taskResult_ = ::dcool::core::invoke(task_, ::dcool::core::constantize(old_));
					if (taskResult_.aborted()) {
						break;
					}
					if (taskResult_.done()) {
						this->sequencedStoreUnderlying_(taskResult_.accessValue());
						break;
					}
					if (taskResult_.delayedRetryRequested()) {
						do {
							this->m_blocker_.wait(locker_);
						} while (::dcool::core::intelliHasEqualValueRepresentation(this->sequencedLoadUnderlying_(), old_));
					} else {
						locker_.unlock();
						::std::this_thread::yield();
						locker_.lock();
					}
				}
			}
			return old_;
		}

		private: template <typename TaskT__> constexpr auto atomicallyHintedExecuteFetchUnderlying_(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				return ::dcool::concurrency::atomicallyHintedExecuteFetch(
					this->m_holder_.underlying_, hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			}
			return this->atomicallyExecuteFetchUnderlying_(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		private: template <typename TaskT__> constexpr auto atomicallyExecuteFetchUnderlying_(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue requires (waitable) {
			UnderlyingValue old_;
			if constexpr (usingAtomicBasicOperation_) {
				old_ = ::dcool::concurrency::atomicallyExecuteFetch(
					this->m_holder_.underlying_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			} else {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				for (; ; ) {
					old_ = this->sequencedLoadUnderlying_();
					auto taskResult_ = ::dcool::core::invoke(task_, ::dcool::core::constantize(old_));
					if (taskResult_.aborted()) {
						break;
					}
					if (taskResult_.done()) {
						this->sequencedStoreUnderlying_(taskResult_.accessValue());
						return taskResult_.accessValue();
					}
					if (taskResult_.delayedRetryRequested()) {
						do {
							this->m_blocker_.wait(locker_);
						} while (::dcool::core::intelliHasEqualValueRepresentation(this->sequencedLoadUnderlying_(), old_));
					} else {
						locker_.unlock();
						::std::this_thread::yield();
						locker_.lock();
					}
				}
			}
			return old_;
		}

		// Load operations
		public: constexpr auto atomicallyLoadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyLoadUnderlying_(order_);
		}

		public: constexpr auto sequencedLoadStamped() const noexcept -> StampedValue requires (stamped) {
			return this->sequencedLoadUnderlying_();
		}

		public: constexpr auto loadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyLoadStamped(order_);
			}
			return this->sequencedLoadStamped();
		}

		public: constexpr auto atomicallyLoad(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyLoadStamped(order_).value;
			} else {
				result_ = this->atomicallyLoadUnderlying_(order_);
			}
			return result_;
		}

		public: constexpr auto sequencedLoad() const noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedLoadStamped().value;
			} else {
				return this->sequencedLoadUnderlying_();
			}
			return result_;
		}

		public: constexpr auto load(::std::memory_order order_ = ::std::memory_order::seq_cst) const noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyLoad(order_);
			}
			return this->sequencedLoad();
		}

		public: constexpr operator Value() const noexcept {
			return this->load();
		}

		// Store operations
		public: constexpr void atomicallyStampedStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_ && stamped) {
			static_cast<void>(this->atomicallyStampedExchangeStamped(desired_, order_));
		}

		public: constexpr void sequencedStampedStore(Value const& desired_)& noexcept requires (stamped) {
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = this->sequencedLoadStamped().stamp + 1
				}
			);
		}

		public: constexpr void stampedStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				this->atomicallyStampedStore(desired_, order_);
			} else {
				this->sequencedStampedStore(desired_);
			}
		}

		public: constexpr void atomicallyStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_) {
			if constexpr (stamped) {
				this->atomicallyStampedStore(desired_, order_);
			} else {
				this->atomicallyStoreUnderlying_(desired_, order_);
			}
		}

		public: constexpr void sequencedStore(Value const& desired_)& noexcept {
			if constexpr (stamped) {
				this->sequencedStampedStore(desired_);
			} else {
				this->sequencedStoreUnderlying_(desired_);
			}
		}

		public: constexpr void store(Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst)& noexcept {
			if constexpr (supportAtomicOperation_) {
				this->atomicallyStore(desired_, order_);
			} else {
				this->sequencedStore(desired_);
			}
		}

		// Exchange operations
		public: constexpr auto atomicallyStampedExchangeStamped(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			if constexpr (lockFull) {
				::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
				return this->sequencedStampedExchangeStamped(desired_);
			}
			StampedValue old_ = this->atomicallyLoadStamped(::std::memory_order::relaxed);
			StampedValue new_ {
				.value = desired_,
				.stamp = old_.stamp + 1
			};
			while (!(this->atomicallyCompareExchangeUnderlyingWeak_(old_, new_, order_, ::std::memory_order::relaxed))) {
				new_.stamp = old_.stamp + 1;
			}
			return old_;
		}

		public: constexpr auto sequencedStampedExchangeStamped(
			Value const& desired_
		)& noexcept -> StampedValue requires (stamped) {
			StampedValue old_ = this->sequencedLoadStamped();
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return old_;
		}

		public: constexpr auto stampedExchangeStamped(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedExchangeStamped(desired_, order_);
			}
			return this->sequencedStampedExchangeStamped(desired_);
		}

		public: constexpr auto atomicallyStampedExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedExchangeStamped(desired_, order_).value;
		}

		public: constexpr auto sequencedStampedExchange(Value const& desired_)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedExchangeStamped(desired_).value;
		}

		public: constexpr auto stampedExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedExchange(desired_, order_);
			}
			return this->sequencedStampedExchange(desired_);
		}

		public: constexpr auto atomicallyExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedExchange(desired_, order_);
			} else {
				result_ = this->atomicallyExchangeUnderlying_(desired_, order_);
			}
			return result_;
		}

		public: constexpr auto sequencedExchange(Value const& desired_)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedExchange(desired_);
			} else {
				result_ = this->sequencedExchangeUnderlying_(desired_);
			}
			return result_;
		}

		public: constexpr auto exchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyExchange(desired_, order_);
			}
			return this->sequencedExchange(desired_);
		}

		// Weak CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStampedWeak(
				expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingWeak_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStampedStrong(expected_, desired_);
		}

		public: constexpr auto stampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStampedWeak(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto stampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedCompareExchangeStampedWeak(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedStampedCompareExchangeStampedWeak(expected_, desired_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeWeak(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->atomicallyStampedCompareExchangeStrong(successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStrong(expected_, desired_);
		}

		public: constexpr auto stampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeWeak(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto stampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedStampedCompareExchangeWeak(expected_, desired_);
		}

		public: constexpr auto atomicallyCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return this->atomicallyCompareExchangeWeak(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto atomicallyCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			::dcool::core::Boolean result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->atomicallyCompareExchangeUnderlyingWeak_(expected_, desired_, successOrder_, failureOrder_);
			}
			return result_;
		}

		public: constexpr auto sequencedCompareExchangeWeak(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedCompareExchangeWeak(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingWeak_(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto compareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto compareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedCompareExchangeWeak(expected_, desired_);
		}

		// Strong CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStampedStrong(
				expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingStrong_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			StampedValue old_ = this->sequencedLoadStamped();
			if (!(::dcool::core::intelliHasEqualValueRepresentation(expected_, old_))) {
				expected_ = old_;
				return false;
			}
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return true;
		}

		public: constexpr auto stampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStampedStrong(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto stampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedCompareExchangeStampedStrong(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedStampedCompareExchangeStampedStrong(expected_, desired_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStrong(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			if constexpr (lockFull) {
				::dcool::core::PhoneyLockGuard guard_(this->m_locker_.mutex_);
				return this->sequencedStampedCompareExchangeStrong(expected_, desired_);
			}
			StampedValue old_ = {
				.value = expected_,
				.stamp = 0
			};
			for (; ; ) {
				if (this->atomicallyStampedCompareExchangeStampedWeak(old_, desired_, successOrder_, failureOrder_)) {
					break;
				}
				if (!(::dcool::core::intelliHasEqualValueRepresentation(old_.value, expected_))) {
					expected_ = old_.value;
					return false;
				}
			}
			return true;
		}

		public: constexpr auto sequencedStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			StampedValue old_ = this->sequencedLoadStamped();
			if (!(::dcool::core::intelliHasEqualValueRepresentation(expected_, old_.value_))) {
				expected_ = old_.value_;
				return false;
			}
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return true;
		}

		public: constexpr auto stampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStrong(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto stampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedStampedCompareExchangeStrong(expected_, desired_);
		}

		public: constexpr auto atomicallyCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return this->atomicallyCompareExchangeStrong(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto atomicallyCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			::dcool::core::Boolean result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->atomicallyCompareExchangeUnderlyingStrong_(expected_, desired_, successOrder_, failureOrder_);
			}
			return result_;
		}

		public: constexpr auto sequencedCompareExchangeStrong(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedCompareExchangeStrong(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto compareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: constexpr auto compareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			}
			return this->sequencedCompareExchangeStrong(expected_, desired_);
		}

		// Fetch transform operations
		private: template <typename TransformerT__> struct StampedStampedTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> StampedValue {
				return StampedValue {
					.value = ::dcool::core::invoke(*(this->transformer_), ::dcool::core::constantize(old_)),
					.stamp = old_.stamp + 1
				};
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> StampedValue {
				return StampedValue {
					.value = ::dcool::core::invoke(
						::dcool::core::forward<TransformerT__>(*(this->transformer_)), ::dcool::core::constantize(old_)
					),
					.stamp = old_.stamp + 1
				};
			}
		};

		private: template <typename TransformerT__> struct StampedTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> Value {
				return ::dcool::core::invoke(*(this->transformer_), ::dcool::core::constantize(old_.value));
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> Value {
				return ::dcool::core::invoke(
					::dcool::core::forward<TransformerT__>(*(this->transformer_)), ::dcool::core::constantize(old_.value)
				);
			}
		};

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyFetchTransformUnderlying_(
				StampedStampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedFetchTransformUnderlying_(
				StampedStampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedFetchTransformStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedFetchTransformStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetchStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyTransformFetchUnderlying_(
				StampedStampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedTransformFetchStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedTransformFetchUnderlying_(
				StampedStampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedTransformFetchStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedTransformFetchStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedTransformFetchStamped(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransform(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformStamped(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedFetchTransformStamped(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedFetchTransform(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetch(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetchStamped(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedTransformFetch(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedTransformFetchStamped(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedTransformFetch(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedTransformFetch(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedTransformFetch(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyFetchTransform(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyFetchTransformUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto fetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->fetchTransform(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto fetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyTransformFetch(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyTransformFetch(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedTransformFetch(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyTransformFetchUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedTransformFetch(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedTransformFetch(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedTransformFetchUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto transformFetch(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->transformFetch(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto transformFetch(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyTransformFetch(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedTransformFetch(::dcool::core::forward<TransformerT__>(transformer_));
		}

		// Fetch transform or load operations
		private: template <typename TransformerT__> struct StampedStampedConditionalTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> StampedValue {
				auto newValue_ = ::dcool::core::invoke(*(this->transformer_), ::dcool::core::constantize(old_));
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return StampedValue {
					.value = newValue_.access(),
					.stamp = old_.stamp + 1
				};
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> StampedValue {
				auto newValue_ = ::dcool::core::invoke(
					::dcool::core::forward<TransformerT__>(*(this->transformer_)), ::dcool::core::constantize(old_)
				);
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return StampedValue {
					.value = newValue_.access(),
					.stamp = old_.stamp + 1
				};
			}
		};

		private: template <typename TransformerT__> struct StampedConditionalTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> Value {
				auto newValue_ = ::dcool::core::invoke(*(this->transformer_), ::dcool::core::constantize(old_.value));
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return newValue_.access();
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> Value {
				auto newValue_ = ::dcool::core::invoke(
					::dcool::core::forward<TransformerT__>(*(this->transformer_)), ::dcool::core::constantize(old_.value)
				);
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return newValue_.access();
			}
		};

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyFetchTransformOrLoadUnderlying_(
				StampedStampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedFetchTransformOrLoadUnderlying_(
				StampedStampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedFetchTransformOrLoadStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedFetchTransformOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetchOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyTransformFetchOrLoadUnderlying_(
				StampedStampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedTransformFetchOrLoadStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedTransformFetchOrLoadUnderlying_(
				StampedStampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedTransformFetchOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedTransformFetchOrLoadStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedTransformFetchOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedFetchTransformOrLoadStamped(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedFetchTransformOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedFetchTransformOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetchOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedTransformFetchOrLoadStamped(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedTransformFetchOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedTransformFetchOrLoadStamped(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedTransformFetchOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyStampedTransformFetchOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedStampedTransformFetchOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyFetchTransformOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedFetchTransformOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyFetchTransformOrLoadUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedFetchTransformOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto fetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->fetchTransformOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto fetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyFetchTransformOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto atomicallyTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyTransformFetchOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyTransformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedTransformFetchOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyTransformFetchOrLoadUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedTransformFetchOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedTransformFetchOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedTransformFetchOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto transformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->transformFetchOrLoad(
				::dcool::core::forward<TransformerT__>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TransformerT__> constexpr auto transformFetchOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			if constexpr (supportAtomicOperation_) {
				return this->atomicallyTransformFetchOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return this->sequencedTransformFetchOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
		}

		// Wait operations
		public: constexpr void atomicallyStampedWaitStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyWaitUnderlying_(old_, order_);
		}

		public: constexpr void stampedWaitStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWaitStamped(old_, order_);
		}

		public: constexpr void atomicallyStampedWait(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyWaitUnderlyingPredicate_(
				[&old_](StampedValue const& value_) noexcept -> ::dcool::core::Boolean {
					return !(::dcool::core::intelliHasEqualValueRepresentation(value_.value, old_));
				},
				order_
			);
		}

		public: constexpr void stampedWait(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWait(old_, order_);
		}

		public: constexpr void atomicallyWait(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			if constexpr (stamped) {
				this->atomicallyStampedWait(old_, order_);
			} else {
				this->atomicallyWaitUnderlying_(old_, order_);
			}
		}

		public: constexpr void wait(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			this->atomicallyWait(old_, order_);
		}

		// Timed wait operations
		public: template <typename TimePointT__> constexpr auto atomicallyStampedWaitStampedUntil(
			StampedValue const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> ::dcool::core::Boolean requires (waitable && stamped) {
			return this->atomicallyWaitUnderlyingUntil_(old_, deadline_, order_);
		}

		public: template <typename TimePointT__> constexpr auto stampedWaitStampedUntil(
			StampedValue const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> ::dcool::core::Boolean requires (waitable && stamped) {
			return this->atomicallyStampedWaitStampedUntil(old_, deadline_, order_);
		}

		public: template <typename TimePointT__> constexpr auto atomicallyStampedWaitUntil(
			Value const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> ::dcool::core::Boolean requires (waitable && stamped) {
			if constexpr (lockFull) {
				::std::unique_lock locker_(this->m_locker_.mutex_);
				return this->m_locker_.blocker_.wait_until(
					locker_,
					deadline_,
					[this, &old_]() noexcept -> ::dcool::core::Boolean {
						return !(::dcool::core::intelliHasEqualValueRepresentation(this->sequencedStampedLoad(), old_));
					}
				);
			}
			for (; ; ) {
				StampedValue stampedOld_ = this->atomicallyLoadStamped(order_);
				if (!(::dcool::core::intelliHasEqualValueRepresentation(stampedOld_.value, old_))) {
					break;
				}
				if (TimePointT__::clock::now() > deadline_) {
					return false;
				}
				::std::this_thread::yield();
				if (!(this->atomicallyStampedWaitStampedUntil(stampedOld_, deadline_, order_))) {
					return false;
				}
			}
			return true;
		}

		public: template <typename TimePointT__> constexpr void stampedWaitUntil(
			Value const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWaitUntil(old_, deadline_, order_);
		}

		public: template <typename TimePointT__> constexpr void atomicallyWaitUntil(
			Value const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			if constexpr (stamped) {
				this->atomicallyStampedWaitUntil(old_, deadline_, order_);
			} else {
				this->atomicallyWaitUnderlyingUntil_(old_, deadline_, order_);
			}
		}

		public: template <typename TimePointT__> constexpr void wait(
			Value const& old_, TimePointT__ const& deadline_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			this->atomicallyWaitUntil(old_, deadline_, order_);
		}

		// Wait fetch operations
		public: constexpr auto atomicallyStampedWaitFetchWeakStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyWaitFetchWeakUnderlying_(old_, order_);
		}

		public: constexpr auto stampedWaitFetchWeakStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedWaitFetchWeakStamped(old_, order_);
		}

		public: constexpr auto atomicallyStampedWaitFetchWeak(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			StampedValue stampedValue_ = this->atomicallyLoadStamped(order_);
			if (!::dcool::core::intelliHasEqualValueRepresentation(stampedValue_.value, old_)) {
				return stampedValue_.value;
			}
			::std::this_thread::yield();
			return this->atomicallyWaitFetchWeakUnderlying_(
				StampedValue {
					.value = old_,
					.stamp = stampedValue_.stamp
				},
				order_
			);
		}

		public: constexpr auto stampedWaitFetchWeak(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedWaitFetchWeak(old_, order_);
		}

		public: constexpr auto atomicallyWaitFetchWeak(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedWaitFetchWeak(old_, order_);
			} else {
				result_ = this->atomicallyWaitFetchWeakUnderlying_(old_, order_);
			}
			return result_;
		}

		public: constexpr auto waitFetchWeak(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			return this->atomicallyWaitFetchWeak(old_, order_);
		}

		public: constexpr auto atomicallyStampedWaitFetchStrongStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyWaitFetchStrongUnderlying_(old_, order_);
		}

		public: constexpr auto stampedWaitFetchStrongStamped(
			StampedValue const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedWaitFetchStrongStamped(old_, order_);
		}

		public: constexpr auto atomicallyStampedWaitFetchStrong(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			Value result_;
			this->atomicallyWaitUnderlyingPredicate_(
				[&old_, &result_](StampedValue const& value_) noexcept -> ::dcool::core::Boolean {
					if (::dcool::core::intelliHasEqualValueRepresentation(value_.value, old_)) {
						return false;
					}
					result_ = value_.value;
					return true;
				},
				order_
			);
			return result_;
		}

		public: constexpr auto stampedWaitFetchStrong(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedWaitFetchStrong(old_, order_);
		}

		public: constexpr auto atomicallyWaitFetchStrong(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedWaitFetchStrong(old_, order_);
			} else {
				result_ = this->atomicallyWaitFetchStrongUnderlying_(old_, order_);
			}
			return result_;
		}

		public: constexpr auto waitFetchStrong(
			Value const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			return this->atomicallyWaitFetchStrong(old_, order_);
		}

		// Wait predicate operations
		public: template <typename PredicateT__> constexpr auto atomicallyStampedWaitPredicateFetchStamped(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyWaitPredicateFetchUnderlying_(::dcool::core::forward<PredicateT__>(predicate_), order_);
		}

		public: template <typename PredicateT__> constexpr auto stampedWaitPredicateFetchStamped(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedWaitPredicateFetchStamped(::dcool::core::forward<PredicateT__>(predicate_), order_);
		}

		private: template <typename PredicateT__> struct StampedPredicate_ {
			::dcool::core::ReferenceRemovedType<PredicateT__>* predicate_;

			constexpr auto operator ()(StampedValue const& old_)& noexcept -> ::dcool::core::Boolean {
				return ::dcool::core::invoke(*(this->predicate_), ::dcool::core::constantize(old_.value));
			}

			constexpr auto operator ()(StampedValue const& old_)&& noexcept -> ::dcool::core::Boolean {
				return ::dcool::core::invoke(
					::dcool::core::forward<PredicateT__>(*(this->predicate_)), ::dcool::core::constantize(old_.value)
				);
			}
		};

		public: template <typename PredicateT__> constexpr auto atomicallyStampedWaitPredicateFetch(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedWaitPredicateFetchStamped(
				StampedPredicate_<PredicateT__> {
					.predicate_ = ::dcool::core::addressOf(predicate_)
				},
				order_
			).value;
		}

		public: template <typename PredicateT__> constexpr auto stampedWaitPredicateFetch(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedWaitPredicateFetch(::dcool::core::forward<PredicateT__>(predicate_), order_);
		}

		public: template <typename PredicateT__> constexpr auto atomicallyWaitPredicateFetch(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->stampedWaitPredicateFetch(::dcool::core::forward<PredicateT__>(predicate_), order_);
			} else {
				result_ = this->atomicallyWaitPredicateFetchUnderlying_(::dcool::core::forward<PredicateT__>(predicate_), order_);
			}
			return result_;
		}

		public: template <typename PredicateT__> constexpr auto waitPredicateFetch(
			PredicateT__&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (waitable) {
			return this->atomicallyWaitPredicateFetch(::dcool::core::forward<PredicateT__>(predicate_), order_);
		}

		// Wait equality operations
		public: constexpr void atomicallyStampedWaitEqualityStamped(
			StampedValue const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyWaitEqualityUnderlying_(expected_, order_);
		}

		public: constexpr void stampedWaitEqualityStamped(
			StampedValue const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWaitEqualityStamped(expected_, order_);
		}

		public: constexpr void atomicallyStampedWaitEquality(
			Value const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWaitPredicateFetch(
				[&expected_](Value const& value_) noexcept -> ::dcool::core::Boolean {
					return ::dcool::core::intelliHasEqualValueRepresentation(value_, expected_);
				},
				order_
			);
		}

		public: constexpr void stampedWaitEquality(
			Value const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable && stamped) {
			this->atomicallyStampedWaitEquality(expected_, order_);
		}

		public: constexpr void atomicallyWaitEquality(
			Value const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			if constexpr (stamped) {
				this->atomicallyStampedWaitEquality(expected_, order_);
			} else {
				this->atomicallyWaitEqualityUnderlying_(expected_, order_);
			}
		}

		public: constexpr void waitEquality(
			Value const& expected_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept requires (waitable) {
			this->atomicallyWaitEquality(expected_, order_);
		}

		// Notify operations
		public: constexpr void atomicallyNotifyOne() noexcept requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				::dcool::concurrency::atomicallyNotifyOne(this->m_holder_.underlying_);
			} else {
				this->m_blocker_.notify_one();
			}
		}

		public: constexpr void atomicallyNotifyAll() noexcept requires (waitable) {
			if constexpr (usingAtomicBasicOperation_) {
				::dcool::concurrency::atomicallyNotifyAll(this->m_holder_.underlying_);
			} else {
				this->m_blocker_.notify_all();
			}
		}

		public: constexpr void notifyOne() noexcept requires (waitable) {
			return this->atomicallyNotifyOne();
		}

		public: constexpr void notifyAll() noexcept requires (waitable) {
			return this->atomicallyNotifyAll();
		}

		// Task operations
		private: template <typename TaskT__> struct StampedStampedTask_ {
			using TaskResult_ = ::dcool::core::TaskResult<StampedValue>;

			::dcool::core::ReferenceRemovedType<TaskT__>* task_;

			constexpr auto operator ()(StampedValue const& old_)& noexcept -> TaskResult_ {
				return TaskResult_::transformFrom(
					::dcool::core::invoke(*(this->task_), ::dcool::core::constantize(old_)),
					[&old_](Value const& value_) noexcept -> StampedValue {
						return TaskResult_(
							StampedValue {
								.value = value_,
								.stamp = old_.stamp + 1
							}
						);
					}
				);
			}

			constexpr auto operator ()(StampedValue const& old_)&& noexcept -> StampedValue {
				return TaskResult_::transformFrom(
					::dcool::core::invoke(::dcool::core::forward<TaskT__>(*(this->transformer_)), ::dcool::core::constantize(old_)),
					[&old_](Value const& value_) noexcept -> StampedValue {
						return TaskResult_(
							StampedValue {
								.value = value_,
								.stamp = old_.stamp + 1
							}
						);
					}
				);
			}
		};

		private: template <typename TaskT__> struct StampedTask_ {
			using TaskResult_ = ::dcool::core::TaskResult<Value>;

			::dcool::core::ReferenceRemovedType<TaskT__>* task_;

			constexpr auto operator ()(StampedValue const& old_)& noexcept -> TaskResult_ {
				return TaskResult_::transformFrom(::dcool::core::invoke(*(this->task_), ::dcool::core::constantize(old_.value)));
			}

			constexpr auto operator ()(StampedValue const& old_)&& noexcept -> TaskResult_ {
				return TaskResult_::transformFrom(
					::dcool::core::invoke(
						::dcool::core::forward<TaskT__>(*(this->task_)), ::dcool::core::constantize(old_.value)
					),
					::dcool::core::constantize(old_.value)
				);
			}
		};

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedFetchExecuteStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecuteStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedFetchExecuteStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyFetchExecuteUnderlying_(
				hint_,
				StampedStampedTask_<TaskT__> {
					.task_ = ::dcool::core::addressOf(task_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedFetchExecuteStamped(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedFetchExecuteStamped(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedFetchExecuteStamped(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecuteStamped(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedFetchExecuteStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->stampedHintedFetchExecuteStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedFetchExecuteStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecuteStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedFetchExecuteStamped(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->stampedFetchExecuteStamped(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedFetchExecuteStamped(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedFetchExecuteStamped(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecute(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecuteStamped(
				hint_,
				StampedTask_<TaskT__> {
					.task_ = ::dcool::core::addressOf(task_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedFetchExecute(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedFetchExecute(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedFetchExecute(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecute(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			).value;
		}

		public: template <typename TaskT__> constexpr auto stampedHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->stampedHintedFetchExecute(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedFetchExecute(
				hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedFetchExecute(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->stampedFetchExecute(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedFetchExecute(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedFetchExecute(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto atomicallyHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedFetchExecute(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyHintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedHintedFetchExecute(
					hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyHintedFetchExecuteUnderlying_(
					hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TaskT__> constexpr auto atomicallyFetchExecute(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyFetchExecute(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyFetchExecute(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedFetchExecute(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto hintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->hintedFetchExecute(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto hintedFetchExecute(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedFetchExecute(hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto fetchExecute(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->fetchExecute(::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: template <typename TaskT__> constexpr auto fetchExecute(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyFetchExecute(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedExecuteFetchStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetchStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedExecuteFetchStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyExecuteFetchUnderlying_(
				hint_,
				StampedStampedTask_<TaskT__> {
					.task_ = ::dcool::core::addressOf(task_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedExecuteFetchStamped(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedExecuteFetchStamped(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedExecuteFetchStamped(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetchStamped(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedExecuteFetchStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->stampedHintedExecuteFetchStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedExecuteFetchStamped(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetchStamped(
				hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedExecuteFetchStamped(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->stampedExecuteFetchStamped(::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: template <typename TaskT__> constexpr auto stampedExecuteFetchStamped(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (waitable && stamped) {
			return this->atomicallyStampedExecuteFetchStamped(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetch(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetchStamped(
				hint_,
				StampedTask_<TaskT__> {
					.task_ = ::dcool::core::addressOf(task_)
				},
				transformOrder_,
				loadOrder_
			).value;
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedExecuteFetch(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedExecuteFetch(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyStampedExecuteFetch(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetch(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			).value;
		}

		public: template <typename TaskT__> constexpr auto stampedHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->stampedHintedExecuteFetch(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedHintedExecuteFetch(
				hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto stampedExecuteFetch(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable && stamped) {
			return this->stampedExecuteFetch(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto stampedExecuteFetch(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable && stamped) {
			return this->atomicallyStampedExecuteFetch(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto atomicallyHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedExecuteFetch(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyHintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedHintedExecuteFetch(
					hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyHintedExecuteFetchUnderlying_(
					hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TaskT__> constexpr auto atomicallyExecuteFetch(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyExecuteFetch(
				::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto atomicallyExecuteFetch(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedExecuteFetch(
				this->atomicallyLoad(loadOrder_), ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_
			);
		}

		public: template <typename TaskT__> constexpr auto hintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->hintedExecuteFetch(
				hint_, ::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
			);
		}

		public: template <typename TaskT__> constexpr auto hintedExecuteFetch(
			UnderlyingValue const& hint_, TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyHintedExecuteFetch(hint_, ::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		public: template <typename TaskT__> constexpr auto executeFetch(
			TaskT__&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (waitable) {
			return this->executeFetch(::dcool::core::forward<TaskT__>(task_), order_, ::dcool::concurrency::toReadOrder(order_));
		}

		public: template <typename TaskT__> constexpr auto executeFetch(
			TaskT__&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (waitable) {
			return this->atomicallyExecuteFetch(::dcool::core::forward<TaskT__>(task_), transformOrder_, loadOrder_);
		}

		// Arithmetic operations
#	define DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Operation_, operation_, standardOperation_, operator_) \
		private: template <typename OperandT__> constexpr auto atomicallyFetch##Operation_##Underlying_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) { \
			if constexpr (usingAtomicBasicOperation_) { \
				return ::dcool::concurrency::atomicallyFetch##Operation_(this->m_holder_.underlying_, operand_, order_); \
			} \
			return this->atomicallyFetchTransformUnderlying_( \
				::dcool::core::Operation_##Transformer<OperandT__ const&> { \
					.operand = operand_ \
				}, \
				order_, \
				::std::memory_order::relaxed \
			); \
		} \
		\
		private: template <typename OperandT__> constexpr auto sequencedFetch##Operation_##Underlying_( \
			OperandT__ const& operand_ \
		)& noexcept -> UnderlyingValue { \
			if constexpr (usingStandardAtomic_) { \
				return this->atomicallyFetch##Operation_##Underlying_(operand_, ::std::memory_order::relaxed); \
			} \
			UnderlyingValue result_ = this->m_holder_.underlying_; \
			this->m_holder_.underlying_ = result_ + operand_; \
			return result_; \
		} \
		\
		private: template <typename OperandT__> constexpr auto atomically##Operation_##FetchUnderlying_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> UnderlyingValue requires (supportAtomicOperation_) { \
			if constexpr (usingAtomicBasicOperation_) { \
				return ::dcool::concurrency::atomically##Operation_##Fetch(this->m_holder_.underlying_, operand_, order_); \
			} \
			return this->atomicallyTransformFetchUnderlying_( \
				::dcool::core::Operation_##Transformer<OperandT__ const&> { \
					.operand = operand_ \
				}, \
				order_, \
				::std::memory_order::relaxed \
			); \
		} \
		\
		private: template <typename OperandT__> constexpr auto sequenced##Operation_##FetchUnderlying_( \
			OperandT__ const& operand_ \
		)& noexcept -> UnderlyingValue { \
			if constexpr (usingStandardAtomic_) { \
				return this->atomically##Operation_##FetchUnderlying_(operand_, ::std::memory_order::relaxed); \
			} \
			UnderlyingValue result_ = this->m_holder_.underlying_; \
			this->m_holder_.underlying_ = result_ + operand_; \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) { \
			auto operationTransformer_ = ::dcool::core::Operation_##Transformer<OperandT__ const&> { \
				.operand = operand_ \
			}; \
			return this->atomicallyStampedFetchTransformStamped( \
				StampedTransformer_<decltype(operationTransformer_)&&> { \
					.transformer_ = ::dcool::core::addressOf(operationTransformer_) \
				}, \
				order_, \
				::std::memory_order::relaxed \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_ \
		)& noexcept -> StampedValue requires (stamped) { \
			auto operationTransformer_ = ::dcool::core::Operation_##Transformer<OperandT__ const&> { \
				.operand = operand_ \
			}; \
			return this->sequencedStampedFetchTransformStamped( \
				StampedTransformer_<decltype(operationTransformer_)&&> { \
					.transformer_ = ::dcool::core::addressOf(operationTransformer_) \
				} \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto stampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (stamped) { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomicallyStampedFetch##Operation_##Stamped(operand_, order_); \
			} \
			return this->sequencedStampedFetch##Operation_##Stamped(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStamped##Operation_##FetchStamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) { \
			auto operationTransformer_ = ::dcool::core::Operation_##Transformer<OperandT__ const&> { \
				.operand = operand_ \
			}; \
			return this->atomicallyStampedTransformFetchStamped( \
				StampedTransformer_<decltype(operationTransformer_)&&> { \
					.transformer_ = ::dcool::core::addressOf(operationTransformer_) \
				}, \
				order_, \
				::std::memory_order::relaxed \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStamped##Operation_##FetchStamped( \
			OperandT__ const& operand_ \
		)& noexcept -> StampedValue requires (stamped) { \
			auto operationTransformer_ = ::dcool::core::Operation_##Transformer<OperandT__ const&> { \
				.operand = operand_ \
			}; \
			return this->sequencedStampedTransformFetchStamped( \
				StampedTransformer_<decltype(operationTransformer_)&&> { \
					.transformer_ = ::dcool::core::addressOf(operationTransformer_) \
				} \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto stamped##Operation_##FetchStamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (stamped) { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomicallyStamped##Operation_##FetchStamped(operand_, order_); \
			} \
			return this->sequencedStamped##Operation_##FetchStamped(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStampedFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) { \
			return this->atomicallyStampedFetch##Operation_##Stamped(operand_, order_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStampedFetch##Operation_( \
			OperandT__ const& operand_ \
		)& noexcept -> Value requires (stamped) { \
			return this->sequencedStampedFetch##Operation_##Stamped(operand_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto stampedFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (stamped) { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomicallyStampedFetch##Operation_(operand_, order_); \
			} \
			return this->sequencedStampedFetch##Operation_(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStamped##Operation_##Fetch( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) { \
			return this->atomicallyStamped##Operation_##FetchStamped(operand_, order_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStamped##Operation_##Fetch( \
			OperandT__ const& operand_ \
		)& noexcept -> Value requires (stamped) { \
			return this->sequencedStamped##Operation_##FetchStamped(operand_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto stamped##Operation_##Fetch( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (stamped) { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomicallyStamped##Operation_##Fetch(operand_, order_); \
			} \
			return this->sequencedStamped##Operation_##Fetch(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_) { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->atomicallyStampedFetch##Operation_(operand_, order_); \
			} else { \
				result_ = this->atomicallyFetch##Operation_##Underlying_(operand_, order_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedFetch##Operation_( \
			OperandT__ const& operand_ \
		)& noexcept -> Value { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->sequencedStampedFetch##Operation_(operand_); \
			} else { \
				result_ = this->sequencedFetch##Operation_##Underlying_(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto fetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomicallyFetch##Operation_(operand_, order_); \
			} \
			return this->sequencedFetch##Operation_(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomically##Operation_##Fetch( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_) { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->atomicallyStamped##Operation_##Fetch(operand_, order_); \
			} else { \
				result_ = this->atomically##Operation_##FetchUnderlying_(operand_, order_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequenced##Operation_##Fetch( \
			OperandT__ const& operand_ \
		)& noexcept -> Value { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->sequencedStamped##Operation_##Fetch(operand_); \
			} else { \
				result_ = this->sequenced##Operation_##FetchUnderlying_(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto operation_##Fetch( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value { \
			if constexpr (supportAtomicOperation_) { \
				return this->atomically##Operation_##Fetch(operand_, order_); \
			} \
			return this->sequenced##Operation_##Fetch(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto operator operator_##=( \
			OperandT__ const& operand_ \
		)& noexcept -> Value { \
			return this->operation_##Fetch(operand_); \
		} \
		\
		public: template <typename OperandT__> constexpr auto fetch_##standardOperation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value { \
			return this->fetch##Operation_(operand_, order_); \
		}

		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Add, add, add, +)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Subtract, subtract, sub, -)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseAnd, bitwiseAnd, and, *)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseOr, bitwiseOr, or, /)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseExclusiveOr, bitwiseExclusiveOr, xor, ^)

		public: constexpr auto operator ++()& noexcept -> Value {
			return this->fetchAdd(1);
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator)& noexcept -> Value {
			return this->addFetch(1);
		}

		public: constexpr auto operator --()& noexcept -> Value {
			return this->fetchSubtract(1);
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator)& noexcept -> Value {
			return this->subtractFetch(1);
		}

		public: constexpr auto is_lock_free() const noexcept -> ::dcool::core::Boolean {
			return this->lockFreeOnExecution();
		}

		public: constexpr auto compare_exchange_weak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, order_);
		}

		public: constexpr auto compare_exchange_weak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
		}

		public: constexpr auto compare_exchange_strong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, order_);
		}

		public: constexpr auto compare_exchange_strong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
		}

		public: constexpr void notify_one() noexcept requires (supportAtomicOperation_) {
			return this->notifyOne();
		}

		public: constexpr void notify_all() noexcept requires (supportAtomicOperation_) {
			return this->notifyAll();
		}
	};
}

#endif
