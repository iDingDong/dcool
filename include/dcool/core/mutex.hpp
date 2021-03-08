#ifndef DCOOL_CORE_MUTEX_HPP_INCLUDED_
#	define DCOOL_CORE_MUTEX_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/functional.hpp>
#	include <dcool/core/member_detector.hpp>

#	include <concepts>
#	include <mutex>
#	include <shared_mutex>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeMutex, ExtractedMutexType, Mutex)

namespace dcool::core {
	template <typename T_> concept StandardBasicLockable = requires (T_ mutex_) {
		mutex_.lock();
		mutex_.unlock(); // ISO C++ requires this expression to throw nothing, but 'noexcept' is not required.
	};

	template <typename T_> concept StandardLockable = ::dcool::core::StandardBasicLockable<T_> && requires (
		T_ mutex_, ::dcool::core::Boolean result_
	) {
		result_ = mutex_.try_lock();
	};

	template <typename T_> concept StandardMutex =
		::dcool::core::StandardLockable<T_> && // ISO C++ requires 'try_lock' to throw nothing, but 'noexcept' is not required.
		::std::is_default_constructible_v<T_> &&
		::std::is_destructible_v<T_> &&
		(!::std::is_copy_constructible_v<T_>) &&
		(!::std::is_copy_assignable_v<T_>) &&
		(!::std::is_move_constructible_v<T_>) &&
		(!::std::is_move_assignable_v<T_>)
	;

	template <typename T_> concept StandardSharedMutex =
		::dcool::core::StandardMutex<T_> &&
		requires (T_ mutex_, ::dcool::core::Boolean result_) {
			mutex_.lock_shared();
			result_ = mutex_.try_lock_shared(); // ISO C++ requires this expression to throw nothing, but 'noexcept' is not required.
			mutex_.unlock_shared(); // ISO C++ requires this expression to throw nothing, but 'noexcept' is not required.
		}
	;

	namespace detail_ {
		template <typename UnderlyingT_> class AdaptedStandardMutexBase_ {
			private: using Self_ = AdaptedStandardMutexBase_<UnderlyingT_>;
			public: using Underlying = UnderlyingT_;

			public: [[no_unique_address]] Underlying underlying;

			public: constexpr void lock() noexcept(noexcept(::dcool::core::declval<Underlying&>().lock())) {
				this->underlying.lock();
			}

			public: constexpr ::dcool::core::Boolean tryLock() noexcept {
				return this->underlying.try_lock();
			}

			public: constexpr void unlock() noexcept(noexcept(::dcool::core::declval<Underlying&>().unlock())) {
				this->underlying.unlock();
			}

			public: constexpr ::dcool::core::Boolean try_lock() noexcept {
				return this->tryLock();
			}
		};
	}

	template <
		::dcool::core::StandardMutex UnderlyingT_
	> class AdaptedStandardMutex: public ::dcool::core::detail_::AdaptedStandardMutexBase_<UnderlyingT_> {
		private: using Self_ = AdaptedStandardMutex<UnderlyingT_>;
		private: using Super_ = ::dcool::core::detail_::AdaptedStandardMutexBase_<UnderlyingT_>;
		private: using Underlying_ = UnderlyingT_;
	};

