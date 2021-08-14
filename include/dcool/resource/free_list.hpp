#ifndef DCOOL_RESOURCE_FREE_LIST_HPP_INCLUDED_
#	define DCOOL_RESOURCE_FREE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <algorithm>
#	include <array>
#	include <bit>
#	include <cstddef>
#	include <cmath>

namespace dcool::resource {
	class ClassicFreeList {
		private: using Self_ = ClassicFreeList;

		private: using Coupling_ = void*;
		public: using HandOverHandle = Coupling_;
		private: using ConstCoupling_ = void const*;

		private: Coupling_ m_top_ = ::dcool::core::nullPointer;

		public: constexpr auto vacant() const noexcept -> ::dcool::core::Boolean {
			return this->m_top_ == ::dcool::core::nullPointer;
		}

		public: constexpr auto bravePickUp() noexcept -> void* {
			DCOOL_CORE_ASSERT(!(this->vacant()));
			void* result_ = this->m_top_;
			this->m_top_ = getNext_(this->m_top_);
			return result_;
		}

		private: static constexpr auto getNext_(ConstCoupling_ node_) noexcept -> Coupling_ {
			Coupling_ result_ = ::dcool::core::readFromBuffer<Coupling_>(node_);
			return result_;
		}

		private: static constexpr void setNext_(Coupling_ node_, Coupling_ newNext_) noexcept {
			::dcool::core::writeToBuffer(newNext_, node_);
		}

		public: constexpr auto pickUp() noexcept -> void* {
			if (this->m_top_ == ::dcool::core::nullPointer) {
				return ::dcool::core::nullPointer;
			}
			void* result_ = this->m_top_;
			this->m_top_ = getNext_(this->m_top_);
			return result_;
		}

		public: constexpr void deposite(void* toDeposite_) noexcept {
			Coupling_ newNode_ = reinterpret_cast<Coupling_>(toDeposite_);
			setNext_(newNode_, this->m_top_);
			this->m_top_ = newNode_;
		}

		public: constexpr auto handleToHandOver() const noexcept -> HandOverHandle {
			DCOOL_CORE_ASSERT(!(this->vacant()));
			Coupling_ current_ = this->m_top_;
			for (; ; ) {
				Coupling_ next_ = getNext_(current_);
				if (next_ == ::dcool::core::nullPointer) {
					break;
				}
				current_ = next_;
			}
			return current_;
		}

		public: constexpr void quickTakeOver(Self_& other_, Coupling_ handle_) noexcept {
			DCOOL_CORE_ASSERT(!(other_.vacant()));
			DCOOL_CORE_ASSERT(handle_ != ::dcool::core::nullPointer);
			setNext_(handle_, this->m_top_);
			this->m_top_ = other_.m_top_;
			other_.m_top_ = ::dcool::core::nullPointer;
		}
	};

	template <::dcool::core::StorageRequirement storageRequirementC_> class FreeList {
		private: using Self_ = FreeList<storageRequirementC_>;
		public: static constexpr ::dcool::core::StorageRequirement storageReqirement = storageRequirementC_;

		private: using Coupling_ = ::dcool::core::AlignedStorage<storageReqirement>*;
		public: using HandOverHandle = Coupling_;
		private: using ConstCoupling_ = ::dcool::core::AlignedStorage<storageReqirement> const*;
		static_assert(::dcool::core::storable<Coupling_>(storageReqirement));

		private: Coupling_ m_top_ = ::dcool::core::nullPointer;

		private: static constexpr auto getNext_(ConstCoupling_ node_) noexcept -> Coupling_ {
			Coupling_ result_ = ::dcool::core::readFromBuffer<Coupling_>(node_);
			return result_;
		}

		private: static constexpr void setNext_(Coupling_ node_, Coupling_ newNext_) noexcept {
			::dcool::core::writeToBuffer(newNext_, node_);
		}

		public: constexpr auto vacant() const noexcept -> ::dcool::core::Boolean {
			return this->m_top_ == ::dcool::core::nullPointer;
		}

		public: constexpr auto bravePickUp() noexcept -> void* {
			DCOOL_CORE_ASSERT(!(this->vacant()));
			void* result_ = this->m_top_;
			this->m_top_ = getNext_(this->m_top_);
			return result_;
		}

		public: constexpr auto pickUp() noexcept -> void* {
			return this->bravePickUp();
		}

		public: constexpr void deposite(void* toDeposite_) noexcept {
			Coupling_ newNode_ = reinterpret_cast<Coupling_>(toDeposite_);
			setNext_(newNode_, this->m_top_);
			this->m_top_ = newNode_;
		}

		public: constexpr auto handleToHandOver() const noexcept -> HandOverHandle {
			DCOOL_CORE_ASSERT(!(this->vacant()));
			Coupling_ current_ = this->m_top_;
			for (; ; ) {
				Coupling_ next_ = getNext_(current_);
				if (next_ == ::dcool::core::nullPointer) {
					break;
				}
				current_ = next_;
			}
			return current_;
		}

		public: constexpr void quickTakeOver(Self_& other_, Coupling_ handle_) noexcept {
			DCOOL_CORE_ASSERT(!(other_.vacant()));
			DCOOL_CORE_ASSERT(handle_ != ::dcool::core::nullPointer);
			setNext_(handle_, this->m_top_);
			this->m_top_ = other_.m_top_;
			other_.m_top_ = ::dcool::core::nullPointer;
		}
	};

