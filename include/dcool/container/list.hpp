#ifndef DCOOL_CONTAINER_LIST_HPP_INCLUDED_
#	define DCOOL_CONTAINER_LIST_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <algorithm>
#	include <limits>
#	include <stdexcept>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::container::detail_, HasValueStorageCapacity_, extractedStorageCapacityValue_, storageCapacity
)

namespace dcool::container {
	using OutOfRange = ::std::out_of_range;

	template <typename T_> concept StaticListConfig = true;

	namespace detail_ {
		template <
			::dcool::core::SignedMaxInteger maxIndexC_ = ::std::numeric_limits<::dcool::core::Difference>::max(),
			typename DistinguisherT_ = void
		> struct ArrayLightIteratorBase_ {
			private: using Self_ = ArrayLightIteratorBase_<maxIndexC_, DistinguisherT_>;
			private: static constexpr ::dcool::core::SignedMaxInteger maxIndex_ = maxIndexC_;

			public: using Length = ::dcool::core::IntegerType<maxIndex_>;
			public: using Index = Length;
			public: using Difference = ::dcool::core::IntegerType<maxIndex_, -maxIndex_>;
			public: static constexpr Index maxIndex = static_cast<Index>(maxIndex_);

			private: Index m_index_;

			public: constexpr ArrayLightIteratorBase_(Index index_) noexcept: m_index_(index_) {
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

			public: constexpr auto next(Difference step_ = 1) noexcept -> Self_ {
				return Self_(this->index() + step_);
			}

			public: constexpr auto previous(Difference step_ = 1) noexcept -> Self_ {
				return this->next(-step_);
			}

			public: constexpr auto distanceTo(Self_ const& other_) const noexcept -> Difference {
				return static_cast<Difference>(other_.index()) - static_cast<Difference>(this->index());
			}

			public: friend constexpr auto operator <=>(
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::StrongOrdering = default;

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
			::dcool::core::RandomAccessRange RangeT_,
			::dcool::core::SignedMaxInteger maxIndexC_ = ::std::numeric_limits<::dcool::core::Difference>::max(),
			typename DistinguisherT_ = void
		> struct ArrayLightIterator_: private ::dcool::container::detail_::ArrayLightIteratorBase_<maxIndexC_> {
			private: using Self_ = ArrayLightIterator_<RangeT_, maxIndexC_, DistinguisherT_>;
			private: using Super_ = ::dcool::container::detail_::ArrayLightIteratorBase_<maxIndexC_>;
			public: using Range = RangeT_;
			private: static constexpr ::dcool::core::SignedMaxInteger maxIndex_ = maxIndexC_;

			public: using typename Super_::Length;
			public: using typename Super_::Index;
			public: using typename Super_::Difference;
			public: using Super_::maxIndex;
			public: using Super_::Super_;
			public: using Super_::index;
			public: using Super_::advance;
			public: using Super_::retreat;
			public: using Value = ::dcool::core::ValueType<Range>;

			public: constexpr auto next(Difference step_ = 1) noexcept -> Self_ {
				return Self_(this->index() + step_);
			}

			public: constexpr auto previous(Difference step_ = 1) noexcept -> Self_ {
				return this->next(-step_);
			}

			public: constexpr auto dereferenceSelf(Range& range_) const noexcept {
				return ::dcool::core::dereference(range_.begin() + this->index());
			}

			public: constexpr auto distanceTo(Self_ const& other_) const noexcept -> Difference {
				return this->Super_::distanceTo(other_);
			}

			public: friend constexpr auto operator <=>(
				Self_ const&, Self_ const&
			) noexcept -> ::dcool::core::StrongOrdering = default;

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

	namespace detail_ {
		template <::dcool::core::Length storageCapacityC_, typename ConfigT_, typename ValueT_> struct ListConfigAdaptorBase_ {
			private: using Self_ = ListConfigAdaptorBase_<storageCapacityC_, ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using Length = ::dcool::core::IntegerType<storageCapacityC_>;

			public:	static constexpr ::dcool::core::Boolean capacityFixed = true;
			public: static constexpr Length storageCapacity = static_cast<Length>(storageCapacityC_);

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, ::dcool::core::Empty<Self_>>;
		};

		template <
			typename ConfigT_, typename ValueT_
		> struct ListConfigAdaptorBase_<::dcool::core::dynamicExtent, ConfigT_, ValueT_> {
			private: using Self_ = ListConfigAdaptorBase_<::dcool::core::dynamicExtent, ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public:	static constexpr ::dcool::core::Boolean capacityFixed = false;
			public: using Pool = ::dcool::resource::PoolType<Config>;

			private: struct DefaultEngine_ {
				public: [[no_unique_address]] Pool partPool;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::listPool'"
			);

			public: using PoolAdaptor = ::dcool::resource::PoolAdaptorFor<Pool, Value>;
			public: using Handle = PoolAdaptor::Handle;
			public: using Length = PoolAdaptor::Length;
		};

		template <
			typename ConfigT_, typename ValueT_
		> struct ListConfigAdaptor_: public ::dcool::container::detail_::ListConfigAdaptorBase_<
			::dcool::container::detail_::extractedStorageCapacityValue_<ConfigT_>(::dcool::core::dynamicExtent),
			ConfigT_,
			ValueT_
		> {
			private: using Self_ = ListConfigAdaptor_<ConfigT_, ValueT_>;
			private: using Super_ = ::dcool::container::detail_::ListConfigAdaptorBase_<
				::dcool::container::detail_::extractedStorageCapacityValue_<ConfigT_>(::dcool::core::dynamicExtent),
				ConfigT_,
				ValueT_
			>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using typename Super_::Length;

			public: using Index = Length;
			public: using Iterator = Value*;
			public: using ConstIterator = Value const*;
			public: using ReverseIterator = ::dcool::core::ReverseIterator<Iterator>;
			public: using ReverseConstIterator = ::dcool::core::ReverseIterator<ConstIterator>;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				::dcool::core::exceptionSafetyStrategyOf<Config>
			;
		};
	}

	template <
		typename T_, typename ValueT_
	> concept CapacityFixedListConfig = ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::capacityFixed;

	template <typename T_, typename ValueT_> concept ListConfig =
		::dcool::container::CapacityFixedListConfig<T_, ValueT_> ||
		::dcool::resource::ArrayPoolFor<typename ::dcool::container::detail_::ListConfigAdaptor_<T_, ValueT_>::Pool, ValueT_>
	;

	template <typename ConfigT_, typename ValueT_> requires ::dcool::container::ListConfig<
		ConfigT_, ValueT_
	> using ListConfigAdaptor = ::dcool::container::detail_::ListConfigAdaptor_<ConfigT_, ValueT_>;

	namespace detail_ {
		template <typename ValueT_, typename ConfigT_> struct ListChassisStorage_ {
			private: using Self_ = ListChassisStorage_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Pool = ConfigAdaptor_::Pool;
			public: using Handle = ConfigAdaptor_::Handle;
			public: using Length = ConfigAdaptor_::Length;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			private: Handle m_storage_;
			private: Length m_capacity_;

			public: constexpr ListChassisStorage_() noexcept = default;
			public: ListChassisStorage_(Self_ const&) = delete;
			public: ListChassisStorage_(Self_&&) = delete;
			public: constexpr ~ListChassisStorage_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: constexpr void initialize(Engine& engine_) noexcept {
				this->m_capacity_ = 0;
			}

			public: constexpr void initialize(Engine& engine_, Length capacity_) {
				this->m_capacity_ = capacity_;
				if (capacity_ > 0) {
					this->m_storage_ = ::dcool::resource::adaptedAllocateFor<Value>(engine_.pool(), capacity_);
					return;
				}
				if constexpr (::dcool::resource::PoolWithBijectiveHandleConverter<Pool, ::dcool::core::storageRequirementFor<Value>>) {
					this->m_storage_ = ::dcool::resource::adaptedToArrayHandleFor<Value>(engine_.pool(), ::dcool::core::nullPointer);
				}
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				if (this->m_capacity_ > 0) {
					::dcool::resource::adaptedDeallocateFor<Value>(engine_.pool(), this->m_storage_, this->capacity(engine_));
				}
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				other_.m_storage_ = this->m_storage_;
				other_.m_capacity_ = this->m_capacity_;
			}

			public: constexpr void swapWith(Self_& other_) noexcept {
				::dcool::core::intelliSwap(this->m_storage_, other_.m_storage_);
				::dcool::core::intelliSwap(this->m_capacity_, other_.m_capacity_);
			}

			public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
				return this->m_capacity_;
			}

			public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
				if constexpr (!::dcool::resource::PoolWithBijectiveHandleConverter<Pool, ::dcool::core::storageRequirementFor<Value>>) {
					if (this->capacity(engine_) == 0) {
						return ::dcool::core::nullPointer;
					}
				}
				return static_cast<Value const*>(::dcool::resource::adaptedFromArrayHandleFor<Value>(engine_.pool(), this->m_storage_));
			}

			public: constexpr auto data(Engine& engine_) noexcept -> Value* {
				return static_cast<Value*>(::dcool::resource::adaptedFromArrayHandleFor<Value>(engine_.pool(), this->m_storage_));
			}

			public: constexpr auto expandBack(Engine& engine_, Length extra_ = 1) -> ::dcool::core::Boolean {
				if (::dcool::resource::adaptedExpandBackFor<Value>(engine_.pool(), this->m_storage_, this->capacity(engine_), extra_)) {
					this->m_capacity_ += extra_;
					return true;
				}
				return false;
			}
		};

		template <
			typename ValueT_, ::dcool::container::CapacityFixedListConfig<ValueT_> ConfigT_
		> struct ListChassisStorage_<ValueT_, ConfigT_> {
			public: using Self_ = ListChassisStorage_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using Length = ConfigAdaptor_::Length;
			public: static constexpr Length storageCapacity = ConfigAdaptor_::storageCapacity;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			private: using StorageType = ::dcool::core::StorageFor<Value[storageCapacity]>;

			private: StorageType m_storage_;

			public: constexpr ListChassisStorage_() noexcept = default;
			public: ListChassisStorage_(Self_ const&) = delete;
			public: ListChassisStorage_(Self_&&) = delete;
			public: constexpr ~ListChassisStorage_() noexcept = default;
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

			public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
				return storageCapacity;
			}

			public: constexpr auto data() const noexcept -> Value const* {
				return reinterpret_cast<Value const*>(::dcool::core::addressOf(this->m_storage_));
			}

			public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
				return this->data();
			}

			public: constexpr auto data() noexcept -> Value* {
				return reinterpret_cast<Value*>(::dcool::core::addressOf(this->m_storage_));
			}

			public: constexpr auto data(Engine& engine_) noexcept -> Value* {
				return this->data();
			}
		};
	}

