#ifndef DCOOL_CORE_STORAGE_HPP_INCLUDED_
#	define DCOOL_CORE_STORAGE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <algorithm>
#	include <bit>
#	include <limits>
#	include <memory>

namespace dcool::core {
	inline constexpr ::dcool::core::Alignment defaultNewAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	inline constexpr ::dcool::core::Alignment minRepresentableAlignment = 1;
	inline constexpr ::dcool::core::Alignment maxRepresentableAlignment = std::bit_floor(
		::std::numeric_limits<::dcool::core::Alignment>::max()
	);
	template <::dcool::core::Size sizeC_> constexpr ::dcool::core::Alignment defaultAlignmentFor = alignof(
		::std::aligned_storage_t<sizeC_>
	);

	inline auto aligned(void* pointer_, ::dcool::core::Alignment required_) noexcept -> ::dcool::core::Boolean {
		::std::size_t assumedSize_ = 1;
		return ::std::align(required_, 1, pointer_, assumedSize_) != ::dcool::core::nullPointer;
	}

#	if DCOOL_DEPENDENCY_BUG_1
	// Workaround for a compiler bug.
	// See document/dependency_bugs#Bug_1 for more details.
	__extension__ using StorageRequirement = unsigned __int128;

	constexpr auto size(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Size {
		return static_cast<::dcool::core::Size>(requirement_);
	}

	constexpr auto alignment(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Alignment {
		return static_cast<::dcool::core::Alignment>(requirement_ >> 64);
	}

	constexpr auto makeStorageRequirement(::dcool::core::Size size_, ::dcool::core::Alignment alignment_) noexcept {
		return 
			static_cast<::dcool::core::StorageRequirement>(size_) | (static_cast<::dcool::core::StorageRequirement>(alignment_) << 64)
		;
	}
#	else
	struct StorageRequirement {
		private: using Self_ = StorageRequirement;

		public: ::dcool::core::Size size;
		public: ::dcool::core::Alignment alignment;

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	constexpr auto size(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Size {
		return requirement_.size;
	}

	constexpr auto alignment(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Alignment {
		return requirement_.alignment;
	}

	constexpr auto makeStorageRequirement(::dcool::core::Size size_, ::dcool::core::Alignment alignment_) noexcept {
		return ::dcool::core::StorageRequirement {
			.size = size_,
			.alignment = alignment_
		};
	}
#	endif

	template <
		::dcool::core::Size sizeC_,
		::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> constexpr ::dcool::core::StorageRequirement storageRequirement = ::dcool::core::makeStorageRequirement(sizeC_, alignmentC_);

	template <
		typename ValueT_
	> constexpr ::dcool::core::StorageRequirement storageRequirementFor = ::dcool::core::storageRequirement<
		sizeof(ValueT_), alignof(ValueT_)
	>;

	template <> constexpr ::dcool::core::StorageRequirement storageRequirementFor<void> = ::dcool::core::storageRequirement<0, 0>;

	constexpr auto requiredStorable(
		::dcool::core::StorageRequirement required_, ::dcool::core::StorageRequirement available_
	) noexcept -> ::dcool::core::Boolean {
		return
			::dcool::core::size(required_) <= ::dcool::core::size(available_) &&
			::dcool::core::alignment(required_) <= ::dcool::core::alignment(available_)
		;
	}

	template <typename T_> constexpr auto storable(
		::dcool::core::StorageRequirement available_
	) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::requiredStorable(storageRequirementFor<T_>, available_);
	}

	template <
		typename T_, ::dcool::core::StorageRequirement availableC_
	> constexpr ::dcool::core::Boolean isStorable = ::dcool::core::storable<T_>(availableC_);

	template <
		typename T_, ::dcool::core::StorageRequirement availableC_
	> concept Storable = isStorable<T_, availableC_>;

	namespace detail_ {
		template <::dcool::core::Size sizeC_, ::dcool::core::Alignment alignmentC_> struct AlignedStorageHelper_ {
			using Type_ = ::std::aligned_storage_t<sizeC_, alignmentC_>;
		};

		template <> struct AlignedStorageHelper_<0, 0> {
			struct Type_ {
			};
		};
	}

	template <
		::dcool::core::StorageRequirement storageRequirementC_
	> using AlignedStorage = ::dcool::core::detail_::AlignedStorageHelper_<
		::dcool::core::size(storageRequirementC_), ::dcool::core::alignment(storageRequirementC_)
	>::Type_;

	template <typename ValueT_> using StorageFor = ::dcool::core::AlignedStorage<
		::dcool::core::storageRequirementFor<ValueT_>
	>;

	namespace detail_ {
		template <typename ValueT_> struct StorableTypeHelper_ {
			using Result_ = ValueT_;
		};

		template <::dcool::core::Reference ReferenceT_> struct StorableTypeHelper_<ReferenceT_> {
			using Result_ = ::dcool::core::ReferenceRemovedType<ReferenceT_>*;
		};

		template <> struct StorableTypeHelper_<void> {
			using Result_ = ::dcool::core::Empty<>;
		};
	}

	template <typename ValueT_> using StorableType = ::dcool::core::detail_::StorableTypeHelper_<ValueT_>::Result_;

	template <typename ValueT_> struct StorageAgent {
		public: using Value = ValueT_;

		private: using Underlying_ = ::dcool::core::StorableType<ValueT_>;
		private: using UnderlyingStorage_ = ::dcool::core::StorageFor<Underlying_>;

		private: UnderlyingStorage_ m_underlyingStorage_;

		private: void emplaceReference_(Value value_) noexcept {
			new (::dcool::core::addressOf(this->m_underlyingStorage_)) Underlying_(::dcool::core::addressOf(value_));
		}

		public: template <typename... ArgumentTs__> void emplace(ArgumentTs__&&... arguments_) {
			if constexpr (::dcool::core::Reference<Value>) {
				this->emplaceReference_(arguments_...);
			} else {
				new (::dcool::core::addressOf(this->m_underlyingStorage_)) Value(::dcool::core::forward<ArgumentTs__>(arguments_)...);
			}
		}

		public: constexpr auto value() const -> Value {
			auto underlying_ = ::dcool::core::launder(
				reinterpret_cast<Underlying_*>(::dcool::core::addressOf(this->m_underlyingStorage_))
			);
			if constexpr (::dcool::core::Reference<Value>) {
				return static_cast<Value>(**underlying_);
			} else {
				return ::dcool::core::move(*underlying_);
			}
		}

		public: constexpr auto fetch() const -> Value {
			if constexpr (::dcool::core::Reference<Value>) {
				return this->value();
			} else {
				Value result_ = ::dcool::core::move(
					*::dcool::core::launder(reinterpret_cast<Value*>(::dcool::core::addressOf(this->m_underlyingStorage_)))
				);
				this->destruct();
				return result_;
			}
		}

		public: void destruct() noexcept {
			reinterpret_cast<Value&>(this->m_underlyingStorage_);
		}
	};

	template <typename T_> constexpr ::dcool::core::Boolean storableInUniversalPointer = false;
	template <
		::dcool::core::Pointer T_
	> constexpr ::dcool::core::Boolean storableInUniversalPointer<T_> = ::dcool::core::ConvertibleTo<T_, void const volatile*>;
	template <::dcool::core::Reference T_> constexpr ::dcool::core::Boolean storableInUniversalPointer<T_> = true;

	template <typename T_> concept StorableInUniversalPointer = ::dcool::core::storableInUniversalPointer<T_>;

	template <::dcool::core::StorableInUniversalPointer T_> constexpr auto packAsUniversalPointer(void* source_) noexcept -> T_ {
		if constexpr (::dcool::core::Pointer<T_>) {
			return static_cast<T_>(source_);
		}
		return static_cast<T_>(*::dcool::core::packAsUniversalPointer<::dcool::core::PointerAddedType<T_>>());
	}

	template <typename T_> constexpr auto unpackFromUniversalPointer(::dcool::core::UndeducedType<T_> source_) noexcept -> void* {
		if constexpr (::dcool::core::Pointer<T_>) {
			using ObjectType_ = ::dcool::core::PointerRemovedType<T_>;
			return const_cast<::dcool::core::PointerAddedType<::dcool::core::QualifierRemovedType<ObjectType_>>>(source_);
		} else {
			return ::dcool::core::unpackFromUniversalPointer<::dcool::core::PointerAddedType<T_>>(::dcool::core::addressOf(source_));
		}
	}

	constexpr auto padSizeToMatchAligment(
		::dcool::core::Size size_, ::dcool::core::Alignment alignmentToMatch_
	) noexcept -> ::dcool::core::Size {
		::dcool::core::Size remainning_ = size_ % alignmentToMatch_;
		return remainning_ > 0 ? alignmentToMatch_ - remainning_ : 0;
	}

	constexpr auto matchAlignment(
		::dcool::core::Size size_, ::dcool::core::Alignment alignmentToMatch_
	) noexcept -> ::dcool::core::Size {
		return size_ + ::dcool::core::padSizeToMatchAligment(size_, alignmentToMatch_);
	}

	template <
		::dcool::core::ConvertibleTo<::dcool::core::StorageRequirement>... Ts_
	> constexpr auto commonStorageRequirement(
		::dcool::core::StorageRequirement first_, Ts_... rests_
	) noexcept -> ::dcool::core::StorageRequirement {
		if constexpr (sizeof...(rests_) > 0) {
			::dcool::core::StorageRequirement subResult_ = ::dcool::core::commonStorageRequirement(rests_...);
			::dcool::core::Alignment alignment_ = ::std::max(
				::dcool::core::alignment(first_), ::dcool::core::alignment(subResult_)
			);
			::dcool::core::Size size_ = ::dcool::core::matchAlignment(
				::std::max(::dcool::core::size(first_), ::dcool::core::size(subResult_)), alignment_
			);
			return ::dcool::core::makeStorageRequirement(size_, alignment_);
		}
		return first_;
	}

	template <
		::dcool::core::StorageRequirement... requirementCs_
	> using CommonStorage = ::dcool::core::AlignedStorage<::dcool::core::commonStorageRequirement(requirementCs_...)>;

	template <typename... ValueTs_> using CommonStorageFor = ::dcool::core::CommonStorage<
		::dcool::core::storageRequirementFor<ValueTs_>...
	>;
}

#endif
