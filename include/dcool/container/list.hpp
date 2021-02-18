#ifndef DCOOL_CONTAINER_LIST_HPP_INCLUDED_
#	define DCOOL_CONTAINER_LIST_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <algorithm>
#	include <limits>
#	include <stdexcept>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::container::detail_, HasValueStuffed_, extractedStuffedValue_, stuffed
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::container::detail_, HasValueCircular_, extractedCircularValue_, circular
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::container::detail_, HasValueSqueezedCapacity_, extractedSqueezedCapacityValue_, squeezedCapacity
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::container::detail_, HasValueSqueezedOnly_, extractedSqueezedOnlyValue_, squeezedOnly
)

namespace dcool::container {
	using OutOfRange = ::std::out_of_range;

	namespace detail_ {
		template <
			::dcool::core::SignedMaxInteger maxIndexC_ = ::std::numeric_limits<::dcool::core::Difference>::max(),
			typename DistinguisherT_ = void
		> struct RandomAccessRangeLightIteratorBase_ {
			private: using Self_ = RandomAccessRangeLightIteratorBase_<maxIndexC_, DistinguisherT_>;
			private: static constexpr ::dcool::core::SignedMaxInteger maxIndex_ = maxIndexC_;

			public: using Length = ::dcool::core::IntegerType<maxIndex_>;
			public: using Index = Length;
			public: using Difference = ::dcool::core::IntegerType<maxIndex_, -maxIndex_>;
			public: static constexpr Index maxIndex = static_cast<Index>(maxIndex_);

			private: Index m_index_;

			public: constexpr RandomAccessRangeLightIteratorBase_() noexcept: Self_(::std::numeric_limits<Index>::max()) {
			}

			public: constexpr explicit RandomAccessRangeLightIteratorBase_(Index index_) noexcept: m_index_(index_) {
			}

			public: constexpr auto index() const noexcept -> Index {
				return this->m_index_;
			}

			public: constexpr void advance(Difference step_ = 1) noexcept {
				this->m_index_ += step_;
			}

			public: constexpr void retreat(Difference step_ = 1) noexcept {
				this->advance(-step_);
			}

			public: constexpr auto next(Difference step_ = 1) const noexcept -> Self_ {
				return Self_(static_cast<Difference>(this->index()) + step_);
			}

			public: constexpr auto previous(Difference step_ = 1) const noexcept -> Self_ {
				return this->next(-step_);
			}

			public: constexpr auto distanceTo(Self_ const& other_) const noexcept -> Difference {
				return static_cast<Difference>(other_.index()) - static_cast<Difference>(this->index());
			}

			public: friend constexpr auto operator <=>(
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::StrongOrdering = default;

			public: friend constexpr auto operator == (
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::Boolean = default;

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->advance();
				return *this;
			}

			public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->advance();
				return result_;
			}

			public: constexpr auto operator --() noexcept -> Self_& {
				this->retreat();
				return *this;
			}

			public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->retreat();
				return result_;
			}

			public: constexpr auto operator +=(Difference step_) noexcept -> Self_& {
				this->advance(step_);
				return *this;
			}

			public: friend constexpr auto operator +(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.next(step_);
			}

			public: friend constexpr auto operator +(Difference step_, Self_ const& iterator_) noexcept -> Self_ {
				return iterator_ + step_;
			}

			public: constexpr auto operator -=(Difference step_) noexcept -> Self_& {
				this->retreat(step_);
				return *this;
			}

			public: friend constexpr auto operator -(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.previous(step_);
			}

			public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Difference {
				return right_.distanceTo(left_);
			}
		};

		template <
			typename RangeChassisT_,
			::dcool::core::Boolean squeezedOnlyC_,
			::dcool::core::SignedMaxInteger maxIndexC_ = ::std::numeric_limits<::dcool::core::Difference>::max(),
			typename DistinguisherT_ = void
		> struct ListChassisLightIterator_: private ::dcool::container::detail_::RandomAccessRangeLightIteratorBase_<
			maxIndexC_, DistinguisherT_
		> {
			private: using Self_ = ListChassisLightIterator_<RangeChassisT_, squeezedOnlyC_, maxIndexC_, DistinguisherT_>;
			private: using Super_ = ::dcool::container::detail_::RandomAccessRangeLightIteratorBase_<maxIndexC_, DistinguisherT_>;
			public: using RangeChassis = RangeChassisT_;
			public: static constexpr ::dcool::core::Boolean squeezedOnly = squeezedOnlyC_;
			private: static constexpr ::dcool::core::SignedMaxInteger maxIndex_ = maxIndexC_;

			public: using typename Super_::Length;
			public: using typename Super_::Index;
			public: using typename Super_::Difference;
			public: using Super_::maxIndex;
			public: using Super_::Super_;
			public: using Super_::index;
			public: using Super_::advance;
			public: using Super_::retreat;
			public: using Value = ::dcool::core::IdenticallyConstType<::dcool::core::ValueType<RangeChassis>, RangeChassis>;
			public: using Engine = typename RangeChassis::Engine;

			public: constexpr ListChassisLightIterator_() noexcept = default;

			public: constexpr ListChassisLightIterator_(
				::dcool::container::detail_::ListChassisLightIterator_<
					::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnlyC_, maxIndexC_, DistinguisherT_
				> const& other_
			) noexcept: Self_(other_.index()) {
			}

			public: constexpr explicit ListChassisLightIterator_(Index index_) noexcept: Super_(index_) {
			}

			public: constexpr auto operator =(
				::dcool::container::detail_::ListChassisLightIterator_<
					::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnly, maxIndexC_, DistinguisherT_
				> const& other_
			) noexcept -> Self_& {
				static_cast<Super_&>(*this) = static_cast<
					::dcool::container::detail_::ListChassisLightIterator_<
						::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnly, maxIndexC_, DistinguisherT_
					>::Super_ const&
				>(other_);
				return *this;
			}

			public: constexpr auto next(Difference step_ = 1) const noexcept -> Self_ {
				return Self_(static_cast<Difference>(this->index()) + step_);
			}

			public: constexpr auto previous(Difference step_ = 1) const noexcept -> Self_ {
				return this->next(-step_);
			}

			public: template <
				::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
			> constexpr auto dereferenceSelfWith(RangeChassisT__&& rangeChassis_) const noexcept -> Value& {
				static_assert(squeezedOnly);
				return rangeChassis_.access(this->index());
			}

			public: template <
				::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
			> constexpr auto dereferenceSelfWith(RangeChassisT__&& rangeChassis_, Engine& engine_) const noexcept -> Value& {
				return rangeChassis_.access(engine_, this->index());
			}

			public: template <
				::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
			> constexpr auto rawPointerWith(RangeChassisT__&& rangeChassis_) const noexcept -> Value* {
				static_assert(squeezedOnly);
				return rangeChassis_.rawPointerAt(this->index());
			}

			public: template <
				::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
			> constexpr auto rawPointerWith(RangeChassisT__&& rangeChassis_, Engine& engine_) const noexcept -> Value* {
				return rangeChassis_.rawPointerAt(engine_, this->index());
			}

			public: constexpr auto distanceTo(Self_ const& other_) const noexcept -> Difference {
				return this->Super_::distanceTo(other_);
			}

			public: friend constexpr auto operator <=>(
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::StrongOrdering = default;

			public: friend constexpr auto operator == (
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::Boolean = default;

			public: constexpr auto operator +=(Difference step_) noexcept -> Self_& {
				this->advance(step_);
				return *this;
			}

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->advance();
				return *this;
			}

			public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->advance();
				return result_;
			}

			public: constexpr auto operator --() noexcept -> Self_& {
				this->retreat();
				return *this;
			}

			public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->retreat();
				return result_;
			}

			public: friend constexpr auto operator +(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.next(step_);
			}

			public: friend constexpr auto operator +(Difference step_, Self_ const& iterator_) noexcept -> Self_ {
				return iterator_ + step_;
			}

			public: constexpr auto operator -=(Difference step_) noexcept -> Self_& {
				this->retreat(step_);
				return *this;
			}

			public: friend constexpr auto operator -(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.previous(step_);
			}

