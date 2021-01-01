#ifndef DCOOL_CORE_POOL_HPP_INCLUDED_
#	define DCOOL_CORE_POOL_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/converter.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/storage.hpp>
#	include <dcool/core/type_difference_detector.hpp>
#	include <dcool/core/type_size_detector.hpp>
#	include <dcool/core/type_value_detector.hpp>
#	include <dcool/core/min.hpp>

#	include <memory>
#	include <new>
#	include <stdexcept>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeUnifiedHandle_, ExtractedUnifiedHandleType_, UnifiedHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeUnifiedConstHandle_, ExtractedUnifiedConstHandleType_, UnifiedConstHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeUnifiedArrayHandle_, ExtractedUnifiedArrayHandleType_, UnifiedArrayHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeUnifiedArrayConstHandle_, ExtractedUnifiedArrayConstHandleType_, UnifiedArrayConstHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::memory::detail_, HasTypeHandle_, ExtractedHandleType_, Handle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::memory::detail_, HasTypeConstHandle_, ExtractedConstHandleType_, ConstHandle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::memory::detail_, HasTypeArrayHandle_, ExtractedArrayHandleType_, ArrayHandle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeArrayConstHandle_, ExtractedArrayConstHandleType_, ArrayConstHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeHandleConverter_, ExtractedHandleConverterType_, HandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeConstHandleConverter_, ExtractedConstHandleConverterType_, ConstHandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeArrayHandleConverter_, ExtractedArrayHandleConverterType_, ArrayHandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::memory::detail_, HasTypeArrayConstHandleConverter_, ExtractedArrayConstHandleConverterType_, ArrayConstHandleConverter
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::memory::detail_, HasValueMaxAlignment_, extractedMaxAlignmentValue_, maxAligment
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::memory::detail_, HasValueDefaultAlignment_, extractedDefaultAlignmentValue_, defaultAligment
)

namespace dcool::memory {
	using BadAllocation = ::std::bad_alloc;

	template <typename T_> concept ClassicPool = requires (
		T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
	) {
		pointer_ = pool_.allocate(size_);
		pool_.deallocate(pointer_, size_);
	};

	namespace detail_ {
		template <typename T_> concept AlignedAllocateableClassicPool_ = ::dcool::memory::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
		) {
			pointer_ = pool_.allocate(size_, alignment_);
		};

		template <::dcool::memory::ClassicPool PoolT_> auto alignedAllocate_(
			PoolT_& pool_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) -> void* {
			return pool_.allocate(size_);
		}

		template <::dcool::memory::detail_::AlignedAllocateableClassicPool_ PoolT_> auto alignedAllocate_(
			PoolT_& pool_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) -> void* {
			return pool_.allocate(size_, alignment_);
		}

