#ifndef DCOOL_RESOURCE_CONCRETE_POOL_HPP_INCLUDED_
#	define DCOOL_RESOURCE_CONCRETE_POOL_HPP_INCLUDED_ 1

#	include <dcool/resource/pool.hpp>

#	include <dcool/core.hpp>

#	include <array>
#	include <limits>
#	include <stdexcept>

namespace dcool::resource {
	template <
		::dcool::core::Length unitCountC_, ::dcool::core::StorageRequirement unitStorageRequirementC_
	> class ConcretePinnedPool {
		private: using Self_ = ConcretePinnedPool<unitCountC_, unitStorageRequirementC_>;
		public: static constexpr ::dcool::core::Length unitCount = unitCountC_;
		public: static constexpr ::dcool::core::StorageRequirement unitStorageRequirement = unitStorageRequirementC_;

		public: using Length = ::dcool::core::IntegerType<unitCount>;
		public: using Index = Length;
		public: using UnifiedHandle = Index;
		public: using UnifiedConstHandle = Index;
		public: using Handle = UnifiedHandle;
		public: using ConstHandle = UnifiedConstHandle;
		private: using UnitStorage_ = ::dcool::core::AlignedStorage<unitStorageRequirementC_>;
		public: static constexpr ::dcool::core::Alignment maxAlignment = ::dcool::core::alignment(unitStorageRequirement);
		public: static constexpr ::dcool::core::Alignment defaultAlignment = ::dcool::core::alignment(unitStorageRequirement);

		static_assert(
			::dcool::core::isStorable<Length, unitStorageRequirementC_>,
			"'dcool::resource::ConcretePinnedPool' need chunk suffitient to hold an index."
		);

		private: ::std::array<UnitStorage_, unitCount> m_storage_;
		private: Index m_first_;

		public: constexpr ConcretePinnedPool() noexcept: m_first_(0) {
			for (Index i = 0; i < unitCount; ++i) {
				this->link_(i, i + 1);
			}
		}

		public: ConcretePinnedPool(Self_ const&) = delete;
		public: ConcretePinnedPool(Self_&&) = delete;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: constexpr auto nextAvailableOf_(Index index_) const noexcept -> Index {
			return reinterpret_cast<Index const&>(this->m_storage_[index_]);
		}

		private: constexpr void link_(Index leading_, Index following_) noexcept {
			reinterpret_cast<Index&>(this->m_storage_[leading_]) = following_;
		}

		public: constexpr auto exhausted() const noexcept {
			return this->m_first_ == unitCount;
		}

		public: template <
			::dcool::core::StorageRequirement storageRequirementC__
		> [[nodiscard("Might leak memory.")]] constexpr auto allocate() -> Handle {
			if constexpr (!::dcool::core::requiredStorable<storageRequirementC__, unitStorageRequirement>) {
				throw ::dcool::resource::BadAllocation();
			}
			if (this->exhausted()) {
				throw ::dcool::resource::BadAllocation();
			}
			Index result_ = this->m_first_;
			this->m_first_ = this->nextAvailableOf_(result_);
			return result_;
		}

		public: template <
			::dcool::core::StorageRequirement storageRequirementC__
		> constexpr void deallocate(Handle handle_) noexcept {
			if constexpr (!::dcool::core::requiredStorable<storageRequirementC__, unitStorageRequirement>) {
				::dcool::core::terminate();
			}
			this->link_(handle_, this->m_first_);
			this->m_first_ = handle_;
		}

		public: struct HandleConverter {
			Self_* pool_;

			constexpr auto operator ()(Handle index_) const noexcept -> void* {
				return ::dcool::core::addressOf(pool_->m_storage_[index_]);
			}

			constexpr auto operator ()(void const* pointer_) const noexcept -> Handle {
				return static_cast<Handle>(
					static_cast<UnitStorage_ const*>(pointer_) - (::dcool::core::addressOf(pool_->m_storage_[0]))
				);
			}
		};

		public: using ConstHandleConverter = HandleConverter;

		public: template <::dcool::core::StorageRequirement storageRequirementC__> constexpr auto handleConverter() noexcept {
			return HandleConverter(this);
		}

		public: template <::dcool::core::StorageRequirement storageRequirementC__> constexpr auto constHandleConverter() noexcept {
			return this->handleConverter<storageRequirementC__>();
		}

		public: friend constexpr auto operator ==(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Boolean {
			return ::dcool::core::addressOf(left_) == ::dcool::core::addressOf(right_);
		}

		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <
		::dcool::core::Length unitCountC_,
		::dcool::core::StorageRequirement unitStorageRequirementC_,
		typename DistinguisherT_ = void
	> class ConcreteReferencePool {
		private: using Self_ = ConcreteReferencePool<unitCountC_, unitStorageRequirementC_, DistinguisherT_>;
		public: static constexpr ::dcool::core::Length unitCount = unitCountC_;
		public: static constexpr ::dcool::core::StorageRequirement unitStorageRequirement = unitStorageRequirementC_;

		public: using Nexus = ConcretePinnedPool<unitCount, unitStorageRequirement>;
		public: using Index = Nexus::Index;
		public: using UnifiedHandle = Nexus::UnifiedHandle;
		public: using UnifiedConstHandle = Nexus::UnifiedConstHandle;
		public: using Handle = Nexus::Handle;
		public: using ConstHandle = Nexus::ConstHandle;
		public: using HandleConverter = Nexus::HandleConverter;
		public: using ConstHandleConverter = Nexus::ConstHandleConverter;
		public: static constexpr ::dcool::core::Alignment maxAlignment = Nexus::maxAlignment;
		public: static constexpr ::dcool::core::Alignment defaultAlignment = Nexus::defaultAlignment;

		private: Nexus* m_pinned_;

		public: constexpr ConcreteReferencePool() noexcept: Self_(::dcool::core::instance<Nexus, Self_>) {
		}

		public: constexpr ConcreteReferencePool(Nexus& pinned_) noexcept: m_pinned_(::dcool::core::addressOf(pinned_)) {
		}

		public: constexpr auto exhausted() const noexcept {
			return this->m_pinned_->exhausted();
		}

		public: template <
			::dcool::core::StorageRequirement storageRequirementC__
		> [[nodiscard("Might leak memory.")]] constexpr auto allocate() -> Handle {
			return this->m_pinned_->template allocate<storageRequirementC__>();
		}

		public: template <
			::dcool::core::StorageRequirement storageRequirementC__
		> constexpr void deallocate(Handle handle_) noexcept {
			return this->m_pinned_->template deallocate<storageRequirementC__>(handle_);
		}

		public: template <::dcool::core::StorageRequirement storageRequirementC__> constexpr auto handleConverter() noexcept {
			return this->m_pinned_->template handleConverter<storageRequirementC__>();
		}

		public: template <::dcool::core::StorageRequirement storageRequirementC__> constexpr auto constHandleConverter() noexcept {
			return this->m_pinned_->template constHandleConverter<storageRequirementC__>();
		}

		public: friend constexpr auto operator ==(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Boolean {
			return left_.m_pinned_ == right_.m_pinned_;
		}

		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};
}

#endif