	template <
		::dcool::core::StandardSharedMutex UnderlyingT_
	> class AdaptedStandardMutex<UnderlyingT_>: public ::dcool::core::detail_::AdaptedStandardMutexBase_<UnderlyingT_> {
		private: using Self_ = AdaptedStandardMutex<UnderlyingT_>;
		private: using Super_ = ::dcool::core::detail_::AdaptedStandardMutexBase_<UnderlyingT_>;
		private: using Underlying_ = UnderlyingT_;

		public: using typename Super_::Underlying;
		public: using Super_::underlying;

		public: constexpr void lockShared() noexcept(::dcool::core::declval<Underlying&>().lock_shared()) {
			this->underlying.lock_shared();
		}

		public: constexpr ::dcool::core::Boolean tryLockShared() noexcept {
			return this->underlying.try_lock_shared();
		}

		public: constexpr void unlockShared() noexcept(::dcool::core::declval<Underlying&>().unlock_shared()) {
			this->underlying.unlock_shared();
		}

		public: constexpr void lock_shared() noexcept(::dcool::core::declval<Self_&>().lockShared()) {
			this->lockShared();
		}

		public: constexpr ::dcool::core::Boolean try_lock_shared() noexcept {
			return this->tryLockShared();
		}

		public: constexpr void unlock_shared() noexcept(::dcool::core::declval<Self_&>().unlockShared()) {
			this->unlockShared();
		}
	};

	using MinimalMutex = ::dcool::core::AdaptedStandardMutex<std::mutex>;
	using MinimalSharedMutex = ::dcool::core::AdaptedStandardMutex<std::shared_mutex>;

	template <typename T_> concept Mutex =
		::dcool::core::StandardBasicLockable<T_> &&
		::std::is_default_constructible_v<T_> &&
		::std::is_destructible_v<T_> &&
		(!::std::is_copy_constructible_v<T_>) &&
		(!::std::is_copy_assignable_v<T_>) &&
		(!::std::is_move_constructible_v<T_>) &&
		(!::std::is_move_assignable_v<T_>) &&
		noexcept(::dcool::core::declval<::dcool::core::Boolean&>() = ::dcool::core::declval<T_&>().tryLock())
	;

	template <typename T_> concept SharedMutex =
		::dcool::core::Mutex<T_> &&
		noexcept(::dcool::core::declval<::dcool::core::Boolean&>() = ::dcool::core::declval<T_&>().tryLockShared()) &&
		requires (T_ mutex_) {
			mutex_.lockShared();
			mutex_.unlockShared();
		}
	;

	template <typename T_> concept CompatibleMutex = ::dcool::core::Mutex<T_> || ::dcool::core::StandardMutex<T_>;

	template <
		typename T_
	> concept CompatibleSharedMutex = ::dcool::core::SharedMutex<T_> || ::dcool::core::StandardSharedMutex<T_>;

	template <
		::dcool::core::CompatibleMutex MutexT_
	> constexpr void generalLock(MutexT_& mutex_) noexcept(noexcept(mutex_.lock())) {
		mutex_.lock();
	}
}

DCOOL_CORE_DEFINE_MEMBER_CALLER(dcool::core::detail_, HasCallableTryLock_, tryLockOr_, tryLock)

namespace dcool::core {
	template <
		::dcool::core::CompatibleMutex MutexT_
	> constexpr auto generalTryLock(MutexT_& mutex_) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::detail_::tryLockOr_(
			[](auto& mutex_) -> ::dcool::core::Boolean {
				return mutex_.try_lock();
			},
			mutex_
		);
	}

	template <
		::dcool::core::CompatibleMutex MutexT_
	> constexpr void generalUnlock(MutexT_& mutex_) noexcept {
		mutex_.unlock();
	}
}

DCOOL_CORE_DEFINE_MEMBER_CALLER(dcool::core::detail_, HasCallableLockShared_, lockSharedOr_, lockShared)

namespace dcool::core {
	template <
		::dcool::core::CompatibleSharedMutex MutexT_
	> constexpr void generalLockShared(MutexT_& mutex_) noexcept(
		noexcept(
			::dcool::core::detail_::lockSharedOr_(
				[](auto& mutex_) {
					mutex_.lock_shared();
				},
				mutex_
			)
		)
	) {
		::dcool::core::detail_::lockSharedOr_(
				[](auto& mutex_) {
					mutex_.lock_shared();
				},
				mutex_
			);
	}
}

DCOOL_CORE_DEFINE_MEMBER_CALLER(dcool::core::detail_, HasCallableTryLockShared_, tryLockSharedOr_, tryLockShared)

