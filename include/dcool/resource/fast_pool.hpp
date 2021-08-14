#ifndef DCOOL_CORE_FAST_POOL_HPP_INCLUDED_
#	define DCOOL_CORE_FAST_POOL_HPP_INCLUDED_ 1

#	include <dcool/resource/free_list.hpp>
#	include <dcool/resource/pool.hpp>

#	include <dcool/core.hpp>

#	include <array>
#	include <atomic>
#	include <new>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeFallbackPoolForCachedPool_, ExtractedFallbackPoolForCachedPoolType_, FallbackPool
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_,
	HasValueFallbackPoolSharedForCachedPool_,
	extractedFallbackPoolSharedForCachedPoolValue_,
	fallbackPoolShared
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_,
	HasValueSynchronizationRequiredForCachedPool_,
	extractedSynchronizationRequiredForCachedPoolValue_,
	synchronizationRequired
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_,
	HasValueQuickAvailabilityCheckForCachedPool_,
	extractedQuickAvailabilityCheckForCachedPoolValue_,
	quickAvailabilityCheck
)

namespace dcool::resource {
	namespace detail_ {
		template <typename ConfigT_> class CachedClassicPoolConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: using FallbackPool = ::dcool::resource::detail_::ExtractedFallbackPoolForCachedPoolType_<
				Config, ::dcool::resource::DefaultClassicPool
			>;
			public: static constexpr ::dcool::core::Boolean fallbackPoolShared =
				::dcool::resource::detail_::extractedFallbackPoolSharedForCachedPoolValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean synchronizationRequired =
				::dcool::resource::detail_::extractedSynchronizationRequiredForCachedPoolValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean quickAvailabilityCheck =
				::dcool::resource::detail_::extractedQuickAvailabilityCheckForCachedPoolValue_<Config>(synchronizationRequired)
			;
		};
	}

	template <typename T_> concept CachedClassicPoolConfig = requires {
		typename ::dcool::resource::detail_::CachedClassicPoolConfigAdaptor_<T_>;
	};

	template <
		::dcool::resource::CachedClassicPoolConfig ConfigT_
	> using CachedClassicPoolConfigAdaptor = ::dcool::resource::detail_::CachedClassicPoolConfigAdaptor_<ConfigT_>;

	template <::dcool::resource::CachedClassicPoolConfig ConfigT_ = ::dcool::core::Empty<>> class CachedClassicPool {
		private: using Self_ = CachedClassicPool<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::resource::CachedClassicPoolConfigAdaptor<Config>;
		public: using FallbackPool = ConfigAdaptor_::FallbackPool;
		public: static constexpr ::dcool::core::Size minSize = sizeof(void*);
		public: static constexpr ::dcool::core::Length listCount = 8;
		public: static constexpr ::dcool::core::Size maxSize = minSize << (listCount - 1);
		public: static constexpr ::dcool::core::Boolean fallbackPoolShared = ConfigAdaptor_::fallbackPoolShared;
		public: static constexpr ::dcool::core::Boolean synchronizationRequired = ConfigAdaptor_::synchronizationRequired;
		public: static constexpr ::dcool::core::Boolean quickAvailabilityCheck = ConfigAdaptor_::quickAvailabilityCheck;
		private: static constexpr ::dcool::core::Boolean availabiliyFlagRequired_ =
			synchronizationRequired && quickAvailabilityCheck
		;
		public: template <typename ConfigT__> friend class CachedClassicPool;

		private: struct UnderlyingIteration_ {
			constexpr auto operator ()(::dcool::core::Size current_) const noexcept -> ::dcool::core::Size {
				return current_ * 2;
			}
		};

		private: using ListMutex_ = ::dcool::core::ConditionalType<
			synchronizationRequired, ::dcool::core::MinimalMutex, ::dcool::core::Empty<>
		>;
		public: using FreeList = ::dcool::resource::ClassicFreeList;

		private: struct ListWithData_ {
			FreeList m_list_;
			[[no_unique_address]] ListMutex_ mutex_;
			[[no_unique_address]] ::dcool::core::StaticOptional<::std::atomic_flag, availabiliyFlagRequired_> available_;
		};

		private: using ChunkCache_ = ::std::array<ListWithData_, listCount>;

		private: static ::dcool::core::StaticOptional<FallbackPool, fallbackPoolShared> s_fallbackPool_;
		private: [[no_unique_address]] ::dcool::core::StaticOptional<FallbackPool, !fallbackPoolShared> m_fallbackPool_;
		private: ChunkCache_ m_chunkCache_ = {};

		public: static constexpr auto listIndexFor(::dcool::core::Size size_) noexcept -> ::dcool::core::Index {
			DCOOL_CORE_ASSERT(size_ <= maxSize);
			if (size_ <= minSize) {
				return 0;
			}
			::dcool::core::Size chunkSize_ = ::std::bit_ceil(size_);
			return ::std::countr_zero(chunkSize_ / minSize);
		}

		public: static constexpr auto chunkSizeAt(::dcool::core::Index index_) noexcept -> ::dcool::core::Size {
			return minSize << index_;
		}

		public: ~CachedClassicPool() noexcept {
			for (::dcool::core::Index i_ = 0; i_ < listCount; ++i_) {
				FreeList& list_ = this->m_chunkCache_[i_].list_;
				::dcool::core::Size chunkSize_ = chunkSizeAt(i_);
				if constexpr (
					requires (FreeList::HandOverHandle handle_) {
						this->fallbackPool().reclaimFreeListOfSize_(list_, chunkSize_);
					}
				) {
					if (!(list_.vacant())) {
						this->fallbackPool().reclaimFreeListOfSize_(list_, chunkSize_);
					}
				} else {
					this->destroyList_(list_, chunkSize_);
				}
			}
		}

		public: constexpr auto fallbackPool() const noexcept -> FallbackPool const& {
			if constexpr (fallbackPoolShared) {
				return this->s_fallbackPool_.value;
			} else {
				return this->m_fallbackPool_.value;
			}
		}

		public: constexpr auto fallbackPool() noexcept -> FallbackPool& {
			if constexpr (fallbackPoolShared) {
				return this->s_fallbackPool_.value;
			} else {
				return this->m_fallbackPool_.value;
			}
		}

		public: [[nodiscard("Might leak memory.")]] auto noThrowAllocate(::dcool::core::Size size_) noexcept -> void* {
			if (size_ <= maxSize) {
				::dcool::core::Index listIndex_ = listIndexFor(size_);
				if (::dcool::core::phoneyTryLock(this->m_chunkCache_[listIndex_].mutex_)) {
					void* result_;
					{
						::dcool::core::PhoneyLockGuard guard_(::dcool::core::adoptLock, this->m_cacheMutex_);
						result_ = this->m_chunkCache_[listIndex_].list_.pickUp(size_);
					}
					if (result_ != ::dcool::core::nullPointer) {
						return result_;
					}
				}
			}
			return this->fallbackPool().noThrowAllocate(size_);
		}

		public: [[nodiscard("Might leak memory.")]] auto allocate(::dcool::core::Size size_) -> void* {
			if (size_ <= maxSize) {
				::dcool::core::Index listIndex_ = listIndexFor(size_);
				if (::dcool::core::phoneyTryLock(this->m_chunkCache_[listIndex_].mutex_)) {
					void* result_;
					{
						::dcool::core::PhoneyLockGuard guard_(::dcool::core::adoptLock, this->m_cacheMutex_);
						result_ = this->m_chunkCache_[listIndex_].list_.pickUp(size_);
					}
					if (result_ != ::dcool::core::nullPointer) {
						return result_;
					}
				}
			}
			return this->fallbackPool().allocate(size_);
		}

		public: void deallocate(void* pointer_, ::dcool::core::Size size_) noexcept {
			if (size_ <= maxSize) {
				::dcool::core::Index listIndex_ = listIndexFor(size_);
				if (::dcool::core::phoneyTryLock(this->m_chunkCache_[listIndex_].mutex_)) {
					::dcool::core::PhoneyLockGuard guard_(::dcool::core::adoptLock, this->m_cacheMutex_);
					this->m_chunkCache_[listIndex_].list_.deposite(pointer_, size_);
					return;
				}
			}
			this->fallbackPool().deallocate(pointer_, size_);
		}

		private: void destroyList_(FreeList& toDestroy_, ::dcool::core::Size chunkSize_) {
			for (; ; ) {
				void* currentChunk_ = toDestroy_.pickUp();
				if (currentChunk_ == ::dcool::core::nullPointer) {
					break;
				}
				this->fallbackPool().deallocate(currentChunk_, chunkSize_);
			}
		}

		private: void reclaimFreeListOfSize_(FreeList& otherList_, ::dcool::core::Size chunkSize_) noexcept {
			DCOOL_CORE_ASSERT(!(otherList_.vacant()));
			FreeList::HandOverHandle handle_ = otherList_.handleToHandOver();
			if (chunkSize_ <= maxSize) {
				::dcool::core::Index selfListIndex_ = listIndexFor(chunkSize_);
				if (::dcool::core::phoneyTryLock(this->m_chunkCache_[selfListIndex_].mutex_)) {
					::dcool::core::PhoneyLockGuard guard_(::dcool::core::adoptLock, this->m_chunkCache_[selfListIndex_].mutex_);
					this->m_chunkCache_[selfListIndex_].list_.quickTakeOver(otherList_, handle_);
					return;
				}
			}
			this->destroyList_(otherList_, chunkSize_);
		}
	};

	namespace detail_ {
		template <typename ConfigT_> struct FastClassicPoolGlobalCachedConfig_ {
			static constexpr ::dcool::core::Boolean fallbackPoolShared = false;
			static constexpr ::dcool::core::Boolean synchronizationRequired = true;
		};

		template <typename ConfigT_> struct FastClassicPoolLocalCachedConfig_ {
			using FallbackPool = ::dcool::resource::CachedClassicPool<
				::dcool::resource::detail_::FastClassicPoolGlobalCachedConfig_<ConfigT_>
			>;
			static constexpr ::dcool::core::Boolean fallbackPoolShared = true;
			static constexpr ::dcool::core::Boolean synchronizationRequired = false;
		};
	}

	template <typename ConfigT_ = ::dcool::core::Empty<>> using FastSerialClassicPool = ::dcool::resource::CachedClassicPool<
		::dcool::resource::detail_::FastClassicPoolLocalCachedConfig_<ConfigT_>
	>;
}

#endif
