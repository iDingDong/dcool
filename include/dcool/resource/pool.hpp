#ifndef DCOOL_RESOURCE_POOL_HPP_INCLUDED_
#	define DCOOL_RESOURCE_POOL_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <memory>
#	include <new>
#	include <stdexcept>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeUnifiedHandle_, ExtractedUnifiedHandleType_, UnifiedHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeUnifiedConstHandle_, ExtractedUnifiedConstHandleType_, UnifiedConstHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeUnifiedArrayHandle_, ExtractedUnifiedArrayHandleType_, UnifiedArrayHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeUnifiedArrayConstHandle_, ExtractedUnifiedArrayConstHandleType_, UnifiedArrayConstHandle
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeHandleConverter_, ExtractedHandleConverterType_, HandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeConstHandleConverter_, ExtractedConstHandleConverterType_, ConstHandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeArrayHandleConverter_, ExtractedArrayHandleConverterType_, ArrayHandleConverter
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeArrayConstHandleConverter_, ExtractedArrayConstHandleConverterType_, ArrayConstHandleConverter
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_, HasValueMaxAlignment_, extractedMaxAlignmentValue_, maxAligment
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_, HasValueDefaultAlignment_, extractedDefaultAlignmentValue_, defaultAligment
)

namespace dcool::resource {
	using BadAllocation = ::std::bad_alloc;

	template <typename T_> concept ClassicPool = requires (
		T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
	) {
		pointer_ = pool_.allocate(size_);
		pool_.deallocate(pointer_, size_);
	};

	namespace detail_ {
		template <typename T_> concept AlignedAllocateableClassicPool_ = ::dcool::resource::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
		) {
			pointer_ = pool_.allocate(size_, alignment_);
		};

		template <::dcool::resource::ClassicPool PoolT_> [[nodiscard("Might leak memory.")]] constexpr auto alignedAllocate_(
			PoolT_& pool_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) -> void* {
			return pool_.allocate(size_);
		}

		template <
			::dcool::resource::detail_::AlignedAllocateableClassicPool_ PoolT_
		> [[nodiscard("Might leak memory.")]] constexpr auto alignedAllocate_(
			PoolT_& pool_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) -> void* {
			return pool_.allocate(size_, alignment_);
		}