namespace dcool::core {
	template <
		::dcool::core::CompatibleSharedMutex MutexT_
	> constexpr auto generalTryLockShared(MutexT_& mutex_) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::detail_::tryLockSharedOr_(
			[](auto& mutex_) -> ::dcool::core::Boolean {
				return mutex_.try_lock_shared();
			},
			mutex_
		);
	}
}

DCOOL_CORE_DEFINE_MEMBER_CALLER(dcool::core::detail_, HasCallableUnlockShared_, unlockSharedOr_, unlockShared)

namespace dcool::core {
	template <::dcool::core::CompatibleSharedMutex MutexT_> constexpr void generalUnlockShared(MutexT_& mutex_) noexcept {
		::dcool::core::detail_::unlockSharedOr_(
			[](auto& mutex_) {
				return mutex_.unlock_shared();
			},
			mutex_
		);
	}

	namespace detail_ {
		template <typename T_> consteval auto isNoThrowPhoneyLockable_() noexcept -> ::dcool::core::Boolean {
			if constexpr (::dcool::core::CompatibleMutex<T_>) {
				return noexcept(::dcool::core::generalLock(::dcool::core::declval<T_&>()));
			}
			return true;
		}

		template <typename T_> consteval auto isNoThrowPhoneySharedLockable_() noexcept -> ::dcool::core::Boolean {
			if constexpr (::dcool::core::CompatibleSharedMutex<T_>) {
				return noexcept(::dcool::core::generalLockShared(::dcool::core::declval<T_&>()));
			}
			return ::dcool::core::detail_::isNoThrowPhoneyLockable_<T_>();
		}
	}

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowPhoneyLockable =
		::dcool::core::detail_::isNoThrowPhoneyLockable_<T_>()
	;

	template <typename T_> constexpr ::dcool::core::Boolean isNoThrowPhoneySharedLockable =
		::dcool::core::detail_::isNoThrowPhoneySharedLockable_<T_>()
	;

	template <typename T_> concept NoThrowPhoneyLockable = ::dcool::core::isNoThrowPhoneyLockable<T_>;

	template <typename T_> concept NoThrowPhoneySharedLockable = ::dcool::core::isNoThrowPhoneySharedLockable<T_>;

	template <typename MutexT_> constexpr void phoneyLock(MutexT_& mutex_) noexcept(
		::dcool::core::isNoThrowPhoneyLockable<MutexT_>
	) {
		if constexpr (::dcool::core::CompatibleMutex<MutexT_>) {
			::dcool::core::generalLock(mutex_);
		}
	}

	template <typename MutexT_> constexpr auto phoneyTryLock(MutexT_& mutex_) noexcept -> ::dcool::core::Boolean {
		if constexpr (::dcool::core::CompatibleMutex<MutexT_>) {
			return ::dcool::core::generalTryLock(mutex_);
		}
		return true;
	}

	template <typename MutexT_> constexpr void phoneyUnlock(MutexT_& mutex_) noexcept {
		if constexpr (::dcool::core::CompatibleMutex<MutexT_>) {
			::dcool::core::generalUnlock(mutex_);
		}
	}

	template <typename MutexT_> constexpr void phoneyLockShared(MutexT_& mutex_) noexcept(
		::dcool::core::isNoThrowPhoneySharedLockable<MutexT_>
	) {
		if constexpr (::dcool::core::CompatibleSharedMutex<MutexT_>) {
			::dcool::core::generalLockShared(mutex_);
		} else {
			::dcool::core::phoneyLock(mutex_);
		}
	}

	template <typename MutexT_> constexpr auto phoneyTryLockShared(MutexT_& mutex_) noexcept -> ::dcool::core::Boolean {
		if constexpr (::dcool::core::CompatibleSharedMutex<MutexT_>) {
			return ::dcool::core::generalTryLockShared(mutex_);
		}
		return ::dcool::core::phoneyTryLock(mutex_);
	}

