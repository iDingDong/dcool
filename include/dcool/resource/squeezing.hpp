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
	> struct ArraySqueezer {
		public: using Item = ItemT_;
		public: using Alternative = AlternativeT_;
		public: static constexpr ::dcool::core::Length squeezedCapacity = squeezedCapacityC_;

		public: using Items = Item[squeezedCapacity];

		private: union {
			Items items_;
			Alternative alternative_;
		} m_storage_;

		public: constexpr void activateItems() {
			new (::dcool::core::addressOf(this->m_storage_.items_)) Items;
		}

		public: constexpr void deactivateItems() noexcept {
			::dcool::core::destruct(this->items());
		}

		public: constexpr void activateAlternative() {
			new (::dcool::core::addressOf(this->m_storage_.alternative_)) Alternative;
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

		public: constexpr auto alternative() const noexcept -> Alternative const& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
		}

		public: constexpr auto alternative() noexcept -> Alternative& {
			return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
		}
	};

	template <typename ItemT_, typename AlternativeT_> struct ArraySqueezer<ItemT_, AlternativeT_, 0> {
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

	namespace detail_ {
		template <
			::dcool::core::Size sizeC_, ::dcool::core::Alignment alignmentC_, typename AlternativeT_
		> struct StorageSqueezer_ {
			private: using Self_ = StorageSqueezer_<sizeC_, alignmentC_, AlternativeT_>;
			public: using Alternative = AlternativeT_;

			public: static constexpr ::dcool::core::StorageRequirement itemRequirement = ::dcool::core::storageRequirement<
				sizeC_, alignmentC_
			>;

			public: using Item = ::dcool::core::AlignedStorage<itemRequirement>;

			private: union {
				Item item_;
				Alternative alternative_;
			} m_storage_;

			public: constexpr void activateItem() {
				new (::dcool::core::addressOf(this->m_storage_.item_)) Item;
			}

			public: constexpr void deactivateItem() noexcept {
				::dcool::core::destruct(this->item());
			}

			public: constexpr void activateAlternative() {
				new (::dcool::core::addressOf(this->m_storage_.alternative_)) Alternative;
			}

			public: constexpr void deactivateAlternative() noexcept {
				this->alternative().~Alternative();
			}

			public: constexpr auto item() const noexcept -> Item const& {
				return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.item_)));
			}

			public: constexpr auto item() noexcept -> Item& {
				return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.item_)));
			}

			public: constexpr auto alternative() const noexcept -> Alternative const& {
				return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
			}

			public: constexpr auto alternative() noexcept -> Alternative& {
				return ::dcool::core::dereference(::dcool::core::launder(::dcool::core::addressOf(this->m_storage_.alternative_)));
			}
		};

		template <
			::dcool::core::Alignment alignmentC_, typename AlternativeT_
		> struct StorageSqueezer_<0, alignmentC_, AlternativeT_> {
			private: using Self_ = StorageSqueezer_<0, alignmentC_, AlternativeT_>;
			public: using Alternative = AlternativeT_;

			public: static constexpr ::dcool::core::StorageRequirement itemRequirement = ::dcool::core::storageRequirement<0, 0>;

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

	template <
		::dcool::core::StorageRequirement requirementC_, typename AlternativeT_
	> using StorageSqueezer = ::dcool::resource::detail_::StorageSqueezer_<
		::dcool::core::size(requirementC_), ::dcool::core::alignment(requirementC_), AlternativeT_
	>;
}

#endif
