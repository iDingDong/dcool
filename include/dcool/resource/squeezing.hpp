#ifndef DCOOL_RESOURCE_SQUEEZING_HPP_INCLUDED_
#	define DCOOL_RESOURCE_SQUEEZING_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

namespace dcool::resource {
	constexpr auto defaultRelaxedSqueezedCapacity(
		::dcool::core::StorageRequirement requirement_, ::dcool::core::StorageRequirement available_
	) noexcept -> ::dcool::core::Length {
		if (::dcool::core::size(available_) % ::dcool::core::alignment(requirement_) != 0) {
			return 0;
		}
		return ::dcool::core::size(available_) / ::dcool::core::size(requirement_);
	}

	constexpr auto defaultSqueezedCapacity(
		::dcool::core::StorageRequirement requirement_, ::dcool::core::StorageRequirement available_
	) noexcept -> ::dcool::core::Length {
		if (::dcool::core::alignment(requirement_) > ::dcool::core::alignment(available_)) {
			return 0;
		}
		return ::dcool::resource::defaultRelaxedSqueezedCapacity(requirement_, available_);
	}

	template <
		typename ItemT_, typename StorageT_
	> constexpr ::dcool::core::Length defaultRelaxedSqueezedCapacityFor = ::dcool::resource::defaultRelaxedSqueezedCapacity(
		::dcool::core::storageRequirementFor<ItemT_>, ::dcool::core::storageRequirementFor<StorageT_>
	);

	template <
		typename ItemT_, typename StorageT_
	> constexpr ::dcool::core::Length defaultSqueezedCapacityFor = ::dcool::resource::defaultSqueezedCapacity(
		::dcool::core::storageRequirementFor<ItemT_>, ::dcool::core::storageRequirementFor<StorageT_>
	);

	template <
		typename ItemT_,
		typename AlternativeT_,
		::dcool::core::Length squeezedCapacityC_ = ::dcool::resource::defaultSqueezedCapacityFor<ItemT_, AlternativeT_>
	> struct Squeezer {
		public: using Item = ItemT_;
		public: using Alternative = AlternativeT_;
		public: static constexpr ::dcool::core::Length squeezedCapacity = squeezedCapacityC_;

		public: using Items = Item[squeezedCapacity];

		private: union {
			Items items_;
			Alternative alternative_;
		} m_storage_;

		public: constexpr void activateItems() {
			new (::dcool::core::addressOf(this->m_storage_.items_)) decltype(this->m_storage_.items_);
		}

		public: constexpr void deactivateItems() noexcept {
			this->items().~Items();
		}

		public: constexpr void activateAlternative() {
			new (::dcool::core::addressOf(this->m_storage_.alternative_)) decltype(this->m_storage_.alternative_);
		}

		public: constexpr void deactivateAlternative() noexcept {
			this->alternative().~Alternative();
		}

		public: constexpr auto items() const noexcept -> Items const& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.items_)));
		}

		public: constexpr auto items() noexcept -> Items& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.items_)));
		}

		public: constexpr auto alternative() const noexcept -> Items const& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
		}

		public: constexpr auto alternative() noexcept -> Items& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
		}
	};

	template <typename ItemT_, typename AlternativeT_> struct Squeezer<ItemT_, AlternativeT_, 0> {
		public: using Item = ItemT_;
		public: using Alternative = AlternativeT_;
		public: static constexpr ::dcool::core::Length squeezedCapacity = 0;

		private: Alternative m_alternative_;

		public: constexpr void activateItems() {
		}

		public: constexpr void deactivateItems() noexcept {
		}

		public: constexpr void activateAlternative() noexcept {
		}

		public: constexpr void deactivateAlternative() noexcept {
		}

		public: constexpr auto alternative() const -> Alternative const& {
			return m_alternative_;
		}

		public: constexpr auto alternative() -> Alternative& {
			return m_alternative_;
		}
	};
}

#endif