	template <typename MutexT_> constexpr void phoneyUnlockShared(MutexT_& mutex_) noexcept {
		if constexpr (::dcool::core::CompatibleSharedMutex<MutexT_>) {
			::dcool::core::generalUnlockShared(mutex_);
		} else {
			::dcool::core::phoneyUnlock(mutex_);
		}
	}

	template <typename MutexT_> struct PhoneyLockGuard {
		private: using Self_ = PhoneyLockGuard<MutexT_>;
		public: using Mutex = MutexT_;

		private: Mutex& m_mutex_;

		public: PhoneyLockGuard() = delete;
		public: PhoneyLockGuard(Self_ const&) = delete;
		public: PhoneyLockGuard(Self_&&) = delete;
		public: constexpr PhoneyLockGuard(Mutex& mutex_) noexcept: m_mutex_(mutex_) {
			::dcool::core::phoneyLock(this->m_mutex_);
		}

		public: constexpr ~PhoneyLockGuard() noexcept {
			::dcool::core::phoneyUnlock(this->m_mutex_);
		}

		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;
	};

	template <typename MutexT_> struct PhoneySharedLockGuard {
		private: using Self_ = PhoneySharedLockGuard<MutexT_>;
		public: using Mutex = MutexT_;

		private: Mutex& m_mutex_;

		public: PhoneySharedLockGuard() = delete;
		public: PhoneySharedLockGuard(Self_ const&) = delete;
		public: PhoneySharedLockGuard(Self_&&) = delete;
		public: constexpr PhoneySharedLockGuard(Mutex& mutex_) noexcept: m_mutex_(mutex_) {
			::dcool::core::phoneyLockShared(this->m_mutex_);
		}

		public: constexpr ~PhoneySharedLockGuard() noexcept {
			::dcool::core::phoneyUnlockShared(this->m_mutex_);
		}

		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;
	};

	template <typename UnderlyingT_> struct PhoneySharedMutex {
		private: using Self_ = AdaptedStandardMutex<UnderlyingT_>;
		public: using Underlying = UnderlyingT_;

		public: [[no_unique_address]] Underlying underlying;

		public: constexpr void lock() noexcept(::dcool::core::isNoThrowPhoneyLockable<Underlying>) {
			::dcool::core::phoneyLock(this->underlying);
		}

		public: constexpr ::dcool::core::Boolean tryLock() noexcept {
			return ::dcool::core::phoneyTryLock(this->underlying);
		}

		public: constexpr void unlock() noexcept {
			::dcool::core::phoneyUnlock(this->underlying);
		}

		public: constexpr void lockShared() noexcept(::dcool::core::isNoThrowPhoneySharedLockable<Underlying>) {
			::dcool::core::phoneyLockShared(this->underlying);
		}

		public: constexpr ::dcool::core::Boolean tryLockShared() noexcept {
			return ::dcool::core::phoneyTryLockShared(this->underlying);
		}

		public: constexpr void unlockShared() noexcept {
			::dcool::core::phoneyUnlockShared(this->underlying);
		}

		public: constexpr ::dcool::core::Boolean try_lock() noexcept {
			return this->tryLock();
		}

		public: constexpr void lock_shared() noexcept(::dcool::core::declval<Self_&>().lockShared()) {
			this->lockShared();
		}

		public: constexpr ::dcool::core::Boolean try_lock_shared() noexcept {
			return this->tryLockShared();
		}

		public: constexpr void unlock_shared() noexcept {
			this->unlockShared();
		}
	};

	using NoOpPhoneySharedMutex = ::dcool::core::PhoneySharedMutex<::dcool::core::Empty<>>;
	using UniquePhoneySharedMutex = ::dcool::core::PhoneySharedMutex<::dcool::core::MinimalMutex>;
	using AlmightyPhoneySharedMutex = ::dcool::core::PhoneySharedMutex<::dcool::core::MinimalSharedMutex>;
}

#endif
