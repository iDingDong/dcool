#ifndef DCOOL_CONTAINER_INDEXABLE_ITERATOR_
#	define DCOOL_CONTAINER_INDEXABLE_ITERATOR_ 1

#	include <dcool/core.hpp>

namespace dcool::container {
	namespace detail_ {
		template <
			typename RangeChassisT_
		> constexpr ::dcool::core::Boolean isAccessibleByIndex_ = requires (
			RangeChassisT_& rangeChassis_, RangeChassisT_::Engine& engine_, RangeChassisT_::Index index_
		) {
			{
				::dcool::core::constantize(rangeChassis_).access(engine_, index_)
			} -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value const&>;
			{ rangeChassis_.access(engine_, index_) } -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value&>;
		};

		template <
			typename RangeChassisT_
		> constexpr ::dcool::core::Boolean isIndexableWithoutEngine_ = requires (
			RangeChassisT_& rangeChassis_, RangeChassisT_::Index index_
		) {
			{
				::dcool::core::constantize(rangeChassis_).rawPointerAt(index_)
			} -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value const*>;
			{ rangeChassis_.rawPointerAt(index_) } -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value*>;
		};

		template <
			typename RangeChassisT_
		> constexpr ::dcool::core::Boolean isAccessibleByIndexWithoutEngine_ = requires (
			RangeChassisT_& rangeChassis_, RangeChassisT_::Index index_
		) {
			{
				::dcool::core::constantize(rangeChassis_).access(index_)
			} -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value const&>;
			{ rangeChassis_.access(index_) } -> ::dcool::core::ConvertibleTo<typename RangeChassisT_::Value&>;
		};
	}

