#ifndef DCOOL_RESOURCE_POOL_HPP_INCLUDED_
#	define DCOOL_RESOURCE_POOL_HPP_INCLUDED_ 1

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
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource::detail_, HasTypeHandle_, ExtractedHandleType_, Handle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource::detail_, HasTypeConstHandle_, ExtractedConstHandleType_, ConstHandle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource::detail_, HasTypeArrayHandle_, ExtractedArrayHandleType_, ArrayHandle)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::resource::detail_, HasTypeArrayConstHandle_, ExtractedArrayConstHandleType_, ArrayConstHandle
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
		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolHandleType_ = ::dcool::resource::detail_::ExtractedHandleType_<
			PoolT_, ::dcool::resource::detail_::ExtractedUnifiedHandleType_<
				PoolT_, decltype(::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>())
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using PoolConstHandleType_ = ::dcool::resource::detail_::ExtractedConstHandleType_<
			PoolT_,
			::dcool::resource::detail_::ExtractedUnifiedConstHandleType_<
				PoolT_,
				::dcool::core::PointedConstantizedType<::dcool::resource::detail_::PoolHandleType_<PoolT_, storageRequirementC_>>
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolHandleType_ = ::dcool::resource::detail_::ExtractedArrayHandleType_<
			PoolT_, ::dcool::resource::detail_::ExtractedUnifiedArrayHandleType_<
				PoolT_, decltype(
					::dcool::core::declval<PoolT_&>().template allocate<storageRequirementC_>(
						::dcool::core::declval<::dcool::core::SizeType<PoolT_>>()
					)
				)
			>
		>;

		template <
			typename PoolT_, ::dcool::core::StorageRequirement storageRequirementC_
		> using ArrayPoolConstHandleType_ = ::dcool::resource::detail_::ExtractedArrayConstHandleType_<
			PoolT_,
			::dcool::resource::detail_::ExtractedUnifiedArrayConstHandleType_<
				PoolT_,
				::dcool::core::PointedConstantizedType<::dcool::resource::detail_::ArrayPoolHandleType_<PoolT_, storageRequirementC_>>
			>
		>;

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
			::dcool::core::SizeType<T_> extra_
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
			::dcool::core::SizeType<PoolT_> extra_
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
			::dcool::core::SizeType<PoolT_> extra_
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

		public: [[nodiscard("Might leak memory.")]] static constexpr auto allocate(Pool& pool_, Length length_) -> ArrayHandle {
			return pool_.template allocate<storageRequirement>(length_);
		}

		public: using Super_::deallocate;

		public: static constexpr void deallocate(Pool& pool_, ArrayHandle handle_, Length length_) noexcept {
			pool_.deallocate<storageRequirement>(handle_, length_);
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
		typename ValueT_, typename PoolT_, typename LengthT_
	> [[nodiscard("Might leak memory.")]] constexpr auto adaptedAllocateFor(PoolT_& pool_, LengthT_ length_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::allocate(pool_, length_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_);
	}

	template <
		typename ValueT_, typename PoolT_, typename HandleT_, typename LengthT_
	> constexpr void adaptedDeallocateFor(PoolT_& pool_, HandleT_ handle_, LengthT_ length_) noexcept {
		::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::deallocate(pool_, handle_, length_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedHandleConverterFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::handleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedConstHandleConverterFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::constHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayHandleConverterFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::arrayHandleConverter(pool_);
	}

	template <typename ValueT_, typename PoolT_> constexpr auto adaptedArrayConstHandleConverterFor(PoolT_& pool_) {
		return ::dcool::resource::PoolAdaptorFor<PoolT_, ValueT_>::arrayConstHandleConverter(pool_);
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
		public: using Handle = UnifiedHandle;
		public: using ArrayHandle = Handle;
		public: using Size = ClassicPoolAdaptor_::Size;
		public: using Length = ClassicPoolAdaptor_::Size;
		public: using Difference = ::dcool::core::Difference;

		public: [[no_unique_address]] ClassicPool classicPool;

		template <
			::dcool::core::StorageRequirement storageRequirementC__
		> [[nodiscard("Might leak memory.")]] constexpr auto allocate(Length length_ = 1) noexcept -> Handle {
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

	using DefaultPool = ::dcool::resource::PoolFromClassic<::dcool::resource::DefaultClassicPool>;
}

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource, HasTypePool, ExtractedPoolType, Pool)

namespace dcool::resource {
	template <typename T_> using PoolType = ::dcool::resource::ExtractedPoolType<T_, ::dcool::resource::DefaultPool>;
}

#endif
