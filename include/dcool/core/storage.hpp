#ifndef DCOOL_CORE_STORAGE_HPP_INCLUDED_
#	define DCOOL_CORE_STORAGE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <limits>
#	include <memory>

namespace dcool::core {
	constexpr ::dcool::core::Alignment defaultNewAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	constexpr ::dcool::core::Alignment minRepresentableAlignment = 1;
	constexpr ::dcool::core::Alignment maxRepresentableAlignment =
		minRepresentableAlignment << (::std::numeric_limits<::dcool::core::Alignment>::digits - 1)
	;
	template <::dcool::core::Size sizeC_> constexpr ::dcool::core::Alignment defaultAlignmentFor = alignof(
		::std::aligned_storage_t<sizeC_>
	);

#	if defined(__GNUC__)
	// Workaround for a compiler bug.
	// See document/dependency_bugs#Bug_1 for mor details.
	__extension__ using StorageRequirement = unsigned __int128;

	constexpr auto size(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Size {
		return static_cast<::dcool::core::Size>(requirement_);
	}

	constexpr auto alignment(::dcool::core::StorageRequirement requirement_) -> ::dcool::core::Alignment {
		return static_cast<::dcool::core::Alignment>(requirement_ >> 64);
	}

	template <
		::dcool::core::Size sizeC_,
		::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> constexpr ::dcool::core::StorageRequirement storageRequirement =
		static_cast<::dcool::core::StorageRequirement>(sizeC_) | (static_cast<::dcool::core::StorageRequirement>(alignmentC_) << 64)
	;
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

	template <
		::dcool::core::Size sizeC_,
		::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> constexpr ::dcool::core::StorageRequirement storageRequirement = ::dcool::core::StorageRequirement {
		.size = sizeC_, .alignmentC_ = alignmentC_
	};
#	endif

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
}

#endif