	namespace detail_ {
		template <
			::dcool::core::StorageRequirement... requirementCs_
		> struct FreeListSetUnderlying_: public FreeList<requirementCs_>... {
		};

		template <::dcool::core::ValueListOf<::dcool::core::StorageRequirement> RequirementsT_> struct FreeListSetUnderlyingHelper_;

		template <
			::dcool::core::StorageRequirement... requirementCs_
		> struct FreeListSetUnderlyingHelper_<::dcool::core::Values<requirementCs_...>> {
			using Result_ = ::dcool::resource::detail_::FreeListSetUnderlying_<requirementCs_...>;
		};

		template <::dcool::core::StorageRequirement... availableCs_> struct FreeListSelector_;

		template <
			::dcool::core::StorageRequirement firstAvailableC_,
			::dcool::core::StorageRequirement... restAvailableCs_
		> struct FreeListSelector_<firstAvailableC_, restAvailableCs_...> {
			template <::dcool::core::StorageRequirement requiredC_, typename FreeListSetUnderlyingT__> static constexpr auto pickUp_(
				FreeListSetUnderlyingT__& set_
			) noexcept -> void* {
				if constexpr (::dcool::core::requiredStorable(requiredC_, firstAvailableC_)) {
					return static_cast<::dcool::resource::FreeList<firstAvailableC_>&>(set_).pickUp();
				}
				return ::dcool::resource::detail_::FreeListSelector_<restAvailableCs_...>::template pickUp_<requiredC_>(set_);
			}

			template <
				::dcool::core::StorageRequirement requiredC_, typename FreeListSetUnderlyingT__
			> static constexpr void deposite_(
				FreeListSetUnderlyingT__& set_, void* toDeposite_
			) noexcept {
				if constexpr (::dcool::core::requiredStorable(requiredC_, firstAvailableC_)) {
					static_cast<::dcool::resource::FreeList<firstAvailableC_>&>(set_).deposite(toDeposite_);
				}
				::dcool::resource::detail_::FreeListSelector_<restAvailableCs_...>::template deposite_<requiredC_>(set_, toDeposite_);
			}
		};

		template <typename RequirementListT_> struct FreeListSelectorHelper_;

		template <::dcool::core::StorageRequirement... availableCs_> struct FreeListSelectorHelper_<
			::dcool::core::Values<availableCs_...>
		> {
			using Result_ = ::dcool::resource::detail_::FreeListSelector_<availableCs_...>;
		};
	}

	template <::dcool::core::StorageRequirement... requirementCs_> class FreeListSet {
		private: using Self_ = FreeListSet<requirementCs_...>;

		private: using Underlying_ = ::dcool::resource::detail_::FreeListSetUnderlying_<requirementCs_...>;
		private: using SelectHelper_ = ::dcool::resource::detail_::FreeListSelector_<requirementCs_...>;

		private: Underlying_ m_underlying_;

		public: template <::dcool::core::StorageRequirement requiredC_> constexpr auto pickUp() noexcept -> void* {
			return SelectHelper_::template pickUp_<requiredC_>(this->m_underlying_);
		}

		public: template <::dcool::core::StorageRequirement requiredC_> constexpr void deposite(void* toDeposite_) noexcept {
			SelectHelper_::template deposite_<requiredC_>(this->m_underlying_, toDeposite_);
		}
	};

	template <typename ConfigT_ = ::dcool::core::Empty<>> class ExponentialFreeListSet {
		private: using Self_ = ExponentialFreeListSet<ConfigT_>;
		public: using Config = ConfigT_;

		public: static constexpr ::dcool::core::StorageRequirement minStorable = ::dcool::core::storageRequirementFor<
			::std::max_align_t
		>;
		public: static constexpr ::dcool::core::Length exponent = 2;
		public: static constexpr ::dcool::core::Length listCount = 8;

		private: struct UnderlyingIteration_ {
			constexpr auto operator ()(
				::dcool::core::StorageRequirement current_
			) const noexcept -> ::dcool::core::StorageRequirement {
				return ::dcool::core::makeStorageRequirement(
					::dcool::core::size(current_) * exponent, ::dcool::core::alignment(current_)
				);
			}
		};

		private: using AvailableList_ = ::dcool::core::ValueSequence<
			minStorable, ::dcool::core::create<UnderlyingIteration_>(), listCount
		>;
		private: using SelectHelper_ = ::dcool::resource::detail_::FreeListSelectorHelper_<AvailableList_>::Result_;
		private: using Underlying_ = ::dcool::resource::detail_::FreeListSetUnderlyingHelper_<AvailableList_>::Result_;
		public: static constexpr ::dcool::core::StorageRequirement maxStorable = ::dcool::core::applyNIteration(
			minStorable, UnderlyingIteration_(), listCount
		);

		private: Underlying_ m_underlying_;

		public: template <::dcool::core::StorageRequirement requiredC_> constexpr auto pickUp() noexcept -> void* {
			return SelectHelper_::template pickUp_<requiredC_>(this->m_underlying_);
		}

		public: template <::dcool::core::StorageRequirement requiredC_> constexpr void deposite(void* toDeposite_) noexcept {
			SelectHelper_::template deposite_<requiredC_>(this->m_underlying_, toDeposite_);
		}
	};
}

#endif
