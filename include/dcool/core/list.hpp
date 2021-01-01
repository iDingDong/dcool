#ifndef DCOOL_CORE_LIST_HPP_INCLUDED_
#	define DCOOL_CORE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>
#	include <dcool/core/dereference.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/integer.hpp>
#	include <dcool/core/memory.hpp>
#	include <dcool/core/min.hpp>
#	include <dcool/core/range.hpp>
#	include <dcool/core/type_value_detector.hpp>
#	include <dcool/core/utility.hpp>

#	include <algorithm>
#	include <limits>
#	include <stdexcept>

namespace dcool::core {
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
		> struct ArrayLightIterator_: private ::dcool::core::detail_::ArrayLightIteratorBase_<maxIndexC_> {
			private: using Self_ = ArrayLightIterator_<RangeT_, maxIndexC_, DistinguisherT_>;
			private: using Super_ = ::dcool::core::detail_::ArrayLightIteratorBase_<maxIndexC_>;
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

	template <
		typename ValueT_, ::dcool::core::Length capacityC_, ::dcool::core::StaticListConfig ConfigT_ = ::dcool::core::Empty<>
	> struct StaticList {
		private: using Self_ = StaticList<ValueT_, capacityC_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;
		private: static constexpr ::dcool::core::Length capacity_ = capacityC_;

		public: using Length = ::dcool::core::IntegerType<capacity_>;
		public: using Index = Length;
		public: using StorageType = ::dcool::core::StorageFor<Value[capacity_]>;
		public: using Iterator = Value*;
		public: using ReverseIterator = ::dcool::core::ReverseIterator<Iterator>;
		public: using ConstIterator = Value const*;
		public: using ReverseConstIterator = ::dcool::core::ReverseIterator<ConstIterator>;
		public: using LightIterator = ::dcool::core::detail_::ArrayLightIterator_<Self_, capacity_>;
		public: using LightConstIterator = ::dcool::core::detail_::ArrayLightIterator_<Self_ const, capacity_>;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			::dcool::core::exceptionSafetyStrategyOf<Config>
		;

		private: StorageType m_storage_;
		private: Length m_length_;

		public: constexpr StaticList() noexcept: m_length_(0) {
		}

		public: constexpr StaticList(Self_ const& other_): Self_() {
			try {
				for (auto const& item_: other_) {
					this->emplaceBack(item_);
				}
			} catch (...) {
				this->destruct_();
				throw;
			}
		}

		public: constexpr StaticList(Self_&& other_): Self_() {
			try {
				for (auto& item_: other_) {
					this->emplaceBack(::dcool::core::move(item_));
				}
			} catch (...) {
				try {
					::std::move(this->begin(), this->end(), other_->begin());
				} catch (...) {
					if constexpr (::dcool::core::strongOrTerminate(exceptionSafetyStrategy)) {
						::dcool::core::terminate();
					}
					throw;
				}
				this->destruct_();
				throw;
			}
		}

		public: constexpr ~StaticList() noexcept {
			this->destruct_();
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			this->copyFrom_(other_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) -> Self_& {
			this->moveFrom_(::dcool::core::move(other_));
			return *this;
		}

		private: constexpr void copyFrom_(Self_ const& other_) noexcept(::dcool::core::isNoThrowCopyable<Value>) {
			if constexpr (
				::dcool::core::atAnyCost(exceptionSafetyStrategy) &&
				(!::dcool::core::isNoThrowCopyable<Value>) &&
				::dcool::core::isNoThrowMovable<Value>
			) {
				Self_ middleMan_(other_);
				this->moveFrom_(::dcool::core::move(middleMan_));
			} else {
				Length lengthToCopy_ = ::dcool::core::min(this->length(), other_.length());
				Iterator endOfCopied_ = other_.begin() + lengthToCopy_;
				try {
					Iterator endOfWritten_ = ::std::copy(other_.begin(), endOfCopied_, this->begin());
					this->batchDestructToEnd_(endOfWritten_);
					::std::uninitialized_copy(endOfCopied_, other_.end(), this->end());
				} catch (...) {
					if constexpr (::dcool::core::strongOrTerminate(exceptionSafetyStrategy)) {
						::dcool::core::terminate();
					}
					throw;
				}
				this->m_length_ = other_.length();
			}
		}

		private: constexpr void moveFrom_(Self_&& other_) noexcept(
			(
				::dcool::core::atAnyCost(exceptionSafetyStrategy) &&
				(!::dcool::core::isNoThrowMovable<Value>) &&
				::dcool::core::isNoThrowCopyable<Value>
			) ||
			::dcool::core::isNoThrowMovable<Value>
		) {
			if constexpr (
				::dcool::core::atAnyCost(exceptionSafetyStrategy) &&
				(!::dcool::core::isNoThrowMovable<Value>) &&
				::dcool::core::isNoThrowCopyable<Value>
			) {
				this->copyFrom_(other_);
			} else {
				Length lengthToMove_ = ::dcool::core::min(this->length(), other_.length());
				Iterator endOfMoved_ = other_.begin() + lengthToMove_;
				try {
					Iterator endOfWritten_ = ::std::move(other_.begin(), endOfMoved_, this->begin());
					this->batchDestructToEnd_(endOfWritten_);
					::std::uninitialized_move(endOfMoved_, other_.end(), this->end());
				} catch (...) {
					if constexpr (::dcool::core::strongOrTerminate(exceptionSafetyStrategy)) {
						::dcool::core::terminate();
					}
					throw;
				}
				this->m_length_ = other_.length();
			}
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->m_length_;
		}

		public: consteval auto capacity() const noexcept -> Length {
			return static_cast<Length>(capacity_);
		}

		public: constexpr auto empty() const noexcept {
			return this->length() == 0;
		}

		public: constexpr auto full() const noexcept {
			return this->length() ==  this->capacity();
		}

		public: constexpr auto storage() const noexcept -> void const* {
			return &(this->m_storage_);
		}

		public: constexpr auto storage() noexcept -> void* {
			return &(this->m_storage_);
		}

		public: constexpr auto lightBegin() const noexcept -> LightConstIterator {
			return LightConstIterator(0);
		}

		public: constexpr auto lightBegin() noexcept -> LightIterator {
			return LightIterator(0);
		}

		public: constexpr auto lightEnd() const noexcept -> LightConstIterator {
			return this->lightBegin() + this->length();
		}

		public: constexpr auto lightEnd() noexcept -> LightIterator {
			return this->lightBegin() + this->length();
		}

		public: constexpr auto dereferenceLight(LightIterator position_) const noexcept -> Value const& {
			return position_.dereferenceSelf(*this);
		}

		public: constexpr auto dereferenceLight(LightIterator position_) noexcept -> Value& {
			return position_.dereferenceSelf(*this);
		}

		public: constexpr auto begin() const noexcept -> ConstIterator {
			return ConstIterator(static_cast<Value const*>(this->storage()));
		}

		public: constexpr auto begin() noexcept -> Iterator {
			return Iterator(static_cast<Value*>(this->storage()));
		}

		public: constexpr auto end() const noexcept -> ConstIterator {
			return this->begin() + this->length();
		}

		public: constexpr auto end() noexcept -> Iterator {
			return this->begin() + this->length();
		}

		public: constexpr auto reverseBegin() const noexcept -> ReverseConstIterator {
			return ::dcool::core::makeReverseIterator(this->end());
		}

		public: constexpr auto reverseBegin() noexcept -> ReverseIterator {
			return ::dcool::core::makeReverseIterator(this->end());
		}

		public: constexpr auto reverseEnd() const noexcept -> ReverseConstIterator {
			return ::dcool::core::makeReverseIterator(this->begin());
		}

		public: constexpr auto reverseEnd() noexcept -> ReverseIterator {
			return ::dcool::core::makeReverseIterator(this->begin());
		}


		public: constexpr auto fromLight(LightConstIterator light_) const noexcept -> ConstIterator {
			return this->begin() + (light_ - this->lightBegin());
		}

		public: constexpr auto toLight(LightConstIterator light_) const noexcept -> ConstIterator {
			return this->begin() + (light_ - this->lightBegin());
		}

		public: constexpr auto fromLight(LightIterator light_) noexcept -> Iterator {
			return this->begin() + (light_ - this->lightBegin());
		}

		public: constexpr auto fromLight(ConstIterator light_) noexcept -> LightConstIterator {
			return this->lightBegin() + (light_ - this->begin());
		}

		public: constexpr auto fromLight(Iterator light_) noexcept -> LightIterator {
			return this->lightBegin() + (light_ - this->begin());
		}

		public: constexpr auto operator [](Index index_) const noexcept -> Value const& {
			return ::dcool::core::dereference(this->begin() + index_);
		}

		public: constexpr auto operator [](Index index_) noexcept -> Value& {
			return ::dcool::core::dereference(this->begin() + index_);
		}

		private: constexpr void insertionIteratorCheck_(Iterator toCheck_) const noexcept {
			if (toCheck_ < this->begin() || toCheck_ > this->end()) [[unlikely]] {
				throw ::std::out_of_range("'dcool::core::StaticList' accessed out of range.");
			}
		}

		private: constexpr void iteratorCheck_(Iterator toCheck_) const noexcept {
			if (toCheck_ < this->begin() || toCheck_ >= this->end()) [[unlikely]] {
				throw ::std::out_of_range("'dcool::core::StaticList' accessed out of range.");
			}
		}

		private: constexpr void iteratorCheck_(Iterator begin_, Iterator end_) const noexcept {
			if (begin_ > end_) [[unlikely]] {
				throw ::std::out_of_range("'dcool::core::StaticList' encountered negative length range.");
			}
			if (begin_ < this->begin() || end_ > this->end()) [[unlikely]] {
				throw ::std::out_of_range("'dcool::core::StaticList' accessed out of range.");
			}
		}

		// This function will leave the gap uninitialized, making the list invalid.
		private: constexpr void makeRoom_(
			Iterator gapBegin_, Length gapLength_ = 1
		) noexcept(::dcool::core::isNoThrowRelocatable<Value>) {
			::dcool::core::batchRelocateForward(gapBegin_, this->end(), gapBegin_ + gapLength_);
			this->m_length_ += gapLength_;
		}

		// This function is intended for recover a valid state after 'makeRoom_'. Failure of recovery would be considered fatal.
		private: constexpr void reclaimRoom_(Iterator gapBegin_, Length gapLength_ = 1) noexcept {
			ReverseIterator reverseBegin_ = this-reverseBegin();
			::dcool::core::batchRelocateForward(
				reverseBegin_, ::dcool::core::makeReverseIterator(gapBegin_ + gapLength_), reverseBegin_ + gapLength_
			);
		}

		private: static constexpr void batchDestruct_(Iterator begin_, Iterator end_) noexcept {
			while (begin_ < end_) {
				::dcool::core::dereference(begin_).~Value();
				++begin_;
			}
		}

		private: constexpr void batchDestructToEnd_(Iterator begin_) noexcept {
			batchDestruct_(begin_, this->end());
		}

		private: constexpr void destruct_() noexcept {
			batchDestructToEnd_(this->begin());
		}

		public: constexpr auto insert(Iterator position_, Value&& value_) noexcept(
			noexcept(::dcool::core::isNoThrowMoveConstructible<Value>)
		) {
			this->makeRoom_(position_);
			try {
				new (::dcool::core::rawPointer(position_)) Value(::dcool::core::move(value_));
			} catch (...) {
				this->reclaimRoom_(position_);
				throw;
			}
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Iterator position_, ArgumentTs__&&... parameters_
		) noexcept(
			::dcool::core::isNoThrowRelocatable<Value> && noexcept(Value(::dcool::core::forward<ArgumentTs__>(parameters_)...))
		) -> Iterator {
			this->makeRoom_(position_);
			try {
				new (::dcool::core::rawPointer(position_)) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
			} catch (...) {
				this->reclaimRoom_(position_);
				throw;
			}
			return position_;
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceFront(ArgumentTs__&&... parameters_) noexcept(
			::dcool::core::isNoThrowRelocatable<Value> && noexcept(Value(::dcool::core::forward<ArgumentTs__>(parameters_)...))
		) -> Iterator {
			return this->emplace(this->begin(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceback(ArgumentTs__&&... parameters_) noexcept(
			noexcept(Value(::dcool::core::forward<ArgumentTs__>(parameters_)...))
		) -> Iterator {
			return this->emplace(this->end(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto cautiousEmplace(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			this->insertionIteratorCheck_(position_);
			if (this->full()) {
				throw ::std::out_of_range("'dcool::core::StaticList' found no room for insertion.");
			}
			return this->emplace(position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto cautiousEmplaceFront(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			if (this->full()) {
				throw ::std::out_of_range("'dcool::core::StaticList' found no room for insertion.");
			}
			return this->emplaceFront(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto cautiousEmplaceBack(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			if (this->full()) {
				throw ::std::out_of_range("'dcool::core::StaticList' found no room for insertion.");
			}
			return this->emplaceBack(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr auto erase(Iterator begin_, Iterator end_) -> Iterator {
			Iterator originalEnd_ = this->end();
			Iterator current_ = begin_;
			while (end_ < originalEnd_) {
				::dcool::core::dereference(current_) = ::dcool::core::move(::dcool::core::dereference(end_));
				++current_;
				++end_;
			}
			batchDestruct_(current_, originalEnd_);
			this->m_length_ = static_cast<Length>(begin_ - this->begin());
			return begin_;
		}

		public: constexpr auto eraseToEnd(Iterator begin_) noexcept -> Iterator {
			batchDestruct_(begin_, this->end());
			this->m_length_ = begin_ - this->begin();
			return begin_;
		}

		public: constexpr auto erase(Iterator position_) noexcept -> Iterator {
			return erase(position_, position_ + 1);
		}

		public: constexpr auto cautiousErase(Iterator begin_, Iterator end_) noexcept -> Iterator {
			this->iteratorCheck_(begin_, end_);
			if (begin_ == end_) [[unlikely]] {
				return begin_;
			}
			return this->erase(begin_, end_);
		}

		public: constexpr auto cautiousErase(Iterator position_) noexcept -> Iterator {
			this->iteratorCheck_(position_);
			return erase(position_);
		}
	};
}

#endif