		template <typename T_> concept AlignedDeallocateableClassicPool_ = ::dcool::memory::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_, alignment_);
		};

		template <::dcool::memory::ClassicPool PoolT_> void alignedDeallocate_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_);
		}

		template <::dcool::memory::detail_::AlignedDeallocateableClassicPool_ PoolT_> void alignedDeallocate_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_, alignment_);
		}

		template <typename T_> concept FrontExpandableClassicPool_ = ::dcool::memory::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::SizeType<T_> extra_
		) {
			{ pool_.expandFront(pointer_, size_, extra_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <::dcool::memory::ClassicPool PoolT_> constexpr auto expandFront_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return false;
		}

		template <::dcool::memory::detail_::FrontExpandableClassicPool_ PoolT_> constexpr auto expandFront_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.expandFront(pointer_, size_, extra_);
		}

		template <typename T_> concept BackExpandableClassicPool_ = ::dcool::memory::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::SizeType<T_> extra_
		) {
			{ pool_.expandBack(pointer_, size_, extra_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <::dcool::memory::ClassicPool PoolT_> constexpr auto expandBack_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return false;
		}

		template <::dcool::memory::detail_::BackExpandableClassicPool_ PoolT_> constexpr auto expandBack_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.expandBack(pointer_, size_, extra_);
		}
	}

	template <::dcool::memory::ClassicPool PoolT_> class ClassicPoolAdaptor {
		private: using Self_ = ClassicPoolAdaptor<PoolT_>;
		public: using Pool = PoolT_;

		public: using Difference = ::dcool::core::DifferenceType<PoolT_>;
		public: using Size = ::dcool::core::SizeType<PoolT_>;

		public: static constexpr ::dcool::core::Alignment maxAlignment = ::dcool::memory::detail_::extractedMaxAlignmentValue_<Pool>(
			::dcool::core::defaultNewAlignment
		);
		public: static constexpr ::dcool::core::Alignment defaultAlignment =
			::dcool::memory::detail_::extractedDefaultAlignmentValue_<Pool>(
				::dcool::core::min(maxAlignment, ::dcool::core::defaultNewAlignment)
			)
		;

		static_assert(defaultAlignment <= maxAlignment);

		public: static constexpr auto allocate(Pool& pool_, Size size_) -> void* {
			return pool_.allocate(size_);
		}

		public: static constexpr auto allocate(Pool& pool_, Size size_, ::dcool::core::Alignment alignment_) -> void* {
			return ::dcool::memory::detail_::alignedAllocate_(pool_, size_, alignment_);
		}

		public: static constexpr void deallocate(Pool& pool_, void* pointer_, Size size_) noexcept {
			pool_.deallocate(pointer_, size_);
		}

		public: static constexpr void deallocate(
			Pool& pool_, void* pointer_, Size size_, ::dcool::core::Alignment alignemt_
		) noexcept {
			::dcool::memory::detail_::alignedDeallocate_(pool_, pointer_, size_, alignemt_);
		}

		public: static constexpr auto expandFront(
			Pool& pool_, void* pointer_, Size size_, Size extra_
		) noexcept -> ::dcool::core::Boolean {
			return ::dcool::memory::detail_::expandFront_(pool_, pointer_, size_, extra_);
		}

		public: static constexpr auto expandBack(
			Pool& pool_, void* pointer_, Size size_, Size extra_
		) noexcept -> ::dcool::core::Boolean {
			return ::dcool::memory::detail_::expandBack_(pool_, pointer_, size_, extra_);
		}
	};

	class DefaultClassicPool {
		private: using Self_ = DefaultClassicPool;

		public: using Size = ::dcool::core::Size;

		public: static constexpr ::dcool::core::Alignment maxAlignment = ::dcool::core::maxRepresentableAlignment;
		public: static constexpr ::dcool::core::Alignment defaultAlignment = ::dcool::core::defaultNewAlignment;

		public: auto allocate(Size size_) -> void* {
			return ::operator new(size_);
		}

		public: auto allocate(Size size_, ::dcool::core::Alignment alignment_) -> void* {
			return ::operator new(size_, ::std::align_val_t(alignment_));
		}

		public: void deallocate(void* pointer_, Size size_) noexcept {
			::operator delete(pointer_, size_);
		}

		public: void deallocate(void* pointer_, Size size_, ::dcool::core::Alignment alignment_) noexcept {
			::operator delete(pointer_, size_, ::std::align_val_t(alignment_));
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <typename T_> using UnifiedHandleType = ::dcool::memory::detail_::ExtractedUnifiedHandleType_<T_, void>;
	template <typename T_> using UnifiedConstHandleType = ::dcool::memory::detail_::ExtractedUnifiedConstHandleType_<T_, void>;
	template <typename T_> using UnifiedArrayHandleType = ::dcool::memory::detail_::ExtractedUnifiedArrayHandleType_<T_, void>;
	template <
		typename T_
	> using UnifiedArrayConstHandleType = ::dcool::memory::detail_::ExtractedUnifiedArrayConstHandleType_<T_, void>;

	namespace detail_ {
		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolHandleType_ = ::dcool::memory::detail_::ExtractedHandleType_<
			PoolT_, ::dcool::memory::detail_::ExtractedUnifiedHandleType_<
				PoolT_, decltype(::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>())
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolConstHandleType_ = ::dcool::memory::detail_::ExtractedConstHandleType_<
			PoolT_,
			::dcool::memory::detail_::ExtractedUnifiedConstHandleType_<
				PoolT_, ::dcool::core::PointedConstantizedType<::dcool::memory::detail_::PoolHandleType_<PoolT_, storageRequirementC_>>
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolHandleType_ = ::dcool::memory::detail_::ExtractedArrayHandleType_<
			PoolT_, ::dcool::memory::detail_::ExtractedUnifiedArrayHandleType_<
				PoolT_, decltype(
					::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>(
						::dcool::core::declval<::dcool::core::SizeType<PoolT_>>()
					)
				)
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolConstHandleType_ = ::dcool::memory::detail_::ExtractedArrayConstHandleType_<
			PoolT_,
			::dcool::memory::detail_::ExtractedUnifiedArrayConstHandleType_<
				PoolT_,
				::dcool::core::PointedConstantizedType<::dcool::memory::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>>
			>
		>;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithConverter_ = requires (T_ pool_) {
			{ pool_.template handleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_>, void*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithConstConverter_ = requires (T_ pool_) {
			{ pool_.template constHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::memory::detail_::PoolConstHandleType_<T_, storageRequirementC_>, void const*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithConverter_ = requires (T_ pool_) {
			{ pool_.template arrayHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>, void*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithConstConverter_ = requires (T_ pool_) {
			{ pool_.template arrayConstHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::memory::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>, void const*
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct PoolHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::PoolHandleType_<PoolT_, storageRequirementC_>, void*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::PoolWithConverter_<storageRequirementC_> PoolT_
		> struct PoolHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template handleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolHandleConverterType_ = ::dcool::memory::detail_::ExtractedHandleConverterType_<
			PoolT_, typename ::dcool::memory::detail_::PoolHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct PoolConstHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::PoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::PoolWithConstConverter_<storageRequirementC_> PoolT_
		> struct PoolConstHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template constHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolConstHandleConverterType_ = ::dcool::memory::detail_::ExtractedConstHandleConverterType_<
			PoolT_, typename ::dcool::memory::detail_::PoolConstHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct ArrayPoolHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>, void*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::ArrayPoolWithConverter_<storageRequirementC_> PoolT_
		> struct ArrayPoolHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template arrayHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolHandleConverterType_ = ::dcool::memory::detail_::ExtractedArrayHandleConverterType_<
			PoolT_, typename ::dcool::memory::detail_::ArrayPoolHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct ArrayPoolConstHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::ArrayPoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::ArrayPoolWithConstConverter_<storageRequirementC_> PoolT_
		> struct ArrayPoolConstHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template arrayConstHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolConstHandleConverterType_ = ::dcool::memory::detail_::ExtractedArrayConstHandleConverterType_<
			PoolT_, typename ::dcool::memory::detail_::ArrayPoolConstHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithProperUnifiedHandle_ =
			::dcool::core::OneOf<
				::dcool::memory::UnifiedHandleType<T_>, ::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_>, void
			> && ::dcool::core::OneOf<
				::dcool::memory::UnifiedConstHandleType<T_>, ::dcool::memory::detail_::PoolConstHandleType_<T_, storageRequirementC_>, void
			>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithProperlyConvertibleHandle_ =
			::dcool::core::ConvertibleTo<
				::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_>,
				::dcool::memory::detail_::PoolConstHandleType_<T_, storageRequirementC_>
			> && (
				::dcool::memory::detail_::PoolWithConverter_<T_, storageRequirementC_> ||
				::dcool::core::ConvertibleBetween<::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_>, void*>
			)
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperUnifiedHandle_ =
			::dcool::core::OneOf<
				::dcool::memory::UnifiedArrayHandleType<T_>, ::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>, void
			> && ::dcool::core::OneOf<
				::dcool::memory::UnifiedArrayConstHandleType<T_>,
				::dcool::memory::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>,
				void
			>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperlyConvertibleHandle_ =
			::dcool::core::ConvertibleTo<
				::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>,
				::dcool::memory::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>
			> && (
				::dcool::memory::detail_::ArrayPoolWithConverter_<T_, storageRequirementC_> ||
				::dcool::core::ConvertibleBetween<::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>, void*>
			)
		;

		template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept PoolWithProperHandle_ =
			::dcool::memory::detail_::PoolWithProperUnifiedHandle_<T_, storageRequirementC_> &&
			::dcool::memory::detail_::PoolWithProperlyConvertibleHandle_<T_, storageRequirementC_>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperHandle_ =
			::dcool::memory::detail_::ArrayPoolWithProperUnifiedHandle_<T_, storageRequirementC_> &&
			::dcool::memory::detail_::ArrayPoolWithProperlyConvertibleHandle_<T_, storageRequirementC_>
		;
	}

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept Pool = ::dcool::memory::detail_::PoolWithProperHandle_<T_, storageRequirementC_> && requires (
		T_ pool_, ::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_> handle_
	) {
		handle_ = pool_.template allocate<storageRequirementC_>();
		pool_.template deallocate<storageRequirementC_>(handle_);
	};

	template <typename T_, typename ValueT_> concept PoolFor = ::dcool::memory::Pool<T_, ::dcool::core::storageRequirementFor<T_>>;

	template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept ArrayPool =
		::dcool::memory::detail_::ArrayPoolWithProperHandle_<T_, storageRequirementC_> &&
		::dcool::memory::Pool<T_, storageRequirementC_> &&
		requires (
			T_ pool_,
			::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_> handle_,
			::dcool::core::LengthType<T_> length_
		)	{
			handle_ = pool_.template allocate<storageRequirementC_>(length_);
			pool_.template deallocate<storageRequirementC_>(handle_, length_);
		}
	;

	template <typename T_, typename ValueT_> concept ArrayPoolFor = ::dcool::memory::ArrayPool<
		T_, ::dcool::core::storageRequirementFor<T_>
	>;

	namespace detail_ {
		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto handleConverterOfPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::PoolHandleType_<PoolT_, storageRequirementC_>, void*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::PoolWithConverter_<storageRequirementC_> PoolT_
		> constexpr auto handleConverterOfPool_(PoolT_& pool_) noexcept {
			return pool_.template handleConverter<storageRequirementC_>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto constHandleConverterOfPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::PoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::PoolWithConstConverter_<storageRequirementC_> PoolT_
		> constexpr auto constHandleConverterOfPool_(PoolT_& pool_) noexcept {
			return pool_.template constHandleConverter<storageRequirementC_>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, ::dcool::memory::Pool<storageRequirementC_> PoolT_
		> class PoolAdaptorHelper_ {
			private: using Self_ = PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
			public: using Pool = PoolT_;
			public: static constexpr ::dcool::core::StorageRequirement storageRequirement = storageRequirementC_;

			public: using UnifiedHandle = ::dcool::memory::UnifiedHandleType<Pool>;
			public: using UnifiedConstHandle = ::dcool::memory::UnifiedConstHandleType<Pool>;
			public: using Handle = ::dcool::memory::detail_::PoolHandleType_<Pool, storageRequirementC_>;
			public: using ConstHandle = ::dcool::memory::detail_::PoolConstHandleType_<Pool, storageRequirementC_>;
			public: using HandleConverter = ::dcool::memory::detail_::PoolHandleConverterType_<Pool, storageRequirementC_>;
			public: using ConstHandleConverter = ::dcool::memory::detail_::PoolConstHandleConverterType_<Pool, storageRequirementC_>;
			public: using Difference = ::dcool::core::DifferenceType<Pool>;
			public: using Size = ::dcool::core::SizeType<Pool>;
			public: using Length = ::dcool::core::LengthType<Pool>;

			public: static constexpr auto allocate(Pool& pool_) -> Handle {
				return pool_.template allocate<storageRequirement>();
			}

			public: static constexpr void deallocate(Pool& pool_, Handle handle_) noexcept {
				pool_.template deallocate<storageRequirement>(handle_);
			}

			public: static constexpr auto handleConverter(Pool& pool_) noexcept {
				return ::dcool::memory::detail_::handleConverterOfPool_<storageRequirement>(pool_);
			}

			public: static constexpr auto constHandleConverter(Pool& pool_) noexcept {
				return ::dcool::memory::detail_::constHandleConverterOfPool_<storageRequirement>(pool_);
			}
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto handleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>, void*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::ArrayPoolWithConverter_<storageRequirementC_> PoolT_
		> constexpr auto handleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return pool_.template arrayHandleConverter<storageRequirementC_>(pool_);
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto constHandleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::memory::detail_::ArrayPoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::ArrayPoolWithConstConverter_<storageRequirementC_> PoolT_
		> constexpr auto constHandleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return pool_.template arrayConstHandleConverter<storageRequirementC_>(pool_);
		}

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept BackExpandableArrayPool_ = ::dcool::memory::ArrayPool<T_, storageRequirementC_> && requires (
			T_ pool_,
			::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_> handle_,
			::dcool::core::LengthType<T_> length_,
			::dcool::core::SizeType<T_> extra_
		) {
			{
				pool_.template expandBack<storageRequirementC_>(handle_, length_, extra_)
			} -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_, ::dcool::memory::ArrayPool<storageRequirementC_> PoolT_
		> constexpr auto expandBack_(
			PoolT_& pool_,
			::dcool::memory::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_> handle_,
			::dcool::core::LengthType<PoolT_> length_,
			::dcool::core::SizeType<PoolT_> extra_
		) -> ::dcool::core::Boolean {
			return false;
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::memory::detail_::BackExpandableArrayPool_<storageRequirementC_> PoolT_
		> constexpr auto expandBack_(
			PoolT_& pool_,
			::dcool::memory::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_> handle_,
			::dcool::core::LengthType<PoolT_> length_,
			::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.template expandBack<storageRequirementC_>(handle_, length_, extra_);
		}
	}

	template <
		typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
	> class PoolAdaptor : public ::dcool::memory::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_> {
		private: using Self_ = PoolAdaptor<PoolT_, storageRequirementC_>;
		private: using Super_ = ::dcool::memory::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
		private: using Pool_ = PoolT_;
		private: static constexpr ::dcool::core::StorageRequirement storageRequirement_ = storageRequirementC_;
	};

	template <
		::dcool::core::StorageRequirement storageRequirementC_, ::dcool::memory::ArrayPool<storageRequirementC_> PoolT_
	> class PoolAdaptor<
		PoolT_, storageRequirementC_
	>: public ::dcool::memory::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_> {
		private: using Self_ = PoolAdaptor<PoolT_, storageRequirementC_>;
		private: using Super_ = ::dcool::memory::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
		private: using Pool_ = PoolT_;
		private: static constexpr ::dcool::core::StorageRequirement storageRequirement_ = storageRequirementC_;

		public: using typename Super_::Pool;
		public: using typename Super_::Size;
		public: using typename Super_::Length;
		public: using typename Super_::Handle;
		public: using Super_::storageRequirement;

		public: using UnifiedArrayHandle = ::dcool::memory::UnifiedArrayHandleType<Pool>;
		public: using ArrayHandle = ::dcool::memory::detail_::ArrayPoolHandleType_<Pool, storageRequirementC_>;
		public: using ArrayConstHandleConverter =
			::dcool::memory::detail_::ArrayPoolConstHandleConverterType_<Pool, storageRequirementC_>
		;
		public: using ArrayHandleConverter = ::dcool::memory::detail_::ArrayPoolHandleConverterType_<Pool, storageRequirementC_>;

		public: using Super_::allocate;

		static constexpr auto allocate(Pool& pool_, Length length_) -> ArrayHandle {
			return pool_.template allocate<storageRequirement>(length_);
		}

		public: using Super_::deallocate;

		static constexpr void deallocate(Pool& pool_, ArrayHandle handle_, Length length_) noexcept {
			pool_.deallocate<storageRequirement>(handle_, length_);
		}

		static constexpr auto expandBack(Pool& pool_, ArrayHandle handle_, Length length_, Size extra_) noexcept {
			return ::dcool::memory::detail_::expandBack_<storageRequirement_>(pool_, handle_, length_, extra_);
		}

		static constexpr auto arrayHandleConverter(Pool& pool_) noexcept {
			return ::dcool::memory::detail_::handleConverterOfArrayPool_<storageRequirement>(pool_);
		}

		static constexpr auto arrayConstHandleConverter(Pool& pool_) noexcept {
			return ::dcool::memory::detail_::constHandleConverterOfArrayPool_<storageRequirement>(pool_);
		}
	};

	template <typename PoolT_, typename ValueT_> using PoolAdaptorFor = ::dcool::memory::PoolAdaptor<
		PoolT_, ::dcool::core::storageRequirementFor<ValueT_>
	>;

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedAllocateFor(PoolT_& pool_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::allocate(pool_);
	}

	template <
		typename ValueT_, typename PoolT_, typename LengthT_
	> constexpr auto adaptedAllocateFor(PoolT_& pool_, LengthT_ length_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::allocate(pool_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_, typename LengthT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_, LengthT_ length_) noexcept {
		::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_, length_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedHandleConverterFor(PoolT_& pool_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::handleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedConstHandleConverterFor(PoolT_& pool_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::constHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayHandleConverterFor(PoolT_& pool_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::arrayHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayConstHandleConverterFor(PoolT_& pool_) {
		return ::dcool::memory::PoolAdaptorFor<PoolT_, ValueT_>::arrayConstHandleConverter(pool_);
	}

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept PoolWithBijectiveHandleConverter =
		::dcool::memory::Pool<T_, storageRequirementC_> &&
		::dcool::core::BijectiveConverter<
			::dcool::memory::detail_::PoolHandleConverterType_<T_, storageRequirementC_>,
			::dcool::memory::detail_::PoolHandleType_<T_, storageRequirementC_>,
			void*
		> &&
		::dcool::core::BijectiveConverter<
			::dcool::memory::detail_::PoolConstHandleConverterType_<T_, storageRequirementC_>,
			::dcool::memory::detail_::PoolConstHandleType_<T_, storageRequirementC_>,
			void const*
		>
	;

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept ArrayPoolWithBijectiveHandleConverter =
		::dcool::memory::Pool<T_, storageRequirementC_> &&
		::dcool::core::BijectiveConverter<
			::dcool::memory::detail_::ArrayPoolHandleConverterType_<T_, storageRequirementC_>,
			::dcool::memory::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>,
			void*
		> &&
		::dcool::core::BijectiveConverter<
			::dcool::memory::detail_::ArrayPoolConstHandleConverterType_<T_, storageRequirementC_>,
			::dcool::memory::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>,
			void const*
		>
	;

	template <typename T_, typename ValueT_> concept PoolWithBijectiveHandleConverterFor =
		::dcool::memory::PoolWithBijectiveHandleConverter<T_, ::dcool::core::storageRequirementFor<ValueT_>>
	;

	template <typename T_, typename ValueT_> concept ArrayPoolWithBijectiveHandleConverterFor =
		::dcool::memory::ArrayPoolWithBijectiveHandleConverter<T_, ::dcool::core::storageRequirementFor<ValueT_>>
	;

	template <::dcool::memory::ClassicPool ClassicPoolT_> class PoolFromClassic {
		private: using Self_ = PoolFromClassic<ClassicPoolT_>;
		public: using ClassicPool = ClassicPoolT_;

		private: using ClassicPoolAdaptor_ = ::dcool::memory::ClassicPoolAdaptor<ClassicPool>;
		public: using UnifiedHandle = void*;
		public: using UnifiedConstHandle = void const*;
		public: using UnifiedArrayHandle = UnifiedHandle;
		public: using UnifiedArrayConstHandle = UnifiedConstHandle;
		public: using Handle = UnifiedHandle;
		public: using ArrayHandle = Handle;
		public: using Size = ClassicPoolAdaptor_::Size;
		public: using Length = ClassicPoolAdaptor_::Size;
		public: using Difference = ::dcool::core::Difference;

		public: [[no_unique_address]] ClassicPool classicPool;

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> constexpr auto allocate(Length length_ = 1) noexcept -> Handle {
			Size totalSize = ::dcool::core::size(storageRequirementC__) * length_;
			if constexpr (::dcool::core::alignment(storageRequirementC__) != ClassicPoolAdaptor_::defaultAlignment) {
				return ClassicPoolAdaptor_::allocate(this->classicPool, totalSize, ::dcool::core::alignment(storageRequirementC__));
			}
			return ClassicPoolAdaptor_::allocate(this->classicPool, totalSize);
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> void deallocate(Handle handle_, Length length_ = 1) noexcept {
			Size totalSize = ::dcool::core::size(storageRequirementC__) * length_;
			if constexpr (::dcool::core::alignment(storageRequirementC__) != ClassicPoolAdaptor_::defaultAlignment) {
				return ClassicPoolAdaptor_::deallocate(
					this->classicPool, handle_, totalSize, ::dcool::core::alignment(storageRequirementC__)
				);
			}
			return ClassicPoolAdaptor_::deallocate(this->classicPool, handle_, totalSize);
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	using DefaultPool = ::dcool::memory::PoolFromClassic<::dcool::memory::DefaultClassicPool>;
}

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::memory, HasTypePool, ExtractedPoolType, Pool)

namespace dcool::memory {
	template <typename T_> using PoolType = ::dcool::memory::ExtractedPoolType<T_, ::dcool::memory::DefaultPool>;
}

#endif