	template <typename RangeChassisT_> struct IndexableChassisLightIterator {
		private: using Self_ = IndexableChassisLightIterator<RangeChassisT_>;
		public: using RangeChassis = RangeChassisT_;

		public: using Value = ::dcool::core::IdenticallyConstType<::dcool::core::ValueType<RangeChassis>, RangeChassis>;
		public: using Length = RangeChassis::Length;
		public: using Difference = RangeChassis::Difference;
		public: using Index = RangeChassis::Length;
		public: using Engine = RangeChassis::Engine;
		private: static constexpr ::dcool::core::Boolean accessibleByIndex_ =
			::dcool::container::detail_::isAccessibleByIndex_<RangeChassis>
		;
		private: static constexpr ::dcool::core::Boolean indexableWithoutEngine_ =
			::dcool::container::detail_::isIndexableWithoutEngine_<RangeChassis>
		;
		private: static constexpr ::dcool::core::Boolean accessibleByIndexWithoutEngine_ =
			::dcool::container::detail_::isAccessibleByIndexWithoutEngine_<RangeChassis>
		;
		public: static constexpr ::dcool::core::Boolean engineRequired = !indexableWithoutEngine_;

		private: Index m_index_;

		public: constexpr explicit IndexableChassisLightIterator() noexcept = default;

		public: constexpr explicit IndexableChassisLightIterator(Index index_) noexcept: m_index_(index_) {
		}

		public: constexpr IndexableChassisLightIterator(
			::dcool::container::IndexableChassisLightIterator<::dcool::core::ConstRemovedType<RangeChassis>> const& other_
		) noexcept: m_index_(other_.index()) {
		}

		public: constexpr auto operator =(
			::dcool::container::IndexableChassisLightIterator<::dcool::core::ConstRemovedType<RangeChassis>> const& other_
		) noexcept -> Self_& {
			this->m_index_ = other_.index();
			return *this;
		}

		public: constexpr auto index() const noexcept {
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

		public: template <
			::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
		> constexpr auto rawPointerWith(RangeChassisT__&& rangeChassis_) const noexcept -> Value* {
			static_assert(!engineRequired);
			return rangeChassis_.rawPointerAt(this->index());
		}

		public: template <
			::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
		> constexpr auto rawPointerWith(Engine& engine_, RangeChassisT__&& rangeChassis_) const noexcept -> Value* {
			return rangeChassis_.rawPointerAt(engine_, this->index());
		}

		public: template <
			::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
		> constexpr auto dereferenceSelfWith(RangeChassisT__&& rangeChassis_) const noexcept -> Value& {
			if constexpr (accessibleByIndexWithoutEngine_) {
				return rangeChassis_.access(this->index());
			}
			return *::dcool::core::launder(this->rawPointerWith(rangeChassis_));
		}

		public: template <
			::dcool::core::FormOfSame<RangeChassis> RangeChassisT__
		> constexpr auto dereferenceSelfWith(Engine& engine_, RangeChassisT__&& rangeChassis_) const noexcept -> Value& {
			if constexpr (accessibleByIndex_) {
				return rangeChassis_.access(engine_, this->index());
			} else if constexpr (accessibleByIndexWithoutEngine_) {
				return this->dereferenceSelfWith(rangeChassis_);
			}
			return *::dcool::core::launder(this->rawPointerWith(engine_, rangeChassis_));
		}

		public: friend constexpr auto operator <=>(Self_ const&, Self_ const&) noexcept = default;

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
		typename RangeChassisT_
	> struct IndexableChassisIterator: private ::dcool::container::IndexableChassisLightIterator<RangeChassisT_> {
		private: using Self_ = IndexableChassisIterator<RangeChassisT_>;
		private: using Super_ = ::dcool::container::IndexableChassisLightIterator<RangeChassisT_>;
		public: using RangeChassis = RangeChassisT_;

		public: using typename Super_::Value;
		public: using typename Super_::Length;
		public: using typename Super_::Difference;
		public: using typename Super_::Index;
		public: using typename Super_::Engine;
		public: using Super_::engineRequired;
		public: using Super_::index;
		public: using Super_::advance;
		public: using Super_::retreat;

		public: using value_type = Value;
		public: using difference_type = Difference;
		public: using pointer = Value*;
		public: using reference = Value&;
		public: using iterator_category = ::dcool::core::RandomAccessIteratorTag;

		private: RangeChassis* m_range_ = ::dcool::core::nullPointer;
		private: [[no_unique_address]] ::dcool::core::StaticOptional<Engine*, engineRequired> m_engine_ = {
			.value = ::dcool::core::nullPointer
		};

		public: constexpr IndexableChassisIterator() noexcept = default;

		public: constexpr IndexableChassisIterator(
			::dcool::container::IndexableChassisIterator<::dcool::core::ConstRemovedType<RangeChassis>> const& other_
		) noexcept: Super_(other_.index()), m_range_(other_.m_range_), m_engine_(other_.m_engine_) {
		}

		public: constexpr IndexableChassisIterator(
			RangeChassis& range_, Index index_
		) noexcept: Super_(index_), m_range_(::dcool::core::addressOf(range_)) {
			static_assert(!engineRequired);
		}

		public: constexpr IndexableChassisIterator(
			RangeChassis& range_, Engine& engine_, Index index_
		) noexcept:
			Super_(index_), m_range_(::dcool::core::addressOf(range_)), m_engine_{ .value = ::dcool::core::addressOf(engine_) }
		{
		}

		public: constexpr auto operator =(
				::dcool::container::IndexableChassisIterator<::dcool::core::ConstRemovedType<RangeChassis>> const& other_
		) noexcept -> Self_& {
			this->m_range_ = other_.m_range_;
			this->Super_::operator =(
				static_cast<::dcool::container::IndexableChassisIterator<::dcool::core::ConstRemovedType<RangeChassis>>::Super_ const&>(
					other_
				)
			);
			return *this;
		}

		public: constexpr auto next(Difference step_ = 1) const noexcept -> Self_ {
			Self_ result_;
			if constexpr (engineRequired) {
				result_ = Self_(*(this->m_range_), *(this->m_engine_.value), static_cast<Difference>(this->index()) + step_);
			} else {
				result_ = Self_(*(this->m_range_), static_cast<Difference>(this->index()) + step_);
			}
			return result_;
		}

		public: constexpr auto previous(Difference step_ = 1) const noexcept -> Self_ {
			return this->next(-step_);
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Value& {
			Value* pointer_;
			if constexpr (engineRequired) {
				pointer_ = ::dcool::core::addressOf(this->dereferenceSelfWith(*(this->m_engine_.value), *(this->m_range_)));
			} else {
				pointer_ = ::dcool::core::addressOf(this->dereferenceSelfWith(*(this->m_range_)));
			}
			return *pointer_;
		}

		public: constexpr auto rawPointer() const noexcept -> Value* {
			Value* result_;
			if constexpr (engineRequired) {
				result_ = this->rawPointerWith(*(this->m_engine_.value), *(this->m_range_));
			} else {
				result_ = this->rawPointerWith(*(this->m_range_));
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
		) noexcept {
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

#endif