			public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Difference {
				return right_.distanceTo(left_);
			}
		};

		template <
			typename RangeChassisT_,
			::dcool::core::Boolean squeezedOnly_,
			::dcool::core::SignedMaxInteger maxIndexC_ = ::std::numeric_limits<::dcool::core::Difference>::max(),
			typename DistinguisherT_ = void
		> struct ListChassisIterator_: private ::dcool::container::detail_::ListChassisLightIterator_<
			RangeChassisT_, squeezedOnly_, maxIndexC_, DistinguisherT_
		> {
			private: using Self_ = ListChassisIterator_<RangeChassisT_, squeezedOnly_, maxIndexC_, DistinguisherT_>;
			private: using Super_ = ::dcool::container::detail_::ListChassisLightIterator_<
				RangeChassisT_, squeezedOnly_, maxIndexC_, DistinguisherT_
			>;
			public: using RangeChassis = RangeChassisT_;
			private: static constexpr ::dcool::core::SignedMaxInteger maxIndex_ = maxIndexC_;

			public: using typename Super_::Length;
			public: using typename Super_::Index;
			public: using typename Super_::Difference;
			public: using typename Super_::Value;
			public: using typename Super_::Engine;
			public: using Super_::squeezedOnly;
			public: using Super_::maxIndex;
			public: using Super_::Super_;
			public: using Super_::index;
			public: using Super_::advance;
			public: using Super_::retreat;

			public: using value_type = Value;
			public: using difference_type = Difference;
			public: using pointer = Value*;
			public: using reference = Value&;
			public: using iterator_category = ::dcool::core::RandomAccessIteratorTag;

			private: RangeChassis* m_range_ = ::dcool::core::nullPointer;
			private: ::dcool::core::StaticOptional<Engine*, !squeezedOnly> m_engine_ = { .value = ::dcool::core::nullPointer };

			public: constexpr ListChassisIterator_() noexcept = default;

			public: constexpr ListChassisIterator_(
				::dcool::container::detail_::ListChassisIterator_<
					::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnly_, maxIndexC_, DistinguisherT_
				> const& other_
			) noexcept: Super_(other_.index()), m_range_(other_.m_range_), m_engine_{ .value = other_.m_engine_ } {
			}

			public: constexpr ListChassisIterator_(
				RangeChassis& range_, Index index_
			) noexcept: Super_(index_), m_range_(::dcool::core::addressOf(range_)) {
				static_assert(squeezedOnly);
			}

			public: constexpr ListChassisIterator_(
				RangeChassis& range_, Engine& engine_, Index index_
			) noexcept:
				Super_(index_), m_range_(::dcool::core::addressOf(range_)), m_engine_{ .value = ::dcool::core::addressOf(engine_) }
			{
			}

			public: constexpr auto operator =(
				::dcool::container::detail_::ListChassisIterator_<
					::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnly_, maxIndexC_, DistinguisherT_
				> const& other_
			) noexcept -> Self_& {
				this->Super_::operator =(
					static_cast<
						::dcool::container::detail_::ListChassisIterator_<
							::dcool::core::ConstRemovedType<RangeChassis>, squeezedOnly_, maxIndexC_, DistinguisherT_
						>::Super_ const&
					>(other_)
				);
				this->m_range_ = other_.m_range_;
				this->m_engine_ = other_.m_engine_;
				return *this;
			}

			public: constexpr auto next(Difference step_ = 1) const noexcept -> Self_ {
				Self_ result_;
				if constexpr (squeezedOnly) {
					result_ = Self_(*(this->m_range_), static_cast<Difference>(this->index()) + step_);
				} else {
					result_ = Self_(*(this->m_range_), *(this->m_engine_.value), static_cast<Difference>(this->index()) + step_);
				}
				return result_;
			}

			public: constexpr auto previous(Difference step_ = 1) const noexcept -> Self_ {
				return this->next(-step_);
			}

			public: constexpr auto dereferenceSelf() const noexcept -> Value& {
				Value* pointer_;
				if constexpr (squeezedOnly) {
					pointer_ = ::dcool::core::addressOf(this->dereferenceSelfWith(*(this->m_range_)));
				} else {
					pointer_ = ::dcool::core::addressOf(this->dereferenceSelfWith(*(this->m_range_), *(this->m_engine_.value)));
				}
				return *pointer_;
			}

			public: constexpr auto rawPointer() const noexcept -> Value* {
				Value* result_;
				if constexpr (squeezedOnly) {
					result_ = this->rawPointerWith(*(this->m_range_));
				} else {
					result_ = this->rawPointerWith(*(this->m_range_), *(this->m_engine_.value));
				}
				return result_;
			}

			public: constexpr auto operator *() const noexcept -> Value& {
				return this->dereferenceSelf();
			}

			public: constexpr auto operator ->() const noexcept -> Value* {
				return ::dcool::core::addressOf(this->dereferenceSelf());
			}

			public: constexpr auto distanceTo(Self_ const& other_) const noexcept -> Difference {
				return this->Super_::distanceTo(other_);
			}

			public: friend constexpr auto operator <=>(
				Self_ const& left_, Self_ const& right_
			) noexcept -> ::dcool::core::StrongOrdering {
				return static_cast<Super_ const&>(left_) <=> static_cast<Super_ const&>(right_);
			}

			public: friend constexpr auto operator == (
				Self_ const& left_, Self_ const& right_
			) noexcept -> ::dcool::core::Boolean {
				return static_cast<Super_ const&>(left_) == static_cast<Super_ const&>(right_);
			}

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->advance();
				return *this;
			}

			public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->advance();
				return result_;
			}

			public: constexpr auto operator --() noexcept -> Self_& {
				this->retreat();
				return *this;
			}

			public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_ = *this;
				this->retreat();
				return result_;
			}

			public: constexpr auto operator +=(Difference step_) noexcept -> Self_& {
				this->advance(step_);
				return *this;
			}

			public: friend constexpr auto operator +(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.next(step_);
			}

			public: friend constexpr auto operator +(Difference step_, Self_ const& iterator_) noexcept -> Self_ {
				return iterator_ + step_;
			}

			public: constexpr auto operator -=(Difference step_) noexcept -> Self_& {
				this->retreat(step_);
				return *this;
			}

			public: friend constexpr auto operator -(Self_ const& iterator_, Difference step_) noexcept -> Self_ {
				return iterator_.previous(step_);
			}

			public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Difference {
				return right_.distanceTo(left_);
			}
		};
	}

	template <typename ValueT_, typename ConfigT_> struct ListChassis;

	namespace detail_ {
		template <::dcool::core::Boolean squeezedOnlyC_, typename ConfigT_, typename ValueT_> struct ListConfigAdaptorBase_ {
			private: using Self_ = ListConfigAdaptorBase_<squeezedOnlyC_, ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			private: static constexpr ::dcool::core::Length squeezedCapacity_ =
				::dcool::container::detail_::extractedSqueezedCapacityValue_<Config>(static_cast<::dcool::core::Length>(0))
			;

			public: using Length = ::dcool::core::IntegerType<squeezedCapacity_>;

			public:	static constexpr ::dcool::core::Boolean squeezedOnly = true;
			public: static constexpr Length squeezedCapacity = squeezedCapacity_;

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, ::dcool::core::ComparableEmpty<Self_>>;
			public: static constexpr Length storageMaxCapacity = squeezedCapacity_;
		};

		template <
			typename ConfigT_, typename ValueT_
		> struct ListConfigAdaptorBase_<false, ConfigT_, ValueT_> {
			private: using Self_ = ListConfigAdaptorBase_<false, ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public:	static constexpr ::dcool::core::Boolean squeezedOnly = false;
			public: using Pool = ::dcool::resource::PoolType<Config>;

			private: struct DefaultEngine_ {
				private: using Self_ = DefaultEngine_;

				public: [[no_unique_address]] Pool partPool;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}

				public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
				public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::listPool'"
			);

			public: using PoolAdaptor = ::dcool::resource::PoolAdaptorFor<Pool, Value>;
			public: using Handle = PoolAdaptor::Handle;
			public: using Length = PoolAdaptor::Length;

			public: static constexpr Length storageMaxCapacity = ::std::numeric_limits<::dcool::core::Difference>::max();

			public: static constexpr Length squeezedCapacity =
				::dcool::container::detail_::extractedSqueezedCapacityValue_<Config>(::dcool::core::Length(0))
			;

			public: using BasicStorage = ::dcool::resource::Squeezer<Value, Handle, squeezedCapacity>;
		};

		template <
			typename ConfigT_, typename ValueT_
		> struct ListConfigAdaptor_: public ::dcool::container::detail_::ListConfigAdaptorBase_<
			::dcool::container::detail_::extractedSqueezedOnlyValue_<ConfigT_>(false),
			ConfigT_,
			ValueT_
		> {
			private: using Self_ = ListConfigAdaptor_<ConfigT_, ValueT_>;
			private: using Super_ = ::dcool::container::detail_::ListConfigAdaptorBase_<
				::dcool::container::detail_::extractedSqueezedOnlyValue_<ConfigT_>(false),
				ConfigT_,
				ValueT_
			>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using typename Super_::Length;
			public: using Super_::squeezedOnly;
			public: using Super_::storageMaxCapacity;

			public: using Index = Length;
			public: static constexpr ::dcool::core::Boolean stuffed = ::dcool::container::detail_::extractedStuffedValue_<Config>(
				false
			);
			public: static constexpr ::dcool::core::Boolean circular = ::dcool::container::detail_::extractedCircularValue_<Config>(
				false
			);
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				::dcool::core::exceptionSafetyStrategyOf<Config>
			;

			public: using Iterator = ::dcool::core::ConditionalType<
				circular,
				::dcool::container::detail_::ListChassisIterator_<
					::dcool::container::ListChassis<Value, Config>, squeezedOnly, storageMaxCapacity
				>,
				::dcool::core::ContaminatedPointer<Value>
			>;
			public: using ConstIterator = ::dcool::core::ConditionalType<
				circular,
				::dcool::container::detail_::ListChassisIterator_<
					::dcool::container::ListChassis<Value, Config> const, squeezedOnly, storageMaxCapacity
				>,
				::dcool::core::ContaminatedPointer<Value const>
			>;
			public: using ReverseIterator = ::dcool::core::ReverseIterator<Iterator>;
			public: using ReverseConstIterator = ::dcool::core::ReverseIterator<ConstIterator>;
		};
	}

	template <
		typename T_, typename ValueT_
	> concept SqueezedOnlyListConfig = ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::squeezedOnly;

	template <
		typename T_, typename ValueT_
	> concept StuffedListConfig = ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::stuffed;

	template <
		typename T_, typename ValueT_
	> concept CircularListConfig = ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::circular;

	template <typename T_, typename ValueT_> concept ListConfig =
		::dcool::container::SqueezedOnlyListConfig<T_, ValueT_> ||
		::dcool::resource::ArrayPoolFor<typename ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::Pool, ValueT_>
	;

	template <typename ConfigT_, typename ValueT_> requires ::dcool::container::ListConfig<
		ConfigT_, ValueT_
	> using ListConfigAdaptor = ::dcool::container::detail_::ListConfigAdaptor_<ConfigT_, ValueT_>;

	namespace detail_ {
		template <typename ValueT_, typename ConfigT_> struct ListChassisStorageBase_ {
			private: using Self_ = ListChassisStorageBase_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Pool = ConfigAdaptor_::Pool;
			public: using Handle = ConfigAdaptor_::Handle;
			public: using Length = ConfigAdaptor_::Length;
			public: using BasicStorage = ConfigAdaptor_::BasicStorage;
			public: static constexpr Length squeezedCapacity = ConfigAdaptor_::squeezedCapacity;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;
			public: static constexpr ::dcool::core::Boolean stuffed = ConfigAdaptor_::stuffed;

			private: BasicStorage m_storage_;
			private: Length m_capacity_;

			public: constexpr ListChassisStorageBase_() noexcept = default;
			public: ListChassisStorageBase_(Self_ const&) = delete;
			public: ListChassisStorageBase_(Self_&&) = delete;
			public: constexpr ~ListChassisStorageBase_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: constexpr void initialize(Engine& engine_) noexcept {
				if constexpr (squeezedCapacity > 0) {
					this->m_storage_.activateItems();
				} else {
					this->m_storage_.activateAlternative();
					if constexpr (
						::dcool::resource::PoolWithBijectiveHandleConverter<Pool, ::dcool::core::storageRequirementFor<Value>>
					) {
						this->m_storage_.alternative() = ::dcool::resource::adaptedToArrayHandleFor<Value>(
							engine_.pool(), ::dcool::core::nullPointer
						);
					}
				}
				if constexpr (stuffed) {
					this->m_capacity_ = 0;
				} else {
					this->m_capacity_ = squeezedCapacity;
				}
			}

			public: constexpr void initialize(Engine& engine_, Length capacity_) {
				if (capacity_ <= squeezedCapacity) {
					if constexpr (squeezedCapacity == 0) {
						this->initialize(engine_);
					}
					this->m_storage_.activateItems();
					if constexpr (stuffed) {
						this->m_capacity_ = capacity_;
					} else {
						this->m_capacity_ = squeezedCapacity;
					}
				} else {
					this->m_storage_.activateAlternative();
					this->m_storage_.alternative() = ::dcool::resource::adaptedAllocateFor<Value>(engine_.pool(), capacity_);
					this->m_capacity_ = capacity_;
				}
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				if (!(this->squeezed(engine_))) {
					::dcool::resource::adaptedDeallocateFor<Value>(
						engine_.pool(), this->m_storage_.alternative(), this->capacity(engine_)
					);
				}
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				other_.m_storage_.alternative() = this->m_storage_.alternative();
				other_.m_capacity_ = this->m_capacity_;
			}

			public: constexpr void swapWith(Self_& other_) noexcept {
				::dcool::core::intelliSwap(this->m_storage_.alternative(), other_.m_storage_.alternative());
				::dcool::core::intelliSwap(this->m_capacity_, other_.m_capacity_);
			}

			public: constexpr void setCapacityForStuffed(Length newCapacity_) noexcept {
				if constexpr (stuffed) {
					this->m_capacity_ = newCapacity_;
				}
			}

			public: constexpr void setCapacityForStuffed(Engine& engine_, Length newCapacity_) noexcept {
				this->setCapacityForStuffed(newCapacity_);
			}

			public: constexpr auto squeezed() const noexcept {
				if constexpr (squeezedCapacity > 0) {
					return this->capacity() <= squeezedCapacity;
				}
				return false;
			}

			public: constexpr auto squeezed(Engine& engine_) const noexcept {
				return this->squeezed();
			}

			public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
				return this->capacity();
			}

			public: constexpr auto capacity() const noexcept -> Length {
				return this->m_capacity_;
			}

			public: constexpr auto squeezedData() const noexcept -> Value const* {
				// Behavior is undefined if not squeezed
				return reinterpret_cast<Value const*>(::dcool::core::addressOf(this->m_storage_.items()));
			}

			public: constexpr auto squeezedData() noexcept -> Value* {
				// Behavior is undefined if not squeezed
				return reinterpret_cast<Value*>(::dcool::core::addressOf(this->m_storage_.items()));
			}

			public: constexpr auto squeezedData(Engine& engine_) const noexcept -> Value const* {
				// Behavior is undefined if not squeezed
				return this->squeezedData();
			}

			public: constexpr auto squeezedData(Engine& engine_) noexcept -> Value* {
				// Behavior is undefined if not squeezed
				return this->squeezedData();
			}

			public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
				if constexpr (squeezedCapacity > 0) {
					if (this->squeezed(engine_)) {
						return this->squeezedData();
					}
				}
				if constexpr (!::dcool::resource::PoolWithBijectiveHandleConverter<Pool, ::dcool::core::storageRequirementFor<Value>>) {
					if (this->capacity(engine_) == 0) {
						return ::dcool::core::nullPointer;
					}
				}
				return static_cast<Value const*>(
					::dcool::resource::adaptedFromArrayHandleFor<Value>(engine_.pool(), this->m_storage_.alternative())
				);
			}

			public: constexpr auto data(Engine& engine_) noexcept -> Value* {
				if constexpr (squeezedCapacity > 0) {
					if (this->squeezed(engine_)) {
						return this->squeezedData();
					}
				}
				if constexpr (!::dcool::resource::PoolWithBijectiveHandleConverter<Pool, ::dcool::core::storageRequirementFor<Value>>) {
					if (this->capacity(engine_) == 0) {
						return ::dcool::core::nullPointer;
					}
				}
				return static_cast<Value*>(
					::dcool::resource::adaptedFromArrayHandleFor<Value>(engine_.pool(), this->m_storage_.alternative())
				);
			}

			public: constexpr auto expandBack(Engine& engine_, Length extra_ = 1) -> ::dcool::core::Boolean {
				if constexpr (stuffed) {
					Length newCapacity_ = this->capacity(engine_) + extra_;
					if (newCapacity_ > this->capacity(engine_)) {
						return false;
					}
					this->m_capacity_ = newCapacity_;
					return true;
				}
				if (this->squeezed(engine_)) {
					return extra_ == 0;
				}
				if (::dcool::resource::adaptedExpandBackFor<Value>(
					engine_.pool(), this->m_storage_.alternative(), this->capacity(engine_), extra_)
				) {
					this->m_capacity_ += extra_;
					return true;
				}
				return false;
			}
		};

		template <
			typename ValueT_, ::dcool::container::SqueezedOnlyListConfig<ValueT_> ConfigT_
		> struct ListChassisStorageBase_<ValueT_, ConfigT_> {
			public: using Self_ = ListChassisStorageBase_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;
			public: static constexpr Length squeezedCapacity = ConfigAdaptor_::squeezedCapacity;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			private: using StorageType = ::dcool::core::StorageFor<Value[squeezedCapacity]>;

			private: StorageType m_storage_;

			public: constexpr ListChassisStorageBase_() noexcept = default;
			public: ListChassisStorageBase_(Self_ const&) = delete;
			public: ListChassisStorageBase_(Self_&&) = delete;
			public: constexpr ~ListChassisStorageBase_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: constexpr void initialize() noexcept {
			}

			public: constexpr void initialize(Engine& engine_) noexcept {
			}

			public: constexpr void uninitialize() noexcept {
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
			}

			public: constexpr void setCapacityForStuffed(Length newCapacity_) noexcept {
			}

			public: constexpr void setCapacityForStuffed(Engine& engine_, Length newCapacity_) noexcept {
				this->setCapacityForStuffed(newCapacity_);
			}

			public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
				return squeezedCapacity;
			}

			public: constexpr auto capacity() const noexcept -> Length {
				return squeezedCapacity;
			}

			public: constexpr auto squeezed() const noexcept {
				return true;
			}

			public: constexpr auto squeezed(Engine& engine_) const noexcept {
				return this->squeezed();
			}

			public: constexpr auto squeezedData() const noexcept -> Value const* {
				return reinterpret_cast<Value const*>(::dcool::core::addressOf(this->m_storage_));
			}

			public: constexpr auto squeezedData() noexcept -> Value* {
				return reinterpret_cast<Value*>(::dcool::core::addressOf(this->m_storage_));
			}

			public: constexpr auto squeezedData(Engine& engine_) const noexcept -> Value const* {
				return this->squeezedData();
			}

			public: constexpr auto squeezedData(Engine& engine_) noexcept -> Value* {
				return this->squeezedData();
			}

			public: constexpr auto data() const noexcept -> Value const* {
				return this->squeezedData();
			}

			public: constexpr auto data() noexcept -> Value* {
				return this->squeezedData();
			}

			public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
				return this->data();
			}

			public: constexpr auto data(Engine& engine_) noexcept -> Value* {
				return this->data();
			}
		};

		template <
			typename ValueT_, typename ConfigT_
		> struct ListChassisLengthRecordedStorage_: public ::dcool::container::detail_::ListChassisStorageBase_<ValueT_, ConfigT_> {
			private: using Self_ = ListChassisLengthRecordedStorage_<ValueT_, ConfigT_>;
			private: using Super_ = ::dcool::container::detail_::ListChassisStorageBase_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;

			private: Length m_length_;

			public: template <typename... ArgumentTs__> constexpr void initialize(ArgumentTs__&&... parameters_) noexcept {
				this->Super_::initialize(::dcool::core::forward<ArgumentTs__>(parameters_)...);
				this->m_length_ = 0;
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				this->Super_::relocateTo(other_);
				other_.setLength(this->length());
			}

			public: constexpr void swapWith(Self_& other_) noexcept {
				this->Super_::swapWith(other_);
				::dcool::core::intelliSwap(this->m_length_, other_.m_length_);
			}

			public: constexpr auto length(Engine& engine_) const noexcept {
				return this->length();
			}

			public: constexpr auto length() const noexcept {
				return this->m_length_;
			}

			public: constexpr void setLength(Engine& engine_, Length newLength_) noexcept {
				this->setLength(newLength_);
			}

			public: constexpr void setLength(Length newLength_) noexcept {
				this->m_length_ = newLength_;
			}
		};

		template <
			typename ValueT_, ::dcool::container::StuffedListConfig<ValueT_> ConfigT_
		> struct ListChassisLengthRecordedStorage_<
			ValueT_, ConfigT_
		>: public ::dcool::container::detail_::ListChassisStorageBase_<ValueT_, ConfigT_> {
			private: using Self_ = ListChassisLengthRecordedStorage_<ValueT_, ConfigT_>;
			private: using Super_ = ::dcool::container::detail_::ListChassisStorageBase_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;
			public: using Index = ConfigAdaptor_::Index;

			public: constexpr auto length(Engine& engine_) const noexcept {
				return this->length();
			}

			public: constexpr auto length() const noexcept {
				return this->capacity();
			}

			public: constexpr void setLength(Engine& engine_, Index newLength_) noexcept {
				this->setLength(newLength_);
			}

			// New length should be equal to capacity.
			public: constexpr void setLength(Index newLength_) noexcept {
			}
		};

		template <
			typename ValueT_, typename ConfigT_
		> struct ListChassisBeginOffsettedStorage_: public ::dcool::container::detail_::ListChassisLengthRecordedStorage_<
			ValueT_, ConfigT_
		> {
			private: using Self_ = ListChassisBeginOffsettedStorage_<ValueT_, ConfigT_>;
			private: using Super_ = ::dcool::container::detail_::ListChassisLengthRecordedStorage_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;
			public: using Index = ConfigAdaptor_::Index;

			public: constexpr auto beginOffset(Engine& engine_) const noexcept -> Length {
				return this->beginOffset();
			}

			public: constexpr auto beginOffset() const noexcept -> Length {
				return 0;
			}

			public: constexpr void setBeginOffset(Engine& engine_, Length newBeginOffset_) noexcept {
				this->setBeginOffset(newBeginOffset_);
			}

			// New begin offset should always be 0.
			public: constexpr void setBeginOffset(Length newBeginOffset_) noexcept {
			}
		};

		template <
			typename ValueT_, ::dcool::container::CircularListConfig<ValueT_> ConfigT_
		> struct ListChassisBeginOffsettedStorage_<
			ValueT_, ConfigT_
		>: public ::dcool::container::detail_::ListChassisLengthRecordedStorage_<ValueT_, ConfigT_> {
			private: using Self_ = ListChassisBeginOffsettedStorage_<ValueT_, ConfigT_>;
			private: using Super_ = ::dcool::container::detail_::ListChassisLengthRecordedStorage_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;

			public: Length m_beginOffset_;

			public: template <typename... ArgumentTs__> constexpr void initialize(ArgumentTs__&&... parameters_) noexcept {
				this->Super_::initialize(::dcool::core::forward<ArgumentTs__>(parameters_)...);
				this->m_beginOffset_ = 0;
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				this->Super_::relocateTo(other_);
				other_.setBeginOffset(this->beginOffset());
			}

			public: constexpr void swapWith(Self_& other_) noexcept {
				this->Super_::swapWith(other_);
				::dcool::core::intelliSwap(this->m_beginOffset_, other_.m_beginOffset_);
			}

			public: constexpr auto beginOffset(Engine& engine_) const noexcept -> Length {
				return this->beginOffset();
			}

			public: constexpr auto beginOffset() const noexcept -> Length {
				return this->m_beginOffset_;
			}

			public: constexpr void setBeginOffset(Engine& engine_, Length newBeginOffset_) noexcept {
				this->setBeginOffset(newBeginOffset_);
			}

			// New begin index should always be 0.
			public: constexpr void setBeginOffset(Length newBeginOffset_) noexcept {
				this->m_beginOffset_ = newBeginOffset_;
			}
		};

		template <
			typename ValueT_, typename ConfigT_
		> using ListChassisStorage_ = ::dcool::container::detail_::ListChassisBeginOffsettedStorage_<ValueT_, ConfigT_>;
	}

	template <typename ValueT_, typename ConfigT_ = ::dcool::core::Empty<>> struct ListChassis {
		static_assert(::dcool::container::ListConfig<ConfigT_, ValueT_>);
		private: using Self_ = ListChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Length = ConfigAdaptor_::Length;
		public: using Index = ConfigAdaptor_::Index;
		public: using Iterator = ConfigAdaptor_::Iterator;
		public: using ConstIterator = ConfigAdaptor_::ConstIterator;
		public: using ReverseIterator = ConfigAdaptor_::ReverseIterator;
		public: using ReverseConstIterator = ConfigAdaptor_::ReverseConstIterator;
		public: static constexpr ::dcool::core::Boolean squeezedOnly = ConfigAdaptor_::squeezedOnly;
		public: static constexpr Length squeezedCapacity = ConfigAdaptor_::squeezedCapacity;
		public: static constexpr ::dcool::core::Boolean stuffed = ConfigAdaptor_::stuffed;
		public: static constexpr ::dcool::core::Boolean circular = ConfigAdaptor_::circular;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;
		private: using StorageType_ = ::dcool::container::detail_::ListChassisStorage_<ValueT_, ConfigT_>;

		private: StorageType_ m_storage_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		public: constexpr ListChassis() noexcept = default;
		public: ListChassis(Self_ const&) = delete;
		public: ListChassis(Self_&&) = delete;
		public: constexpr ~ListChassis() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: static consteval auto noThrowFillableByDefault_() noexcept -> ::dcool::core::Boolean {
			if constexpr (stuffed) {
				return noexcept(new (::dcool::core::declval<Value*>()) Value);
			}
			return true;
		}

		private: constexpr void fillByDefault_() noexcept(noThrowFillableByDefault_()) {
			if constexpr (stuffed) {
				Value* begin_ = this->m_storage_.data();
				::dcool::core::batchDefaultInitialize(begin_, begin_ + this->m_storage_.length());
			}
		}

		private: constexpr void fillByDefault_(Engine& engine_) noexcept(noThrowFillableByDefault_()) {
			if constexpr (stuffed) {
				::dcool::core::batchDefaultInitialize(this->begin(engine_), this->end(engine_));
			}
		}

		private: constexpr void initializeWithoutFill_() noexcept {
			static_assert(squeezedOnly);
			this->m_storage_.initialize();
		}

		private: constexpr void initialize_(Engine& engine_) {
			this->m_storage_.initialize(engine_);
			try {
				this->fillByDefault_(engine_);
			} catch (...) {
				this->uninitializeWithoutFilled_(engine_);
				throw;
			}
		}

		private: constexpr void initializeWithoutFill_(Engine& engine_) noexcept {
			this->initialize_(engine_);
		}

		private: constexpr void initializeWithoutFill_(Engine& engine_, Length capacity_) {
			static_assert(!squeezedOnly, "Attempted to change fixed capacity of a 'dcool::container::List'.");
			this->m_storage_.initialize(engine_, capacity_);
		}

		public: constexpr void initialize(Engine& engine_) noexcept(
			noexcept(::dcool::core::declval<Self_&>().fillByDefault_(engine_))
		) {
			this->initializeWithoutFill_(engine_);
			if constexpr (noexcept(this->fillByDefault_(engine_))) {
				this->fillByDefault_(engine_);
			} else {
				try {
					this->fillByDefault_(engine_);
				} catch (...) {
					this->uninitializeWithoutFilled_(engine_);
					throw;
				}
			}
		}

		public: template <::dcool::core::InputIterator IteratorT__> constexpr void initialize(
			Engine& engine_, IteratorT__ iterator_, Length count_
		) {
			this->initializeWithoutFill_(engine_, count_);
			try {
				Value* current_ = ::std::uninitialized_copy_n(iterator_, count_, this->data(engine_));
				Value* end_ = this->data(engine_) + this->capacity(engine_);
				if constexpr (stuffed && squeezedOnly) {
					::dcool::core::batchDefaultInitialize(current_, end_);
				}
			} catch (...) {
				this->uninitializeWithoutFilled_(engine_);
				throw;
			}
			this->m_storage_.setLength(engine_, count_);
		}

		public: constexpr void initialize(Engine& engine_, Length capacity_) noexcept(
			noexcept(::dcool::core::declval<Self_&>().fillByDefault_(engine_))
		) {
			this->initializeWithoutFill_(engine_, capacity_);
			try {
				this->fillByDefault_(engine_);
			} catch (...) {
				this->uninitializeWithoutFilled_(engine_);
				throw;
			}
		}

		private: constexpr void uninitializeWithoutFilled_() noexcept {
			static_assert(squeezedOnly);
			this->m_storage_.uninitialize();
		}

		private: constexpr void uninitializeWithoutFilled_(Engine& engine_) noexcept {
			this->m_storage_.uninitialize(engine_);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			::dcool::core::batchDestruct(this->begin(engine_), this->end(engine_));
			this->uninitializeWithoutFilled_(engine_);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void squeezedRelocateTo_(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			other_.initializeWithoutFill_(otherEngine_);
			try {
				::dcool::core::batchRelocate<strategyC__>(
					this->begin(engine_), this->begin(engine_) + this->length(engine_), other_.begin(otherEngine_)
				);
			} catch (...) {
				other_.uninitializeWithoutFilled_(otherEngine_);
				throw;
			}
			other_.m_storage_.setLength(otherEngine_, this->length(engine_));
			other_.m_storage_.setCapacityForStuffed(otherEngine_, this->length(engine_));
			this->uninitializeWithoutFilled_(engine_);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void unsqueezedRelocateTo_(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			this->m_storage_.relocateTo(other_.m_storage_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			if constexpr (squeezedOnly) {
				this->squeezedRelocateTo_(engine_, otherEngine_, other_);
			} else {
				if constexpr (squeezedCapacity > 0) {
					if (this->squeezed(engine_) || engine_ != otherEngine_) {
						this->squeezedRelocateTo_(engine_, otherEngine_, other_);
						return;
					}
				}
				this->unsqueezedRelocateTo_(engine_, otherEngine_, other_);
			}
		}

		public: template <typename ValueT__, typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::container::ListChassis<ValueT__, ConfigT__>::Engine& otherEngine_,
			::dcool::container::ListChassis<ValueT__, ConfigT__>& other_
		) const {
			if constexpr (other_.squeezedOnly) {
				other_.initializeWithoutFill_(otherEngine_);
			} else {
				other_.initializeWithoutFill_(otherEngine_, this->length(engine_));
			}
			try {
				::dcool::core::batchCopy(this->begin(engine_), this->end(engine_), other_.begin(otherEngine_));
				other_.m_storage_.setLength(otherEngine_, this->length(engine_));
			} catch (...) {
				other_.uninitializeWithoutFilled_(otherEngine_);
				throw;
			}
		}

		private: template <::dcool::core::ExceptionSafetyStrategy strategyC__> constexpr void itemWiseSwapWith_(
			Engine& engine_, Engine& otherEngine_, Self_& other_
		) {
			Length selfLength_ = this->length(engine_);
			Length otherLength_ = other_.length(otherEngine_);
			Length commonLength_ = ::dcool::core::min(selfLength_, otherLength_);
			Iterator selfBegin_ = this->begin(engine_);
			Iterator otherBegin_ = other_.begin(otherEngine_);
			Index index_ = 0;
			try {
				while (index_ < commonLength_) {
					::dcool::core::intelliSwap(
						::dcool::core::dereference(selfBegin_ + index_), ::dcool::core::dereference(otherBegin_ + index_)
					);
				}
				// At least one of the following call of 'batchRelocate' will be no-op.
				::dcool::core::batchRelocate<strategyC__>(
					selfBegin_ + commonLength_, selfBegin_ + selfLength_, otherBegin_ + commonLength_
				);
				::dcool::core::batchRelocate<strategyC__>(
					otherBegin_ + commonLength_, otherBegin_ + other_.m_storage_.length(otherEngine_), selfBegin_ + commonLength_
				);
				Length middleMan_ = selfLength_;
				this->m_storage_.setLength(otherLength_);
				this->m_storage_.setCapacityForStuffed(engine_, this->length(engine_));
				other_.m_storage_.setLength(middleMan_);
				other_.m_storage_.setCapacityForStuffed(otherEngine_, this->length(engine_));
			} catch (...) {
				try {
					while (index_ > 0) {
						--index_;
						::dcool::core::dereference(selfBegin_ + index_), ::dcool::core::dereference(otherBegin_ + index_);
					}
				} catch (...) {
					::dcool::core::goWeak<strategyC__>();
				}
				throw;
			}
		}

		// Self should not be squeezed
		public: template <::dcool::core::ExceptionSafetyStrategy strategyC__> constexpr void swapWithSqueezed(
			Engine& engine_, Engine& otherEngine_, Self_& squeezedOther_
		) {
			static_assert(!squeezedOnly);
			Self_ middleMan_;
			this->unsqueezedRelocateTo_(engine_, engine_, middleMan_);
			this->initializeWithoutFill_(engine_);
			try {
				::dcool::core::batchRelocate<strategyC__>(
					squeezedOther_.begin(otherEngine_), squeezedOther_.end(otherEngine_), this->m_storage_.squeezedData(engine_)
				);
			} catch (...) {
				this->uninitializeWithoutFilled_(engine_);
				middleMan_.unsqueezedRelocateTo_(engine_, engine_, *this);
				throw;
			}
			this->m_storage_.setLength(engine_, squeezedOther_.length(otherEngine_));
			this->m_storage_.setCapacityForStuffed(engine_, squeezedOther_.length(otherEngine_));
			squeezedOther_.uninitializeWithoutFilled_(otherEngine_);
			middleMan_.unsqueezedRelocateTo_(engine_, otherEngine_, squeezedOther_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(
			Engine& engine_, Engine& otherEngine_, Self_& other_
		) {
			if constexpr (squeezedOnly) {
				this->itemWiseSwapWith_<strategyC__>(engine_, otherEngine_, other_);
			} else {
				if constexpr (!engineWillBeSwappedC__) {
					if (engine_ != otherEngine_) {
						this->itemWiseSwapWith_<strategyC__>(engine_, otherEngine_, other_);
						return;
					}
				}
				if constexpr (squeezedCapacity > 0) {
					if (this->squeezed(engine_)) {
						if (other_.squeezed(otherEngine_)) {
							this->itemWiseSwapWith_<strategyC__>(engine_, otherEngine_, other_);
						} else {
							other_.swapWithSqueezed<strategyC__>(otherEngine_, engine_, *this);
						}
						return;
					} else if (other_.squeezed(otherEngine_)) {
						this->swapWithSqueezed<strategyC__>(otherEngine_, engine_, other_);
						return;
					}
				}
				this->m_storage_.swapWith(other_.m_storage_);
			}
		}

		public: constexpr auto squeezed(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->m_storage_.squeezed(engine_);
		}

		public: constexpr auto vacant(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->length(engine_) == 0;
		}

		public: constexpr auto full(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->length(engine_) == this->capacity(engine_);
		}

		private: constexpr auto contiguous_() const noexcept -> ::dcool::core::Boolean {
			if constexpr (circular) {
				return this->m_storage_.length() <= this->frontPartCapacity_();
			}
			return true;
		}

		public: constexpr auto contiguous(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			if constexpr (circular) {
				return this->length(engine_) <= this->frontPartCapacity_(engine_);
			}
			return true;
		}

		public: constexpr auto length(Engine& engine_) const noexcept -> Length {
			return this->m_storage_.length(engine_);
		}

		public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
			return this->m_storage_.capacity(engine_);
		}

		public: constexpr auto leftOver(Engine& engine_) const noexcept -> Length {
			return this->capacity(engine_) - this->length(engine_);
		}

		private: constexpr auto data() const noexcept -> Value const* {
			static_assert(squeezedOnly);
			return this->m_storage_.data();
		}

		private: constexpr auto data() noexcept -> Value* {
			static_assert(squeezedOnly);
			return this->m_storage_.data();
		}

		public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
			return this->m_storage_.data(engine_);
		}

		public: constexpr auto data(Engine& engine_) noexcept -> Value* {
			return this->m_storage_.data(engine_);
		}

		public: constexpr auto begin(Engine& engine_) const noexcept -> ConstIterator {
			ConstIterator result_;
			if constexpr (circular) {
				result_ = ConstIterator(*this, engine_, 0);
			} else {
				result_ = ConstIterator(this->data(engine_));
			}
			return result_;
		}

		public: constexpr auto begin(Engine& engine_) noexcept -> Iterator {
			Iterator result_;
			if constexpr (circular) {
				result_ = Iterator(*this, engine_, 0);
			} else {
				result_ = Iterator(this->data(engine_));
			}
			return result_;
		}

		public: constexpr auto end(Engine& engine_) const noexcept -> ConstIterator {
			return this->begin(engine_) + this->length(engine_);
		}

		public: constexpr auto end(Engine& engine_) noexcept -> Iterator {
			return this->begin(engine_) + this->length(engine_);
		}

		public: constexpr auto reverseBegin(Engine& engine_) const noexcept -> ReverseConstIterator {
			return ::dcool::core::makeReverseIterator(this->end(engine_));
		}

		public: constexpr auto reverseBegin(Engine& engine_) noexcept -> ReverseIterator {
			return ::dcool::core::makeReverseIterator(this->end(engine_));
		}

		public: constexpr auto reverseEnd(Engine& engine_) const noexcept -> ReverseConstIterator {
			return ::dcool::core::makeReverseIterator(this->begin(engine_));
		}

		public: constexpr auto reverseEnd(Engine& engine_) noexcept -> ReverseIterator {
			return ::dcool::core::makeReverseIterator(this->begin(engine_));
		}

		public: constexpr auto position(Engine& engine_, Index index_) const noexcept -> ConstIterator {
			return this->begin(engine_) + index_;
		}

		public: constexpr auto position(Engine& engine_, Index index_) noexcept -> Iterator {
			return this->begin(engine_) + index_;
		}

		private: constexpr auto frontPartCapacity_() const noexcept -> Length {
			return this->m_storage_.capacity() - this->m_storage_.beginOffset();
		}

		private: constexpr auto frontPartCapacity_(Engine& engine_) const noexcept -> Length {
			return this->capacity(engine_) - this->m_storage_.beginOffset(engine_);
		}

		private: constexpr auto backPartLength_() const noexcept -> Length {
			if (this->contiguous()) {
				return 0;
			}
			return this->m_storage_.length() - this->frontPartCapacity_();
		}

		private: constexpr auto backPartLength_(Engine& engine_) const noexcept -> Length {
			if (this->contiguous(engine_)) {
				return 0;
			}
			return this->length(engine_) - this->frontPartCapacity_(engine_);
		}

		private: constexpr auto itemOffset_(Index index_) const noexcept -> Length {
			Length lengthOfFrontPart_ = this->frontPartCapacity_();
			if (index_ < lengthOfFrontPart_) {
				return this->m_storage_.beginOffset() + index_;
			}
			return index_ - lengthOfFrontPart_;
		}

		private: constexpr auto itemOffset_(Engine& engine_, Index index_) const noexcept -> Length {
			Length lengthOfFrontPart_ = this->frontPartCapacity_(engine_);
			if (index_ < lengthOfFrontPart_) {
				return this->m_storage_.beginOffset(engine_) + index_;
			}
			return index_ - lengthOfFrontPart_;
		}

		public: constexpr auto rawPointerAt(Index index_) const noexcept -> Value const* {
			Value const* result_;
			if constexpr (circular) {
				result_ = this->data() + this->itemOffset_(index_);
			} else {
				result_ = this->data() + index_;
			}
			return result_;
		}

		public: constexpr auto rawPointerAt(Index index_) noexcept -> Value* {
			Value* result_;
			if constexpr (circular) {
				result_ = this->data() + this->itemOffset_(index_);
			} else {
				result_ = this->data() + index_;
			}
			return result_;
		}

		public: constexpr auto rawPointerAt(Engine& engine_, Index index_) const noexcept -> Value const* {
			Value const* result_;
			if constexpr (circular) {
				result_ = this->data(engine_) + this->itemOffset_(engine_, index_);
			} else {
				result_ = this->data(engine_) + index_;
			}
			return result_;
		}

		public: constexpr auto rawPointerAt(Engine& engine_, Index index_) noexcept -> Value* {
			Value* result_;
			if constexpr (circular) {
				result_ = this->data(engine_) + this->itemOffset_(engine_, index_);
			} else {
				result_ = this->data(engine_) + index_;
			}
			return result_;
		}

		public: constexpr auto access(Index index_) const noexcept -> Value const& {
			static_assert(squeezedOnly);
			::dcool::core::ContaminatedPointer<Value const> pointer_;
			if constexpr (circular) {
				pointer_ = ::dcool::core::ContaminatedPointer<Value const>(this->data() + this->itemOffset_(index_));
			} else {
				pointer_ = ::dcool::core::ContaminatedPointer<Value const>(this->data() + index_);
			}
			return ::dcool::core::dereference(pointer_);
		}

		public: constexpr auto access(Index index_) noexcept -> Value& {
			static_assert(squeezedOnly);
			::dcool::core::ContaminatedPointer<Value> pointer_;
			if constexpr (circular) {
				pointer_ = ::dcool::core::ContaminatedPointer<Value>(this->data() + this->itemOffset_(index_));
			} else {
				pointer_ = ::dcool::core::ContaminatedPointer<Value>(this->data() + index_);
			}
			return ::dcool::core::dereference(pointer_);
		}

		public: constexpr auto access(Engine& engine_, Index index_) const noexcept -> Value const& {
			::dcool::core::ContaminatedPointer<Value const> pointer_;
			if constexpr (circular) {
				pointer_ = ::dcool::core::ContaminatedPointer<Value const>(this->data(engine_) + this->itemOffset_(engine_, index_));
			} else {
				pointer_ = this->position(engine_, index_);
			}
			return ::dcool::core::dereference(pointer_);
		}

		public: constexpr auto access(Engine& engine_, Index index_) noexcept -> Value& {
			::dcool::core::ContaminatedPointer<Value> pointer_;
			if constexpr (circular) {
				pointer_ = ::dcool::core::ContaminatedPointer<Value>(this->data(engine_) + this->itemOffset_(engine_, index_));
			} else {
				pointer_ = this->position(engine_, index_);
			}
			return ::dcool::core::dereference(pointer_);
		}

		public: constexpr auto front(Engine& engine_) const noexcept -> Value const& {
			return this->access(engine_, 0);
		}

		public: constexpr auto front(Engine& engine_) noexcept -> Value& {
			return this->access(engine_, 0);
		}

		public: constexpr auto back(Engine& engine_) const noexcept -> Value const& {
			return this->access(engine_, this->length(engine_) - 1);
		}

		public: constexpr auto back(Engine& engine_) noexcept -> Value& {
			return this->access(engine_, this->length(engine_) - 1);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto expandBack_(Engine& engine_, Length extra_) -> ::dcool::core::Boolean {
			if constexpr (circular) {
				if (!(this->contiguous(engine_))) {
					if constexpr (::dcool::core::atAnyCost(strategyC__) && (!::dcool::core::isNoThrowRelocatable<Value>)) {
						return false;
					}
					Length oldCapacity_ = this->capacity(engine_);
					Length oldFrontPartLength_ = this->frontPartCapacity_(engine_);
					Length oldBackPartLength_ = this->backPartLength_(engine_);
					if (!(this->m_storage_.expandBack(engine_, extra_))) {
						return false;
					}
					if (oldFrontPartLength_ < oldBackPartLength_) {
						::dcool::core::ContaminatedPointer<Value> beginToMove_(this->data(engine_) + this->m_storage_.beginOffset());
						::dcool::core::ContaminatedPointer<Value> endToMove_ = beginToMove_ + oldFrontPartLength_;
						Value* endOfDestination_ = this->data(engine_) + this->capacity(engine_);
						::dcool::core::batchRelocateForward<strategyC__>(
							::dcool::core::makeReverseIterator(endToMove_),
							::dcool::core::makeReverseIterator(beginToMove_),
							::dcool::core::makeReverseIterator(endOfDestination_)
						);
					} else {
						::dcool::core::ContaminatedPointer<Value> beginToMove_(this->data(engine_));
						::dcool::core::ContaminatedPointer<Value> endToMove_ =
							beginToMove_ + ::dcool::core::min(oldFrontPartLength_, extra_)
						;
						Value* beginOfDestination_ = beginToMove_ + oldCapacity_;
						::dcool::core::ContaminatedPointer<Value> endOfBackPart_ = beginToMove_ + oldFrontPartLength_;
						::dcool::core::batchRelocate<strategyC__>(beginToMove_, endToMove_, beginOfDestination_);
						try {
							::dcool::core::batchRelocate<strategyC__>(endToMove_, endOfBackPart_, this->data(engine_));
						} catch (...) {
							::dcool::core::goWeak<strategyC__>();
							::dcool::core::batchDestruct(endToMove_, endOfBackPart_);
							throw;
						}
					}
					return true;
				}
			}
			return this->m_storage_.expandBack(engine_, extra_);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void forceExpandBack_(Engine& engine_, Length extra_) {
			static_assert(!squeezedOnly, "Attempted to change fixed capacity of a 'dcool::container::List'.");
			if (extra_ == 0) {
				return;
			}
			if (this->m_storage_.expandBack(engine_, extra_)) {
				return;
			}
			StorageType_ newStorage_;
			Length newCapacity_ = this->capacity(engine_) + extra_;
			newStorage_.initialize(engine_, newCapacity_);
			try {
				::dcool::core::batchRelocate<strategyC__>(this->begin(engine_), this->end(engine_), newStorage_.data(engine_));
			} catch (...) {
				newStorage_.uninitialize(engine_);
				throw;
			}
			this->m_storage_.uninitialize(engine_);
			newStorage_.relocateTo(this->m_storage_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void forceExpandBack(Engine& engine_, Length extra_) {
			static_assert(!stuffed, "Attempted to change capacity of a stuffed 'dcool::container::List' without adding new items.");
			this->forceExpandBack_(engine_, extra_);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void reserve_(Engine& engine_, Length expectedCapacity_) {
			if (expectedCapacity_ > this->capacity(engine_)) {
				this->forceExpandBack_<strategyC__>(engine_, expectedCapacity_ - this->capacity(engine_));
			}
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void reserve(Engine& engine_) {
			static_assert((!squeezedOnly) && (!stuffed));
			this->forceExpandBack<strategyC__>(engine_, ::dcool::core::max(this->capacity(engine_), 1));
		}

		// This function will leave the gap uninitialized, making the list invalid.
		private: public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void makeRoom_(
			Engine& engine_, Iterator gapBegin_, Length gapLength_ = 1
		) noexcept(::dcool::core::isNoThrowRelocatable<Value>) {
			Length newLength_ = this->length(engine_) + gapLength_;
			if constexpr (circular) {
				if (gapBegin_.index() * 2 < this->length(engine_)) {
					Length oldOffset_ = this->m_storage_.beginOffset();
					if (oldOffset_ >= gapLength_) {
						this->m_storage_.setBeginOffset(oldOffset_ - gapLength_);
					} else {
						this->m_storage_.setBeginOffset(this->capacity(engine_) - (gapLength_ - oldOffset_));
					}
					if constexpr (::dcool::core::isNoThrowRelocatable<Value>) {
						::dcool::core::batchRelocateForward<strategyC__>(
							this->begin(engine_) + gapLength_, gapBegin_ + gapLength_, this->begin(engine_)
						);
					} else {
						try {
							::dcool::core::batchRelocateForward<strategyC__>(
								this->begin(engine_) + gapLength_, gapBegin_ + gapLength_, this->begin(engine_)
							);
						} catch (...) {
							this->m_storage_.setBeginOffset(oldOffset_);
							throw;
						}
					}
					this->m_storage_.setLength(engine_, newLength_);
					return;
				}
			}
			::dcool::core::batchRelocateForward<strategyC__>(
				this->reverseBegin(engine_),
				::dcool::core::makeReverseIterator(gapBegin_),
				::dcool::core::makeReverseIterator(this->begin(engine_) + newLength_)
			);
			this->m_storage_.setLength(engine_, newLength_);
		}

		// This function is intended for recover a valid state after 'makeRoom_'. Failure of recovery would be considered fatal.
		private: public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void reclaimRoom_(Engine& engine_, Iterator gapBegin_, Length gapLength_ = 1) noexcept {
			::dcool::core::batchRelocateForward<strategyC__>(
				gapBegin_ + gapLength_, this->end(engine_), this->begin(engine_) + (gapBegin_ - this->begin(engine_))
			);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto braveEmplace(Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_) -> Iterator {
			this->makeRoom_<strategyC__>(engine_, position_);
			try {
				new (::dcool::core::rawPointerOf(position_)) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
			} catch (...) {
				this->reclaimRoom_<strategyC__>(engine_, position_);
				throw;
			}
			return position_;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto emplace(Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_) -> Iterator {
			if (this->full(engine_)) {
				if constexpr (!squeezedOnly) {
					Length extraCapacity_ = (((!stuffed) && this->capacity(engine_) > 0) ? this->capacity(engine_) : 1);
					if (this->m_storage_.expandBack(engine_, extraCapacity_)) {
						return this->braveEmplace<strategyC__>(engine_, position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
					}
					Self_ newChassis_;
					Length newCapacity_ = this->capacity(engine_) + extraCapacity_;
					newChassis_.initializeWithoutFill_(engine_, newCapacity_);
					Length lengthBeforePosition_ = static_cast<Length>(position_ - this->begin(engine_));
					Iterator newPosition_ = newChassis_.position(engine_, lengthBeforePosition_);
					try {
						new (::dcool::core::rawPointerOf(newPosition_)) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
					} catch (...) {
						newChassis_.uninitializeWithoutFilled_(engine_);
						throw;
					}
					try {
						if constexpr (::dcool::core::atAnyCost(strategyC__) && !(::dcool::core::isNoThrowMoveConstructible<Value>)) {
							::std::uninitialized_copy(this->begin(engine_), position_, newChassis_.begin(engine_));
							try {
								::dcool::core::batchRelocate<strategyC__>(
									position_, this->end(engine_), ::dcool::core::rawPointerOf(newPosition_) + 1
								);
							} catch (...) {
								::dcool::core::batchDestruct(newChassis_.begin(engine_), newPosition_);
								throw;
							}
						} else {
							::dcool::core::batchMoveConstruct<strategyC__>(this->begin(engine_), position_, newChassis_.begin(engine_));
							try {
								::dcool::core::batchRelocate<strategyC__>(position_, this->end(engine_), newPosition_ + 1);
							} catch (...) {
								try {
									::dcool::core::batchMove(newChassis_.begin(engine_), newPosition_, this->begin(engine_));
								} catch(...) {
									::dcool::core::goWeak<strategyC__>();
								}
								::dcool::core::batchDestruct(newChassis_.begin(engine_), newPosition_);
								throw;
							}
						}
						::dcool::core::batchDestruct(this->begin(engine_), position_);
					} catch (...) {
						newPosition_->~Value();
						newChassis_.uninitializeWithoutFilled_(engine_);
						throw;
					}
					newChassis_.m_storage_.setLength(engine_, this->length(engine_) + 1);
					this->uninitialize(engine_);
					newChassis_.relocateTo(engine_, engine_, *this);
					return newPosition_;
				}
				throw ::dcool::container::OutOfRange("Further growing this 'dcool::container::List' would cause out of range access.");
			}
			return this->braveEmplace<strategyC__>(engine_, position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->emplace<exceptionSafetyStrategy>(engine_, position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void braveEmplaceFront(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->braveEmplace<strategyC__>(engine_, this->begin(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void braveEmplaceFront(
			Engine& engine_, ArgumentTs__&&... parameters_
		) {
			this->braveEmplaceFront<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void braveEmplaceBack(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->braveEmplace<strategyC__>(engine_, this->end(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void braveEmplaceBack(
			Engine& engine_, ArgumentTs__&&... parameters_
		) {
			this->braveEmplaceBack<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void emplaceFront(Engine& engine_, ArgumentTs__&&... parameters_)  {
			this->emplace<strategyC__>(engine_, this->begin(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceFront(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplaceFront<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void emplaceBack(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplace<strategyC__>(engine_, this->end(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceBack(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplaceBack<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushFront(Engine& engine_, Value const& value_) {
			this->emplaceFront(engine_, value_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushFront(Engine& engine_, Value&& value_) {
			this->emplaceFront(engine_, ::dcool::core::move(value_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushBack(Engine& engine_, Value const& value_) {
			this->emplaceBack(engine_, value_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushBack(Engine& engine_, Value&& value_) {
			this->emplaceBack(engine_, ::dcool::core::move(value_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void inPlacePopFront_(Engine& engine_) noexcept {
			Length newBeginOffset_ = this->m_storage_.beginOffset(engine_) + 1;
			if (newBeginOffset_ >= this->capacity(engine_)) {
				newBeginOffset_ = 0;
			}
			this->front(engine_).~Value();
			Length newLength_ = this->length(engine_) - 1;
			this->m_storage_.setLength(engine_, newLength_);
			this->m_storage_.setCapacityForStuffed(engine_, newLength_);
			this->m_storage_.setBeginOffset(engine_, newBeginOffset_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void popFront(Engine& engine_) noexcept(!stuffed) {
			if (stuffed) {
				this->eraseAndShrink<strategyC__>(engine_, this->begin(engine_));
			} else {
				this->inPlacePopFront_<strategyC__>(engine_);
			}
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void inPlacePopBack_(Engine& engine_) noexcept {
			this->back(engine_).~Value();
			Length newLength_ = this->length(engine_) - 1;
			this->m_storage_.setLength(engine_, newLength_);
			if (this->squeezed(engine_)) {
				this->m_storage_.setCapacityForStuffed(engine_, newLength_);
			}
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void popBack(Engine& engine_) noexcept(!stuffed) {
			if constexpr (stuffed) {
				this->eraseAndShrink<strategyC__>(engine_, this->end(engine_) - 1);
			} else {
				this->inPlacePopBack_<strategyC__>(engine_);
			}
		}

		private: static constexpr auto copyRequiredToErase_(::dcool::core::ExceptionSafetyStrategy strategy_) noexcept {
			static_assert(!(squeezedOnly && stuffed), "Stuffed 'dcool::container::List' with fixed capacity cannot erase.");
			return ::dcool::core::atAnyCost(strategy_) && (!::dcool::core::isNoThrowMoveConstructible<Value>);
		}

		private: static constexpr auto reallocationMightRequiredToErase_(
			::dcool::core::ExceptionSafetyStrategy strategy_
		) noexcept {
			static_assert(!(squeezedOnly && stuffed), "Stuffed 'dcool::container::List' with fixed capacity cannot erase.");
			if constexpr (squeezedOnly) {
				return false;
			}
			if constexpr (stuffed) {
				return true;
			}
			return ::dcool::core::atAnyCost(strategy_) && (!::dcool::core::isNoThrowMoveAssignable<Value>);
		}

		private: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto inPlaceErase_(Engine& engine_, Iterator position_) -> Iterator {
			if constexpr (circular) {
				if (position_.index() * 2 < this->length(engine_)) {
					::dcool::core::batchMove(
						::dcool::core::makeReverseIterator(position_),
						this->reverseEnd(engine_),
						::dcool::core::makeReverseIterator(position_ + 1)
					);
				}
				this->inPlacePopFront_(engine_);
				return position_;
			}
			::dcool::core::batchMove<strategyC__>(position_ + 1, this->end(engine_), position_);
			this->inPlacePopBack_(engine_);
			return position_;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto eraseAndShrink(Engine& engine_, Iterator position_) -> Iterator {
			static_assert(!squeezedOnly);
			if (this->squeezed(engine_)) {
				return this->inPlaceErase_<strategyC__>(engine_, position_);
			}
			if (position_ == this->end(engine_) - 1) {
				this->popBack(engine_);
				return position_;
			}
			Self_ newChassis_;
			Length newLength_ = this->length(engine_) - 1;
			newChassis_.initializeWithoutFill_(engine_, newLength_);
			Iterator newPosition_;
			try {
				if (copyRequiredToErase_(strategyC__)) {
					newPosition_ = ::std::uninitialized_copy(this->begin(engine_), position_, newChassis_.begin(engine_));
				} else {
					newPosition_ = ::std::uninitialized_move(this->begin(engine_), position_, newChassis_.begin(engine_));
				}
				try {
					::dcool::core::batchRelocate<strategyC__>(position_ + 1, this->end(engine_), newPosition_);
				} catch (...) {
					if (!copyRequiredToErase_(strategyC__)) {
						try {
							::std::move(newChassis_.begin(engine_), newPosition_, this->begin(engine_));
						} catch (...) {
							::dcool::core::goWeak<strategyC__>();
							throw;
						}
					}
					::dcool::core::batchDestruct(newChassis_.begin(engine_), newPosition_);
					throw;
				}
				newChassis_.m_storage_.setLength(engine_, newLength_);
			} catch (...) {
				newChassis_.uninitializeWithoutFilled_(engine_);
				throw;
			}
			this->uninitializeWithoutFilled_(engine_);
			newChassis_.relocateTo(engine_, engine_, *this);
			return newPosition_;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto erase(Engine& engine_, Iterator position_) -> Iterator {
			if constexpr (reallocationMightRequiredToErase_(strategyC__)) {
				return this->eraseAndShrink<strategyC__>(engine_, position_);
			} else {
				static_assert(!stuffed, "Stuffed 'dcool::container::List' with fixed capacity cannot erase.");
			}
			return this->inPlaceErase_<strategyC__>(engine_, position_);
		}
	};

	template <typename ValueT_, typename ConfigT_ = ::dcool::core::Empty<>> struct List {
		static_assert(::dcool::container::ListConfig<ConfigT_, ValueT_>);
		private: using Self_ = List<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
		public: using Chassis = ::dcool::container::ListChassis<Value, Config>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Length = ConfigAdaptor_::Length;
		public: using Index = ConfigAdaptor_::Index;
		public: using Iterator = ConfigAdaptor_::Iterator;
		public: using ConstIterator = ConfigAdaptor_::ConstIterator;
		public: using ReverseIterator = ConfigAdaptor_::ReverseIterator;
		public: using ReverseConstIterator = ConfigAdaptor_::ReverseConstIterator;
		public: static constexpr ::dcool::core::Boolean squeezedOnly = ConfigAdaptor_::squeezedOnly;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr List() noexcept(noexcept(::dcool::core::declval<Self_&>().chassis().initialize(this->engine_()))) {
			this->chassis().initialize(this->engine_());
		}

		public: constexpr List(Self_ const& other_): m_engine_(other_.engine_()) {
			other_.chassis().cloneTo(other_.engine_(), this->engine_(), this->chassis());
		}

		public: constexpr List(Self_&& other_): m_engine_(other_.engine_()) {
			other_.chassis().relocateTo(other_.engine_(), this->engine_(), this->chassis());
			other_.chassis().initialize(other_.engine_());
		}

		public: constexpr List(Length capacity_) noexcept {
			this->chassis().initialize(this->engine_(), capacity_);
		}

		public: template <::dcool::core::InputIterator IteratorT_> constexpr List(
			::dcool::core::RangeInputTag, IteratorT_ otherBegin_, Length count_
		) {
			this->chassis().initialize(this->engine_(), otherBegin_, count_);
		}

		public: template <::dcool::core::InputIterator IteratorT_> constexpr List(
			::dcool::core::RangeInputTag tag_, IteratorT_ otherBegin_, IteratorT_ otherEnd_
		): Self_(tag_, otherBegin_, ::dcool::core::rangeLength(otherBegin_, otherEnd_)) {
		}

		public: constexpr ~List() noexcept {
			this->chassis().uninitialize(this->engine_());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) -> Self_& {
			Self_ middleMan_(::dcool::core::move(other_));
			this->swapWith(middleMan_);
			return *this;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void swapWith(Self_& other_) {
			this->chassis().template swapWith<strategyC__, true>(this->engine_(), other_.engine_(), other_.chassis());
			::dcool::core::intelliSwap(this->engine_(), other_.engine_());
		}

		public: constexpr auto vacant() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().vacant(this->engine_());
		}

		public: constexpr auto full() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().full(this->engine_());
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->chassis().length(this->engine_());
		}

		public: constexpr auto capacity() const noexcept -> Length {
			return this->chassis().capacity(this->engine_());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		public: constexpr auto engine_() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto begin() const noexcept -> ConstIterator {
			return this->chassis().begin(this->engine_());
		}

		public: constexpr auto begin() noexcept -> Iterator {
			return this->chassis().begin(this->engine_());
		}

		public: constexpr auto end() const noexcept -> ConstIterator {
			return this->chassis().end(this->engine_());
		}

		public: constexpr auto end() noexcept -> Iterator {
			return this->chassis().end(this->engine_());
		}

		public: constexpr auto position(Index index_) const noexcept -> ConstIterator {
			return this->chassis().position(this->engine_(), index_);
		}

		public: constexpr auto position(Index index_) noexcept -> Iterator {
			return this->chassis().position(this->engine_(), index_);
		}

		public: constexpr auto rawPointerAt(Index index_) const noexcept -> Value const* {
			return this->chassis().rawPointerAt(this->engine_(), index_);
		}

		public: constexpr auto rawPointerAt(Index index_) noexcept -> Value* {
			return this->chassis().rawPointerAt(this->engine_(), index_);
		}

		public: constexpr auto access(Index index_) const noexcept -> Value const& {
			return this->chassis().access(this->engine_(), index_);
		}

		public: constexpr auto access(Index index_) noexcept -> Value& {
			return this->chassis().access(this->engine_(), index_);
		}

		public: constexpr auto front() const noexcept -> Value const& {
			return this->chassis().front(this->engine_());
		}

		public: constexpr auto front() noexcept -> Value& {
			return this->chassis().front(this->engine_());
		}

		public: constexpr auto back() const noexcept -> Value const& {
			return this->chassis().back(this->engine_());
		}

		public: constexpr auto back() noexcept -> Value& {
			return this->chassis().back(this->engine_());
		}

		public: constexpr auto operator [](Index index_) const noexcept -> Value const& {
			return this->access(index_);
		}

		public: constexpr auto operator [](Index index_) noexcept -> Value& {
			return this->access(index_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void forceExpandBack(Length extra_) {
			this->chassis().template forceExpandBack<strategyC__>(this->engine_(), extra_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto braveEmplace(Iterator position_, ArgumentTs__&&... parameters_) -> Iterator {
			return this->chassis().template braveEmplace<strategyC__>(
				this->engine_(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto braveEmplace(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->braveEmplace<exceptionSafetyStrategy>(position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto emplace(Iterator position_, ArgumentTs__&&... parameters_) -> Iterator {
			return this->chassis().template emplace<strategyC__>(
				this->engine_(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->emplace<exceptionSafetyStrategy>(position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void braveEmplaceFront(ArgumentTs__&&... parameters_) {
			this->chassis().template braveEmplaceFront<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr void braveEmplaceFront(ArgumentTs__&&... parameters_) {
			this->braveEmplaceFront<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void braveEmplaceBack(ArgumentTs__&&... parameters_) {
			this->chassis().template braveEmplaceBack<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr void braveEmplaceBack(ArgumentTs__&&... parameters_) {
			this->braveEmplaceBack<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void emplaceFront(ArgumentTs__&&... parameters_) {
			this->chassis().template emplaceFront<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceFront(ArgumentTs__&&... parameters_) {
			this->emplaceFront<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr void emplaceBack(ArgumentTs__&&... parameters_) {
			this->chassis().template emplaceBack<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceBack(ArgumentTs__&&... parameters_) {
			this->emplaceBack<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushFront(Value const& value_) {
			this->chassis().template pushFront<strategyC__>(this->engine_(), value_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushFront(Value&& value_) {
			this->chassis().template pushFront<strategyC__>(this->engine_(), ::dcool::core::move(value_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushBack(Value const& value_) {
			this->chassis().template pushBack<strategyC__>(this->engine_(), value_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void pushBack(Value&& value_) {
			this->chassis().template pushBack<strategyC__>(this->engine_(), ::dcool::core::move(value_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void popFront() noexcept(noexcept(::dcool::core::declval<Self_&>().chassis().popFront(this->engine_()))) {
			this->chassis().template popFront<strategyC__>(this->engine_());
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void popBack() noexcept(noexcept(::dcool::core::declval<Self_&>().chassis().popBack(this->engine_()))) {
			this->chassis().template popBack<strategyC__>(this->engine_());
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto erase(Iterator position_) -> Iterator {
			return this->chassis().template erase<strategyC__>(this->engine_(), position_);
		}
	};

	namespace detail_ {
		template <::dcool::core::Length capacityC_> struct DefaultStaticListConfig_ {
			static constexpr ::dcool::core::Boolean squeezedOnly = true;
			static constexpr ::dcool::core::Length squeezedCapacity = capacityC_;
		};

		struct DefaultStuffedListConfig_ {
			static constexpr ::dcool::core::Boolean stuffed = true;
		};

		struct DefaultCircularListConfig_ {
			static constexpr ::dcool::core::Boolean circular = true;
		};
	}

	template <typename ValueT_, ::dcool::core::Length capacityC_> using StaticList = ::dcool::container::List<
		ValueT_, ::dcool::container::detail_::DefaultStaticListConfig_<capacityC_>
	>;

	template <typename ValueT_> using StuffedList = ::dcool::container::List<
		ValueT_, ::dcool::container::detail_::DefaultStuffedListConfig_
	>;

	template <typename ValueT_> using CircularList = ::dcool::container::List<
		ValueT_, ::dcool::container::detail_::DefaultCircularListConfig_
	>;
}

#endif