	template <typename ValueT_, ::dcool::container::ListConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct ListChassis {
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
		public: static constexpr ::dcool::core::Boolean capacityFixed = ConfigAdaptor_::capacityFixed;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;
		private: using StorageType_ = ::dcool::container::detail_::ListChassisStorage_<ValueT_, ConfigT_>;

		private: StorageType_ m_storage_;
		private: Length m_length_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		public: constexpr ListChassis() noexcept = default;
		public: ListChassis(Self_ const&) = delete;
		public: ListChassis(Self_&&) = delete;
		public: constexpr ~ListChassis() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: constexpr void initialize_() noexcept {
			static_assert(capacityFixed);
			this->m_storage_.initialize();
			this->m_length_ = 0;
		}

		public: constexpr void initialize(Engine& engine_) noexcept {
			this->m_storage_.initialize(engine_);
			this->m_length_ = 0;
		}

		public: constexpr void initialize(Engine& engine_, Length capacity_) {
			static_assert(!capacityFixed, "Attempted to change fixed capacity of a 'dcool::container::List'.");
			this->m_storage_.initialize(engine_, capacity_);
			this->m_length_ = 0;
		}

		private: constexpr void uninitialize_() noexcept {
			static_assert(capacityFixed);
			auto begin_ = this->data_();
			::dcool::core::batchDestruct(begin_, begin_ + this->m_length_);
			this->m_storage_.uninitialize();
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			::dcool::core::batchDestruct(this->begin(engine_), this->end(engine_));
			this->m_storage_.uninitialize(engine_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void relocateTo(Self_& other_) {
			if constexpr (capacityFixed) {
				other_.initialize_();
				try {
					::dcool::core::batchRelocate<strategyC__>(
						this->m_storage_.data(), this->m_storage_.data() + this->m_length_, other_.m_storage_.data()
					);
				} catch (...) {
					other_.uninitialize_();
					throw;
				}
				other_.m_length_ = this->m_length_;
				this->uninitialize_();
			} else {
				this->m_storage_.relocateTo(other_.m_storage_);
				other_.m_length_ = this->m_length_;
			}
		}

		public: template <typename ValueT__, typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::container::ListChassis<ValueT__, ConfigT__>::Engine& otherEngine_,
			::dcool::container::ListChassis<ValueT__, ConfigT__>& other_
		) const {
			if constexpr (other_.capacityFixed) {
				other_.initialize(otherEngine_);
			} else {
				other_.initialize(otherEngine_, this->length(engine_));
			}
			try {
				::std::uninitialized_copy(this->begin(engine_), this->end(engine_), other_.begin(otherEngine_));
				other_.m_length_ = this->length(engine_);
			} catch (...) {
				other_.uninitialize(otherEngine_);
				throw;
			}
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void swapWith(Self_& other_) noexcept {
			if constexpr (!capacityFixed) {
				this->m_storage_.swapWith(other_.m_storage_);
			} else {
				if constexpr (::dcool::core::atAnyCost(strategyC__)) {
				} else {
					Length commonLength_ = ::dcool::core::min(this->m_length_, other_.m_length_);
					auto selfBegin_ = this->m_storage_.data();
					auto otherBegin_ = other_.m_storage_.data();
					Index index_ = 0;
					try {
						while (index_ < commonLength_) {
							::dcool::core::intelliSwap(selfBegin_[index_], otherBegin_[index_]);
						}
						// At least one of the following call of 'batchRelocate' will be no-op.
						::dcool::core::batchRelocate<strategyC__>(
							selfBegin_ + commonLength_, selfBegin_ + this->m_length_, otherBegin_ + commonLength_
						);
						::dcool::core::batchRelocate<strategyC__>(
							otherBegin_ + commonLength_, otherBegin_ + other_.m_length_, selfBegin_ + commonLength_
						);
						::dcool::core::intelliSwap(this->m_length_, other_.m_length_);
					} catch (...) {
						try {
							while (index_ > 0) {
								--index_;
								::dcool::core::intelliSwap(selfBegin_[index_], otherBegin_[index_]);
							}
						} catch (...) {
							::dcool::core::goWeak<strategyC__>();
						}
						throw;
					}
				}
			}
		}

		public: constexpr auto vacant(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->length(engine_) == 0;
		}

		public: constexpr auto full(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->length(engine_) == this->capacity(engine_);
		}

		public: constexpr auto length(Engine& engine_) const noexcept -> Length {
			return this->m_length_;
		}

		public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
			return this->m_storage_.capacity(engine_);
		}

		public: constexpr auto leftOver(Engine& engine_) const noexcept -> Length {
			return this->capacity(engine_) - this->length(engine_);
		}

		private: constexpr auto data_() const noexcept -> Value const* {
			static_assert(capacityFixed);
			return this->m_storage_.data();
		}

		private: constexpr auto data_() noexcept -> Value* {
			return this->m_storage_.data();
		}

		public: constexpr auto data(Engine& engine_) const noexcept -> Value const* {
			return this->m_storage_.data(engine_);
		}

		public: constexpr auto data(Engine& engine_) noexcept -> Value* {
			return this->m_storage_.data(engine_);
		}

		public: constexpr auto begin(Engine& engine_) const noexcept -> ConstIterator {
			return this->data(engine_);
		}

		public: constexpr auto begin(Engine& engine_) noexcept -> Iterator {
			return this->data(engine_);
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

		public: constexpr auto access(Engine& engine_, Index index_) const noexcept -> Value const& {
			return *(this->position(engine_, index_));
		}

		public: constexpr auto access(Engine& engine_, Index index_) noexcept -> Value& {
			return *(this->position(engine_, index_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void forceExpandBack(Engine& engine_, Length extra_) {
			static_assert(!capacityFixed, "Attempted to change fixed capacity of a 'dcool::container::List'.");
			if (extra_ == 0) {
				return;
			}
			if (!(this->m_storage_.expandBack(engine_, extra_))) {
				StorageType_ newStorage_;
				Length newCapacity_ = this->capacity() + extra_;
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
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void reserve(Engine& engine_) {
			this->forceExpandBack<strategyC__>(engine_, this->capacity() + 1);
		}

		// This function will leave the gap uninitialized, making the list invalid.
		private: public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void makeRoom_(
			Engine& engine_, Iterator gapBegin_, Length gapLength_ = 1
		) noexcept(::dcool::core::isNoThrowRelocatable<Value>) {
			::dcool::core::batchRelocateForward<strategyC__>(gapBegin_, this->end(engine_), gapBegin_ + gapLength_);
			this->m_length_ += gapLength_;
		}

		// This function is intended for recover a valid state after 'makeRoom_'. Failure of recovery would be considered fatal.
		private: public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void reclaimRoom_(Engine& engine_, Iterator gapBegin_, Length gapLength_ = 1) noexcept {
			ReverseIterator reverseBegin_ = this->reverseBegin(engine_);
			::dcool::core::batchRelocateForward<strategyC__>(
				reverseBegin_, ::dcool::core::makeReverseIterator(gapBegin_ + gapLength_), reverseBegin_ + gapLength_
			);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto braveEmplace(Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_) -> Iterator {
			this->makeRoom_<strategyC__>(engine_, position_);
			try {
				new (::dcool::core::rawPointer(position_)) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
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
				if constexpr (!capacityFixed) {
					Length extraCapacity_ = ((this->capacity(engine_) > 0) ? this->capacity(engine_) : 1);
					if (this->m_storage_.expandBack(engine_, extraCapacity_)) {
						return this->braveEmplace<strategyC__>(engine_, position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
					}
					StorageType_ newStorage_;
					Length newCapacity_ = this->capacity(engine_) + extraCapacity_;
					newStorage_.initialize(engine_, newCapacity_);
					Length lengthBeforePosition_ = static_cast<Length>(position_ - this->begin(engine_));
					Value* newPosition_ = newStorage_.data(engine_) + lengthBeforePosition_;
					try {
						new (newPosition_) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
					} catch (...) {
						newStorage_.uninitialize(engine_);
						throw;
					}
					try {
						if constexpr (
							::dcool::core::atAnyCost(strategyC__) &&
							!(noexcept(::std::uninitialized_move(this->begin(engine_), position_, newStorage_.data(engine_))))
						) {
							::std::uninitialized_copy(this->begin(engine_), position_, newStorage_.data(engine_));
							try {
								::dcool::core::batchRelocate<strategyC__>(position_, this->end(engine_), newPosition_ + 1);
							} catch (...) {
								::dcool::core::batchDestruct(newStorage_.data(engine_), newPosition_);
								throw;
							}
						} else {
							::std::uninitialized_move(this->begin(engine_), position_, newStorage_.data(engine_));
							try {
								::dcool::core::batchRelocate<strategyC__>(position_, this->end(engine_), newPosition_ + 1);
							} catch (...) {
								try {
									::std::move(newStorage_.data(engine_), newPosition_, this->begin(engine_));
								} catch(...) {
									::dcool::core::goWeak<strategyC__>();
								}
								::dcool::core::batchDestruct(newStorage_.data(engine_), newPosition_);
								throw;
							}
						}
						::dcool::core::batchDestruct(this->begin(engine_), position_);
					} catch (...) {
						newPosition_->~Value();
						newStorage_.uninitialize(engine_);
						throw;
					}
					this->m_storage_.uninitialize(engine_);
					newStorage_.relocateTo(this->m_storage_);
					++(this->m_length_);
					return Iterator(newPosition_);
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
		> constexpr auto braveEmplaceBack(Engine& engine_, ArgumentTs__&&... parameters_) -> Iterator {
			return this->braveEmplace<strategyC__>(engine_, this->end(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto braveEmplaceBack(
			Engine& engine_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->braveEmplaceBack<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto emplaceBack(Engine& engine_, ArgumentTs__&&... parameters_) -> Iterator {
			return this->emplace<strategyC__>(engine_, this->end(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceBack(
			Engine& engine_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->emplaceBack<exceptionSafetyStrategy>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: void popBack(Engine& engine_) noexcept {
			(this->end(engine_) - 1)->~Value();
			--(this->m_length_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto erase(Engine& engine_, Iterator position_) -> Iterator {
			if constexpr (
				(!capacityFixed) &&
				::dcool::core::atAnyCost(strategyC__) &&
				!noexcept(::std::move(position_ + 1, this->end(engine_), position_))
			) {
				if (position_ == this->end() - 1) {
					this->popBack(engine_);
					return position_;
				}
				Self_ newChassis_;
				newChassis_.initialize(engine_, this->length() - 1);
				Length lengthBeforePosition_ = static_cast<Length>(position_ - this->begin());
				Iterator newPosition_;
				try {
					newPosition_ = ::std::uninitialized_copy(this->begin(engine_), position_, newChassis_.begin(engine_));
					newChassis_.m_length_ = lengthBeforePosition_;
					Length newLength_ = this->length() - 1;
					::dcool::core::batchRelocate<strategyC__>(position_ + 1, this->end(), newPosition_);
					this->m_length_ = lengthBeforePosition_;
					newChassis_.m_length_ = newLength_;
				} catch (...) {
					newChassis_.uninitialize(engine_); // We will need another manual destruction before this if length was not set.
					throw;
				}
				this->uninitialize(engine_);
				newChassis_.relocateTo(*this);
				return newPosition_;
			} else {
				try {
					::std::move(position_ + 1, this->end(engine_), position_);
				} catch(...) {
					::dcool::core::goWeak<strategyC__>();
					throw;
				}
				this->popBack(engine_);
			}
			return position_;
		}
	};

	template <typename ValueT_, ::dcool::container::ListConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct List {
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
		public: static constexpr ::dcool::core::Boolean capacityFixed = ConfigAdaptor_::capacityFixed;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr List() noexcept {
			this->chassis().initialize(this->engine_());
		}

		public: constexpr List(Self_ const& other_): m_engine_(other_.engine_()) {
			other_.chassis().cloneTo(other_.engine_(), this->engine_(), this->chassis());
		}

		public: constexpr List(Self_&& other_): m_engine_(other_.engine_()) {
			other_.chassis().relocateTo(this->chassis());
			other_.chassis().initialize(other_.engine_());
		}

		public: constexpr List(Length capacity_) noexcept {
			this->chassis().initialize(this->engine_(), capacity_);
		}

		public: template <::dcool::core::InputIterator IteratorT_> constexpr List(
			::dcool::core::RangeInputTag, IteratorT_ otherBegin_, IteratorT_ otherEnd_
		): Self_(static_cast<Length>(::dcool::core::rangeLength(otherBegin_, otherEnd_))) {
			// May optimize this to prevent some unecessary checks
			try {
				while (otherBegin_ != otherEnd_) {
					this->emplaceBack(::dcool::core::dereference(otherBegin_));
					++otherBegin_;
				}
			} catch (...) {
				this->chassis().uninitialize(this->engine_());
				throw;
			}
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

		public: constexpr void swapWith(Self_& other_) {
			this->chassis().swapWith(other_.chassis());
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

		public: constexpr auto access(Index index_) const noexcept -> Value const& {
			return this->chassis().access(this->engine_(), index_);
		}

		public: constexpr auto access(Index index_) noexcept -> Value& {
			return this->chassis().access(this->engine_(), index_);
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
		> constexpr auto braveEmplaceBack(ArgumentTs__&&... parameters_) -> Iterator {
			return this->chassis().template braveEmplaceBack<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto braveEmplaceBack(ArgumentTs__&&... parameters_) -> Iterator {
			return this->braveEmplaceBack<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__, typename... ArgumentTs__
		> constexpr auto emplaceBack(ArgumentTs__&&... parameters_) -> Iterator {
			return this->chassis().template emplaceBack<strategyC__>(
				this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceBack(ArgumentTs__&&... parameters_) -> Iterator {
			return this->emplaceBack<exceptionSafetyStrategy>(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr void popBack() noexcept {
			return this->chassis().popBack(this->engine_());
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr auto erase(Iterator position_) -> Iterator {
			return this->chassis().template erase<strategyC__>(this->engine_(), position_);
		}
	};

	namespace detail_ {
		template <::dcool::core::Length storageCapacityC_> struct DefaultStaticListConfig_ {
			static constexpr ::dcool::core::Length storageCapacity = storageCapacityC_;
		};
	}

	template <typename ValueT_, ::dcool::core::Length storageCapacityC_> using StaticList = ::dcool::container::List<
		ValueT_, ::dcool::container::detail_::DefaultStaticListConfig_<storageCapacityC_>
	>;
}

#endif