		template <typename T_> concept AlignedDeallocateableClassicPool_ = ::dcool::resource::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_, alignment_);
		};

		template <::dcool::resource::ClassicPool PoolT_> void alignedDeallocate_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_);
		}

		template <::dcool::resource::detail_::AlignedDeallocateableClassicPool_ PoolT_> void alignedDeallocate_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::Alignment alignment_
		) {
			pool_.deallocate(pointer_, size_, alignment_);
		}

		template <typename T_> concept FrontExpandableClassicPool_ = ::dcool::resource::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::SizeType<T_> extra_
		) {
			{ pool_.expandFront(pointer_, size_, extra_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <
			::dcool::resource::ClassicPool PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandFront_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return false;
		}

		template <
			::dcool::resource::detail_::FrontExpandableClassicPool_ PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandFront_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.expandFront(pointer_, size_, extra_);
		}

		template <typename T_> concept BackExpandableClassicPool_ = ::dcool::resource::ClassicPool<T_> && requires (
			T_ pool_, void* pointer_, ::dcool::core::SizeType<T_> size_, ::dcool::core::SizeType<T_> extra_
		) {
			{ pool_.expandBack(pointer_, size_, extra_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <
			::dcool::resource::ClassicPool PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandBack_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return false;
		}

		template <
			::dcool::resource::detail_::BackExpandableClassicPool_ PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandBack_(
			PoolT_& pool_, void* pointer_, ::dcool::core::SizeType<PoolT_> size_, ::dcool::core::SizeType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.expandBack(pointer_, size_, extra_);
		}
	}

	template <::dcool::resource::ClassicPool PoolT_> class ClassicPoolAdaptor {
		private: using Self_ = ClassicPoolAdaptor<PoolT_>;
		public: using Pool = PoolT_;

		public: using Difference = ::dcool::core::DifferenceType<PoolT_>;
		public: using Size = ::dcool::core::SizeType<PoolT_>;

		public: static constexpr ::dcool::core::Alignment maxAlignment = ::dcool::resource::detail_::extractedMaxAlignmentValue_<Pool>(
			::dcool::core::defaultNewAlignment
		);
		public: static constexpr ::dcool::core::Alignment defaultAlignment =
			::dcool::resource::detail_::extractedDefaultAlignmentValue_<Pool>(
				::dcool::core::min(maxAlignment, ::dcool::core::defaultNewAlignment)
			)
		;

		static_assert(defaultAlignment <= maxAlignment);

		public: [[nodiscard("Might leak memory.")]] static constexpr auto allocate(Pool& pool_, Size size_) -> void* {
			return pool_.allocate(size_);
		}

		public: [[nodiscard("Might leak memory.")]] static constexpr auto allocate(
			Pool& pool_, Size size_, ::dcool::core::Alignment alignment_
		) -> void* {
			return ::dcool::resource::detail_::alignedAllocate_(pool_, size_, alignment_);
		}

		public: static constexpr void deallocate(Pool& pool_, void* pointer_, Size size_) noexcept {
			pool_.deallocate(pointer_, size_);
		}

		public: static constexpr void deallocate(
			Pool& pool_, void* pointer_, Size size_, ::dcool::core::Alignment alignemt_
		) noexcept {
			::dcool::resource::detail_::alignedDeallocate_(pool_, pointer_, size_, alignemt_);
		}

		public: [[nodiscard("Might make wrong assumption on result of expand.")]] static constexpr auto expandFront(
			Pool& pool_, void* pointer_, Size size_, Size extra_
		) noexcept -> ::dcool::core::Boolean {
			return ::dcool::resource::detail_::expandFront_(pool_, pointer_, size_, extra_);
		}

		public: [[nodiscard("Might make wrong assumption on result of expand.")]] static constexpr auto expandBack(
			Pool& pool_, void* pointer_, Size size_, Size extra_
		) noexcept -> ::dcool::core::Boolean {
			return ::dcool::resource::detail_::expandBack_(pool_, pointer_, size_, extra_);
		}
	};

	class DefaultClassicPool {
		private: using Self_ = DefaultClassicPool;

		public: using Size = ::dcool::core::Size;

		public: static constexpr ::dcool::core::Alignment maxAlignment = ::dcool::core::maxRepresentableAlignment;
		public: static constexpr ::dcool::core::Alignment defaultAlignment = ::dcool::core::defaultNewAlignment;

		public: [[nodiscard("Might leak memory.")]] auto noThrowAllocate(Size size_) noexcept -> void* {
			return ::operator new(size_, ::std::nothrow);
		}

		public: [[nodiscard("Might leak memory.")]] auto allocate(Size size_) -> void* {
			return ::operator new(size_);
		}

		public: [[nodiscard("Might leak memory.")]] auto allocate(Size size_, ::dcool::core::Alignment alignment_) -> void* {
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

	template <typename T_> using UnifiedHandleType = ::dcool::resource::detail_::ExtractedUnifiedHandleType_<T_, void>;
	template <typename T_> using UnifiedConstHandleType = ::dcool::resource::detail_::ExtractedUnifiedConstHandleType_<T_, void>;
	template <typename T_> using UnifiedArrayHandleType = ::dcool::resource::detail_::ExtractedUnifiedArrayHandleType_<T_, void>;
	template <
		typename T_
	> using UnifiedArrayConstHandleType = ::dcool::resource::detail_::ExtractedUnifiedArrayConstHandleType_<T_, void>;

	namespace detail_ {
		template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept HasPoolHandleType_ = requires {
			typename T_::template Handle<storageRequirementC_>;
		};

		template <typename T_, typename OtherwiseT_> using UnifiedHandleOrType_ = ::dcool::core::ConditionalType<
			::dcool::core::isSame<::dcool::resource::UnifiedHandleType<T_>, void>,
			::dcool::resource::UnifiedHandleType<T_>,
			OtherwiseT_
		>;

		template <typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_> struct PoolHandle_ {
			using Result_ = ::dcool::resource::detail_::UnifiedHandleOrType_<
				PoolT_, decltype(::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>())
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::HasPoolHandleType_<storageRequirementC_> PoolT_
		> struct PoolHandle_<PoolT_, storageRequirementC_> {
			using Result_ = typename PoolT_::template Handle<storageRequirementC_>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolHandleType_ = ::dcool::resource::detail_::PoolHandle_<PoolT_, storageRequirementC_>::Result_;

		template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept HasPoolConstHandleType_ = requires {
			typename T_::template ConstHandle<storageRequirementC_>;
		};

		template <typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_> struct PoolConstHandle_ {
			using Result_ = ::dcool::resource::detail_::ExtractedUnifiedConstHandleType_<
				PoolT_,
				::dcool::core::PointedConstantizedType<::dcool::resource::detail_::PoolHandleType_<PoolT_, storageRequirementC_>>
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::HasPoolConstHandleType_<storageRequirementC_> PoolT_
		> struct PoolConstHandle_<PoolT_, storageRequirementC_> {
			using Result_ = typename PoolT_::template ConstHandle<storageRequirementC_>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolConstHandleType_ = PoolConstHandle_<PoolT_, storageRequirementC_>::Result_;

		template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept HasArrayPoolHandleType_ = requires {
			typename T_::template ArrayHandle<storageRequirementC_>;
		};

		template <typename T_, typename OtherwiseT_> using UnifiedArrayHandleOrType_ = ::dcool::core::ConditionalType<
			::dcool::core::isSame<::dcool::resource::UnifiedArrayHandleType<T_>, void>,
			::dcool::resource::UnifiedArrayHandleType<T_>,
			OtherwiseT_
		>;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept CanAllocateArray_ = requires (T_ pool_, ::dcool::core::LengthType<T_> length_) {
			{ pool_.template allocate<storageRequirementC_>(length_) } -> ::dcool::core::Object;
		};

		template <typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_> struct DeductedArrayPoolHandle_ {
			using Result_ = void;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::CanAllocateArray_<storageRequirementC_> PoolT_
		> struct DeductedArrayPoolHandle_<PoolT_, storageRequirementC_> {
			using Result_ = decltype(
				::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>(
					::dcool::core::declval<::dcool::core::LengthType<PoolT_>>()
				)
			);
		};

		template <typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_> struct ArrayPoolHandle_ {
			using Result_ = typename ::dcool::resource::detail_::UnifiedArrayHandleOrType_<
				PoolT_, typename ::dcool::resource::detail_::DeductedArrayPoolHandle_<PoolT_, storageRequirementC_>::Result_
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::HasArrayPoolHandleType_<storageRequirementC_> PoolT_
		> struct ArrayPoolHandle_<PoolT_, storageRequirementC_> {
			using Result_ = typename PoolT_::template ArrayHandle<storageRequirementC_>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolHandleType_ = ::dcool::resource::detail_::ArrayPoolHandle_<PoolT_, storageRequirementC_>::Result_;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept HasArrayPoolConstHandleType_ = requires {
			typename T_::template ArrayConstHandle<storageRequirementC_>;
		};

		template <typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_> struct ArrayPoolConstHandle_ {
			using Result_ = ::dcool::resource::detail_::ExtractedUnifiedArrayConstHandleType_<
				PoolT_,
				::dcool::core::PointedConstantizedType<::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>>
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::HasArrayPoolConstHandleType_<storageRequirementC_> PoolT_
		> struct ArrayPoolConstHandle_<PoolT_, storageRequirementC_> {
			using Result_ = typename PoolT_::template ArrayConstHandle<storageRequirementC_>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolConstHandleType_ = ::dcool::resource::detail_::ArrayPoolConstHandle_<
			PoolT_, storageRequirementC_
		>::Result_;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithConverter_ = requires (T_ pool_) {
			{ pool_.template handleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_>, void*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithConstConverter_ = requires (T_ pool_) {
			{ pool_.template constHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::resource::detail_::PoolConstHandleType_<T_, storageRequirementC_>, void const*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithConverter_ = requires (T_ pool_) {
			{ pool_.template arrayHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>, void*
			>;
		};

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithConstConverter_ = requires (T_ pool_) {
			{ pool_.template arrayConstHandleConverter<storageRequirementC_>() } -> ::dcool::core::BidirectionalConsistentConverter<
				::dcool::resource::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>, void const*
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct PoolHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::PoolHandleType_<PoolT_, storageRequirementC_>, void*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::PoolWithConverter_<storageRequirementC_> PoolT_
		> struct PoolHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template handleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolHandleConverterType_ = ::dcool::resource::detail_::ExtractedHandleConverterType_<
			PoolT_, typename ::dcool::resource::detail_::PoolHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct PoolConstHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::PoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::PoolWithConstConverter_<storageRequirementC_> PoolT_
		> struct PoolConstHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template constHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolConstHandleConverterType_ = ::dcool::resource::detail_::ExtractedConstHandleConverterType_<
			PoolT_, typename ::dcool::resource::detail_::PoolConstHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct ArrayPoolHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>, void*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::ArrayPoolWithConverter_<storageRequirementC_> PoolT_
		> struct ArrayPoolHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template arrayHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolHandleConverterType_ = ::dcool::resource::detail_::ExtractedArrayHandleConverterType_<
			PoolT_, typename ::dcool::resource::detail_::ArrayPoolHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> struct ArrayPoolConstHandleConverterHelper_ {
			using Result_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::ArrayPoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::ArrayPoolWithConstConverter_<storageRequirementC_> PoolT_
		> struct ArrayPoolConstHandleConverterHelper_<PoolT_, storageRequirementC_> {
			using Result_ = ::dcool::core::QualifiedReferenceRemovedType<
				decltype(::dcool::core::declval<PoolT_&>().template arrayConstHandleConverter<storageRequirementC_>())
			>;
		};

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolConstHandleConverterType_ = ::dcool::resource::detail_::ExtractedArrayConstHandleConverterType_<
			PoolT_, typename ::dcool::resource::detail_::ArrayPoolConstHandleConverterHelper_<PoolT_, storageRequirementC_>::Result_
		>;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithProperUnifiedHandle_ =
			::dcool::core::OneOf<
				::dcool::resource::UnifiedHandleType<T_>, ::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_>, void
			> && ::dcool::core::OneOf<
				::dcool::resource::UnifiedConstHandleType<T_>,
				::dcool::resource::detail_::PoolConstHandleType_<T_, storageRequirementC_>,
				void
			>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept PoolWithProperlyConvertibleHandle_ =
			::dcool::core::ConvertibleTo<
				::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_>,
				::dcool::resource::detail_::PoolConstHandleType_<T_, storageRequirementC_>
			> && (
				::dcool::resource::detail_::PoolWithConverter_<T_, storageRequirementC_> ||
				::dcool::core::ConvertibleBetween<::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_>, void*>
			)
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperUnifiedHandle_ =
			::dcool::core::OneOf<
				::dcool::resource::UnifiedArrayHandleType<T_>,
				::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>,
				void
			> && ::dcool::core::OneOf<
				::dcool::resource::UnifiedArrayConstHandleType<T_>,
				::dcool::resource::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>,
				void
			>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperlyConvertibleHandle_ =
			::dcool::core::ConvertibleTo<
				::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>,
				::dcool::resource::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>
			> && (
				::dcool::resource::detail_::ArrayPoolWithConverter_<T_, storageRequirementC_> ||
				::dcool::core::ConvertibleBetween<::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>, void*>
			)
		;

		template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept PoolWithProperHandle_ =
			::dcool::resource::detail_::PoolWithProperUnifiedHandle_<T_, storageRequirementC_> &&
			::dcool::resource::detail_::PoolWithProperlyConvertibleHandle_<T_, storageRequirementC_>
		;

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept ArrayPoolWithProperHandle_ =
			::dcool::resource::detail_::ArrayPoolWithProperUnifiedHandle_<T_, storageRequirementC_> &&
			::dcool::resource::detail_::ArrayPoolWithProperlyConvertibleHandle_<T_, storageRequirementC_>
		;
	}

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept Pool = ::dcool::resource::detail_::PoolWithProperHandle_<T_, storageRequirementC_> && requires (
		T_ pool_, ::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_> handle_
	) {
		handle_ = pool_.template allocate<storageRequirementC_>();
		pool_.template deallocate<storageRequirementC_>(handle_);
	};

	template <typename T_, typename ValueT_> concept PoolFor = ::dcool::resource::Pool<
		T_, ::dcool::core::storageRequirementFor<T_>
	>;

	template <typename T_, ::dcool::core::StorageRequirement storageRequirementC_> concept ArrayPool =
		::dcool::resource::detail_::ArrayPoolWithProperHandle_<T_, storageRequirementC_> &&
		::dcool::resource::Pool<T_, storageRequirementC_> &&
		requires (
			T_ pool_,
			::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_> handle_,
			::dcool::core::LengthType<T_> length_
		)	{
			handle_ = pool_.template allocate<storageRequirementC_>(length_);
			pool_.template deallocate<storageRequirementC_>(handle_, length_);
		}
	;

	template <typename T_, typename ValueT_> concept ArrayPoolFor = ::dcool::resource::ArrayPool<
		T_, ::dcool::core::storageRequirementFor<T_>
	>;

	namespace detail_ {
		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto handleConverterOfPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::PoolHandleType_<PoolT_, storageRequirementC_>, void*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::PoolWithConverter_<storageRequirementC_> PoolT_
		> constexpr auto handleConverterOfPool_(PoolT_& pool_) noexcept {
			return pool_.template handleConverter<storageRequirementC_>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto constHandleConverterOfPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::PoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::PoolWithConstConverter_<storageRequirementC_> PoolT_
		> constexpr auto constHandleConverterOfPool_(PoolT_& pool_) noexcept {
			return pool_.template constHandleConverter<storageRequirementC_>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, ::dcool::resource::Pool<storageRequirementC_> PoolT_
		> class PoolAdaptorHelper_ {
			private: using Self_ = PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
			public: using Pool = PoolT_;
			public: static constexpr ::dcool::core::StorageRequirement storageRequirement = storageRequirementC_;

			public: using UnifiedHandle = ::dcool::resource::UnifiedHandleType<Pool>;
			public: using UnifiedConstHandle = ::dcool::resource::UnifiedConstHandleType<Pool>;
			public: using ConstHandle = ::dcool::resource::detail_::PoolConstHandleType_<Pool, storageRequirementC_>;
			public: using Handle = ::dcool::resource::detail_::PoolHandleType_<Pool, storageRequirementC_>;
			public: using ConstHandleConverter = ::dcool::resource::detail_::PoolConstHandleConverterType_<
				Pool, storageRequirementC_
			>;
			public: using HandleConverter = ::dcool::resource::detail_::PoolHandleConverterType_<Pool, storageRequirementC_>;
			public: using Difference = ::dcool::core::DifferenceType<Pool>;
			public: using Size = ::dcool::core::SizeType<Pool>;
			public: using Length = ::dcool::core::LengthType<Pool>;

			public: [[nodiscard("Might leak memory.")]] static constexpr auto allocate(Pool& pool_) -> Handle {
				return pool_.template allocate<storageRequirement>();
			}

			public: [[nodiscard("Might leak memory.")]] static constexpr auto allocatePointer(Pool& pool_) -> void* {
				return fromHandle(pool_, allocate(pool_));
			}

			public: static constexpr void deallocate(Pool& pool_, Handle handle_) noexcept {
				pool_.template deallocate<storageRequirement>(handle_);
			}

			public: static constexpr void deallocatePointer(Pool& pool_, void* pointer_) noexcept {
				deallocate(pool_, toHandle(pool_, pointer_));
			}

			public: static constexpr auto constHandleConverter(Pool& pool_) noexcept {
				return ::dcool::resource::detail_::constHandleConverterOfPool_<storageRequirement>(pool_);
			}

			public: static constexpr auto handleConverter(Pool& pool_) noexcept {
				return ::dcool::resource::detail_::handleConverterOfPool_<storageRequirement>(pool_);
			}

			public: static constexpr auto toConstHandle(Pool& pool_, void const* pointer_) noexcept -> ConstHandle {
				return constHandleConverter(pool_)(pointer_);
			}

			public: static constexpr auto toHandle(Pool& pool_, void* pointer_) noexcept -> Handle {
				return handleConverter(pool_)(pointer_);
			}

			public: static constexpr auto fromConstHandle(Pool& pool_, ConstHandle handle_) noexcept -> void const* {
				return constHandleConverter(pool_)(handle_);
			}

			public: static constexpr auto fromHandle(Pool& pool_, Handle handle_) noexcept -> void* {
				return handleConverter(pool_)(handle_);
			}
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto handleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>, void*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::ArrayPoolWithConverter_<storageRequirementC_> PoolT_
		> constexpr auto handleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return pool_.template arrayHandleConverter<storageRequirementC_>(pool_);
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_, typename PoolT_
		> constexpr auto constHandleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return ::dcool::core::DefaultBidirectionalImplicitConverter<
				::dcool::resource::detail_::ArrayPoolConstHandleType_<PoolT_, storageRequirementC_>, void const*
			>();
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::ArrayPoolWithConstConverter_<storageRequirementC_> PoolT_
		> constexpr auto constHandleConverterOfArrayPool_(PoolT_& pool_) noexcept {
			return pool_.template arrayConstHandleConverter<storageRequirementC_>(pool_);
		}

		template <
			typename T_, ::dcool::core::StorageRequirement storageRequirementC_
		> concept BackExpandableArrayPool_ = ::dcool::resource::ArrayPool<T_, storageRequirementC_> && requires (
			T_ pool_,
			::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_> handle_,
			::dcool::core::LengthType<T_> length_,
			::dcool::core::LengthType<T_> extra_
		) {
			{
				pool_.template expandBack<storageRequirementC_>(handle_, length_, extra_)
			} -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <
			::dcool::core::StorageRequirement storageRequirementC_, ::dcool::resource::ArrayPool<storageRequirementC_> PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandBack_(
			PoolT_& pool_,
			::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_> handle_,
			::dcool::core::LengthType<PoolT_> length_,
			::dcool::core::LengthType<PoolT_> extra_
		) -> ::dcool::core::Boolean {
			return false;
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC_,
			::dcool::resource::detail_::BackExpandableArrayPool_<storageRequirementC_> PoolT_
		> [[nodiscard("Might make wrong assumption on result of expand.")]] constexpr auto expandBack_(
			PoolT_& pool_,
			::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_> handle_,
			::dcool::core::LengthType<PoolT_> length_,
			::dcool::core::LengthType<PoolT_> extra_
		) noexcept -> ::dcool::core::Boolean {
			return pool_.template expandBack<storageRequirementC_>(handle_, length_, extra_);
		}
	}

	template <
		typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
	> class PoolAdaptor : public ::dcool::resource::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_> {
		private: using Self_ = PoolAdaptor<PoolT_, storageRequirementC_>;
		private: using Super_ = ::dcool::resource::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
		private: using Pool_ = PoolT_;
		private: static constexpr ::dcool::core::StorageRequirement storageRequirement_ = storageRequirementC_;
	};

	template <
		::dcool::core::StorageRequirement storageRequirementC_, ::dcool::resource::ArrayPool<storageRequirementC_> PoolT_
	> class PoolAdaptor<
		PoolT_, storageRequirementC_
	>: public ::dcool::resource::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_> {
		private: using Self_ = PoolAdaptor<PoolT_, storageRequirementC_>;
		private: using Super_ = ::dcool::resource::detail_::PoolAdaptorHelper_<storageRequirementC_, PoolT_>;
		private: using Pool_ = PoolT_;
		private: static constexpr ::dcool::core::StorageRequirement storageRequirement_ = storageRequirementC_;

		public: using typename Super_::Pool;
		public: using typename Super_::Size;
		public: using typename Super_::Length;
		public: using typename Super_::Handle;
		public: using Super_::storageRequirement;

		public: using UnifiedArrayConstHandle = ::dcool::resource::UnifiedArrayConstHandleType<Pool>;
		public: using UnifiedArrayHandle = ::dcool::resource::UnifiedArrayHandleType<Pool>;
		public: using ArrayConstHandle = ::dcool::resource::detail_::ArrayPoolConstHandleType_<Pool, storageRequirementC_>;
		public: using ArrayHandle = ::dcool::resource::detail_::ArrayPoolHandleType_<Pool, storageRequirementC_>;
		public: using ArrayConstHandleConverter =
			::dcool::resource::detail_::ArrayPoolConstHandleConverterType_<Pool, storageRequirementC_>
		;
		public: using ArrayHandleConverter = ::dcool::resource::detail_::ArrayPoolHandleConverterType_<Pool, storageRequirementC_>;

		public: using Super_::allocate;
		public: using Super_::allocatePointer;

		public: [[nodiscard("Might leak memory.")]] static constexpr auto allocate(Pool& pool_, Length length_) -> ArrayHandle {
			return pool_.template allocate<storageRequirement>(length_);
		}

		public: [[nodiscard("Might leak memory.")]] static constexpr auto allocatePointer(Pool& pool_, Length length_) -> void* {
			return fromArrayHandle(pool_, allocate(pool_, length_));
		}

		public: using Super_::deallocate;
		public: using Super_::deallocatePointer;

		public: static constexpr void deallocate(Pool& pool_, ArrayHandle handle_, Length length_) noexcept {
			pool_.template deallocate<storageRequirement>(handle_, length_);
		}

		public: static constexpr void deallocatePointer(Pool& pool_, void* pointer_, Length length_) noexcept {
			deallocate(pool_, toArrayHandle(pool_, pointer_), length_);
		}

		public: [[nodiscard("Might make wrong assumption on result of expand.")]] static constexpr auto expandBack(
			Pool& pool_, ArrayHandle handle_, Length length_, Length extra_
		) noexcept {
			return ::dcool::resource::detail_::expandBack_<storageRequirement_>(pool_, handle_, length_, extra_);
		}

		public: [[nodiscard("Might make wrong assumption on result of expand.")]] static constexpr auto expandBackPointer(
			Pool& pool_, void* pointer_, Length length_, Length extra_
		) noexcept {
			return expandBack(pool_, toArrayHandle(pool_, pointer_), length_, extra_);
		}

		public: static constexpr auto arrayConstHandleConverter(Pool& pool_) noexcept {
			return ::dcool::resource::detail_::constHandleConverterOfArrayPool_<storageRequirement>(pool_);
		}

		public: static constexpr auto arrayHandleConverter(Pool& pool_) noexcept {
			return ::dcool::resource::detail_::handleConverterOfArrayPool_<storageRequirement>(pool_);
		}

		public: static constexpr auto toArrayConstHandle(Pool& pool_, void const* pointer_) noexcept -> ArrayConstHandle {
			return arrayConstHandleConverter(pool_)(pointer_);
		}

		public: static constexpr auto toArrayHandle(Pool& pool_, void* pointer_) noexcept -> ArrayHandle {
			return arrayHandleConverter(pool_)(pointer_);
		}

		public: static constexpr auto fromArrayConstHandle(Pool& pool_, ArrayConstHandle handle_) noexcept -> void const* {
			return arrayConstHandleConverter(pool_)(handle_);
		}

		public: static constexpr auto fromArrayHandle(Pool& pool_, ArrayHandle handle_) noexcept -> void* {
			return arrayHandleConverter(pool_)(handle_);
		}
	};

	template <typename PoolT_, typename ValueT_> using PoolAdaptorFor = ::dcool::resource::PoolAdaptor<
		PoolT_, ::dcool::core::storageRequirementFor<ValueT_>
	>;

	template <
		typename ValueT_, typename PoolT_
	> [[nodiscard("Might leak memory.")]] constexpr auto adaptedAllocateFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::allocate(pool_);
	}

	template <
		typename ValueT_, typename PoolT_
	> [[nodiscard("Might leak memory.")]] constexpr auto adaptedAllocatePointerFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::allocatePointer(pool_);
	}

	template <
		typename ValueT_, typename PoolT_, typename LengthT_
	> [[nodiscard("Might leak memory.")]] constexpr auto adaptedAllocateFor(PoolT_& pool_, LengthT_ length_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::allocate(pool_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename LengthT_
	> [[nodiscard("Might leak memory.")]] constexpr auto adaptedAllocatePointerFor(PoolT_& pool_, LengthT_ length_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::allocatePointer(pool_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr void adaptedDeallocatePointerFor(PoolT_& pool_, void* pointer_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocatePointer(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_, typename LengthT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_, LengthT_ length_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename LengthT_
	> constexpr void adaptedDeallocatePointerFor(PoolT_& pool_, void* pointer_, LengthT_ length_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocatePointer(pool_, pointer_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_, typename LengthT_
	> constexpr auto adaptedExpandBackFor(PoolT_& pool_, HandleT_ handle_, LengthT_ length_, LengthT_ extra_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::expandBack(pool_, handle_, length_, extra_);
	}

	template <
		typename ValueT_, typename PoolT_, typename LengthT_
	> constexpr auto adaptedExpandBackPointerFor(PoolT_& pool_, void* pointer_, LengthT_ length_, LengthT_ extra_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::expandBackPointer(pool_, pointer_, length_, extra_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedHandleConverterFor(PoolT_& pool_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::handleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedConstHandleConverterFor(PoolT_& pool_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::constHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayHandleConverterFor(PoolT_& pool_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::arrayHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayConstHandleConverterFor(PoolT_& pool_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::arrayConstHandleConverter(pool_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr auto adaptedFromConstHandleFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::fromConstHandle(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr auto adaptedFromHandleFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::fromHandle(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr auto adaptedToConstHandleFor(PoolT_& pool_, void const* pointer_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::toConstHandle(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr auto adaptedToHandleFor(PoolT_& pool_, void* pointer_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::toHandle(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr auto adaptedFromArrayConstHandleFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::fromArrayConstHandle(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr auto adaptedFromArrayHandleFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::fromArrayHandle(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr auto adaptedToArrayConstHandleFor(PoolT_& pool_, void const* pointer_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::toArrayConstHandle(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr auto adaptedToArrayHandleFor(PoolT_& pool_, void* pointer_) noexcept {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::toArrayHandle(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_, typename... ArgumentTs_
	> [[nodiscard("Might leak memory.")]] constexpr auto createPointerByPoolFor(PoolT_& pool_, ArgumentTs_&&... parameters_) {
		ValueT_* pointer_ = static_cast<ValueT_*>(::dcool::resource::adaptedAllocatePointerFor<ValueT_>(pool_));
		try {
			pointer_ = new (pointer_) ValueT_(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		} catch (...) {
			::dcool::resource::adaptedDeallocatePointerFor<ValueT_>(pool_, pointer_);
			throw;
		}
		return pointer_;
	}

	template <
		typename ValueT_, typename PoolT_, typename... ArgumentTs_
	> [[nodiscard("Might leak memory.")]] constexpr auto createHandleByPoolFor(PoolT_& pool_, ArgumentTs_&&... parameters_) {
		ValueT_* pointer_ = ::dcool::resource::createPointerByPoolFor<ValueT_>(
			pool_, ::dcool::core::forward<ArgumentTs_>(parameters_)...
		);
		return ::dcool::resource::adaptedToHandleFor<ValueT_>(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_
	> constexpr void destroyPointerByPoolFor(PoolT_& pool_, ValueT_* pointer_) noexcept {
		::dcool::core::destruct(*pointer_);
		::dcool::resource::adaptedDeallocatePointerFor<ValueT_>(pool_, pointer_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr void destroyHandleByPoolFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		static_cast<ValueT_*>(::dcool::resource::adaptedFromHandleFor<ValueT_>(pool_, handle_))->~ValueT_();
		ValueT_* pointer_ = static_cast<ValueT_*>(::dcool::resource::adaptedFromHandleFor<ValueT_>(pool_, handle_));
		::dcool::resource::destroyPointerByPoolFor<ValueT_>(pool_, pointer_);
	}

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept PoolWithBijectiveHandleConverter =
		::dcool::resource::Pool<T_, storageRequirementC_> &&
		::dcool::core::BijectiveConverter<
			::dcool::resource::detail_::PoolHandleConverterType_<T_, storageRequirementC_>,
			::dcool::resource::detail_::PoolHandleType_<T_, storageRequirementC_>,
			void*
		> &&
		::dcool::core::BijectiveConverter<
			::dcool::resource::detail_::PoolConstHandleConverterType_<T_, storageRequirementC_>,
			::dcool::resource::detail_::PoolConstHandleType_<T_, storageRequirementC_>,
			void const*
		>
	;

	template <
		typename T_, ::dcool::core::StorageRequirement storageRequirementC_
	> concept ArrayPoolWithBijectiveHandleConverter =
		::dcool::resource::Pool<T_, storageRequirementC_> &&
		::dcool::core::BijectiveConverter<
			::dcool::resource::detail_::ArrayPoolHandleConverterType_<T_, storageRequirementC_>,
			::dcool::resource::detail_::ArrayPoolHandleType_<T_, storageRequirementC_>,
			void*
		> &&
		::dcool::core::BijectiveConverter<
			::dcool::resource::detail_::ArrayPoolConstHandleConverterType_<T_, storageRequirementC_>,
			::dcool::resource::detail_::ArrayPoolConstHandleType_<T_, storageRequirementC_>,
			void const*
		>
	;

	template <typename T_, typename ValueT_> concept PoolWithBijectiveHandleConverterFor =
		::dcool::resource::PoolWithBijectiveHandleConverter<T_, ::dcool::core::storageRequirementFor<ValueT_>>
	;

	template <typename T_, typename ValueT_> concept ArrayPoolWithBijectiveHandleConverterFor =
		::dcool::resource::ArrayPoolWithBijectiveHandleConverter<T_, ::dcool::core::storageRequirementFor<ValueT_>>
	;

	template <::dcool::resource::ClassicPool ClassicPoolT_> class PoolFromClassic {
		private: using Self_ = PoolFromClassic<ClassicPoolT_>;
		public: using ClassicPool = ClassicPoolT_;

		private: using ClassicPoolAdaptor_ = ::dcool::resource::ClassicPoolAdaptor<ClassicPool>;
		public: using UnifiedHandle = void*;
		public: using UnifiedConstHandle = void const*;
		public: using UnifiedArrayHandle = UnifiedHandle;
		public: using UnifiedArrayConstHandle = UnifiedConstHandle;
		public: template <::dcool::core::StorageRequirement> using Handle = UnifiedHandle;
		public: template <::dcool::core::StorageRequirement> using ConstHandle = UnifiedConstHandle;
		public: template <::dcool::core::StorageRequirement> using ArrayHandle = UnifiedArrayHandle;
		public: template <::dcool::core::StorageRequirement> using ArrayConstHandle = UnifiedArrayConstHandle;
		public: using Size = ClassicPoolAdaptor_::Size;
		public: using Length = ClassicPoolAdaptor_::Size;
		public: using Difference = ::dcool::core::Difference;

		public: [[no_unique_address]] ClassicPool classicPool;

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> [[nodiscard("Might leak memory.")]] constexpr auto allocate(
			Length length_ = 1
		) noexcept -> Handle<storageRequirementC__> {
			Size totalSize = ::dcool::core::size(storageRequirementC__) * length_;
			if constexpr (::dcool::core::alignment(storageRequirementC__) != ClassicPoolAdaptor_::defaultAlignment) {
				return ClassicPoolAdaptor_::allocate(this->classicPool, totalSize, ::dcool::core::alignment(storageRequirementC__));
			}
			return ClassicPoolAdaptor_::allocate(this->classicPool, totalSize);
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> void deallocate(Handle<storageRequirementC__> handle_, Length length_ = 1) noexcept {
			Size totalSize = ::dcool::core::size(storageRequirementC__) * length_;
			if constexpr (::dcool::core::alignment(storageRequirementC__) != ClassicPoolAdaptor_::defaultAlignment) {
				ClassicPoolAdaptor_::deallocate(
					this->classicPool, handle_, totalSize, ::dcool::core::alignment(storageRequirementC__)
				);
			} else {
				ClassicPoolAdaptor_::deallocate(this->classicPool, handle_, totalSize);
			}
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <typename AllocatorT_> struct PoolFromStandardAllocator {
		private: using Self_ = PoolFromStandardAllocator<AllocatorT_>;
		public: using StandardAllocator = AllocatorT_;

		private: using StandardTraits_ = ::std::allocator_traits<StandardAllocator>;
		public: using UnifiedHandle = void*;
		public: using UnifiedConstHandle = void const*;
		public: using UnifiedArrayHandle = UnifiedHandle;
		public: using UnifiedArrayConstHandle = UnifiedConstHandle;
		public: template <::dcool::core::StorageRequirement> using Handle = UnifiedHandle;
		public: template <::dcool::core::StorageRequirement> using ConstHandle = UnifiedConstHandle;
		public: template <::dcool::core::StorageRequirement> using ArrayHandle = UnifiedArrayHandle;
		public: template <::dcool::core::StorageRequirement> using ArrayConstHandle = UnifiedArrayConstHandle;
		public: using Size = ::dcool::core::Size;
		public: using Length = StandardTraits_::size_type;
		public: using Difference = StandardTraits_::difference_type;

		public: [[no_unique_address]] StandardAllocator standardAllocator;

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> [[nodiscard("Might leak memory.")]] constexpr auto allocate(
			Length length_ = 1
		) noexcept -> Handle<storageRequirementC__> {
			using Value_ = ::dcool::core::AlignedStorage<storageRequirementC__>;
			using RebindedAllocator_ = StandardTraits_::template rebind_alloc<Value_>;
			using RebindedAllocatorTraits_ = StandardTraits_::template rebind_traits<Value_>;
			RebindedAllocator_ rebinded_(this->standardAllocator);
			return RebindedAllocatorTraits_::allocate(rebinded_, length_);
		}

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> void deallocate(Handle<storageRequirementC__> handle_, Length length_ = 1) noexcept {
			using Value_ = ::dcool::core::AlignedStorage<storageRequirementC__>;
			using RebindedAllocator_ = StandardTraits_::template rebind_alloc<Value_>;
			using RebindedAllocatorTraits_ = StandardTraits_::template rebind_traits<Value_>;
			RebindedAllocator_ rebinded_(this->standardAllocator);
			RebindedAllocatorTraits_::deallocate(rebinded_, static_cast<Value_*>(handle_), length_);
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	// using DefaultPool = ::dcool::resource::PoolFromClassic<::dcool::resource::DefaultClassicPool>;
	using DefaultPool = ::dcool::resource::PoolFromStandardAllocator<::std::allocator<::dcool::core::Byte>>;
}

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource, HasTypePool, ExtractedPoolType, Pool)

namespace dcool::resource {
	template <typename T_> using PoolType = ::dcool::resource::ExtractedPoolType<T_, ::dcool::resource::DefaultPool>;
}

#endif
