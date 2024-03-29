#ifndef DCOOL_CORE_COMPARE_HPP_INCLUDED_
#	define DCOOL_CORE_COMPARE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <atomic> // Value representation compare hack
#	include <concepts>
#	include <compare>
#	include <functional>

namespace dcool::core {
	template <typename T_, typename OtherT_ = T_> concept EqualityComparable = ::std::equality_comparable_with<T_, OtherT_>;
	template <typename T_, typename OtherT_ = T_> concept ThreeWayComparable = ::std::three_way_comparable_with<T_, OtherT_>;
	template <typename T_, typename OtherT_ = T_> concept TotallyOrdered = ::std::totally_ordered_with<T_, OtherT_>;

	using PartialOrdering = ::std::partial_ordering;
	using WeakOrdering = ::std::weak_ordering;
	using StrongOrdering = ::std::strong_ordering;

	template <typename OrderingT_> constexpr auto equalOrEquivalent(OrderingT_ result_) noexcept -> ::dcool::core::Boolean {
		if (result_ == OrderingT_::equivalent) {
			return true;
		}
		if constexpr (requires {
			OrderingT_::equal;
		}) {
			return result_ == OrderingT_::equal;
		}
		return false;
	}

	template <typename ...Ts_> struct ComparableEmpty {
		private: using Self_ = ComparableEmpty<Ts_...>;

		public: friend auto operator <=>(Self_ const&, Self_ const&) noexcept = default;
		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator <(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator >(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator <=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator >=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <
		typename OperandT_,
		typename WrappedT_ = ::std::compare_three_way
	> struct StandardComparerWrapperFor {
		private: using Self_ = StandardComparerWrapperFor<OperandT_, WrappedT_>;
		public: using Operand = OperandT_;
		public: using Wrapped = WrappedT_;

		[[no_unique_address]] Wrapped underlying;

		constexpr auto operator ()(
			Operand const& left_, Operand const& right_
		) const noexcept(noexcept(underlying(left_, right_))) {
			return underlying(left_, right_);
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean {
			return true;
		}

		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <typename OperandT_> using DefaultComparerFor = ::dcool::core::StandardComparerWrapperFor<
		OperandT_, ::std::compare_three_way
	>;

	template <typename OperandT_, typename WrappedT_> struct ComparerEqualityWrapperFor {
		private: using Self_ = ComparerEqualityWrapperFor<OperandT_, WrappedT_>;
		public: using Operand = OperandT_;
		public: using Wrapped = WrappedT_;

		[[no_unique_address]] Wrapped underlying;

		constexpr auto operator ()(
			Operand const& left_, Operand const& right_
		) const noexcept(noexcept(underlying(left_, right_))) -> ::dcool::core::Boolean {
			return underlying(left_, right_) == 0;
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean {
			return true;
		}

		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <
		typename OperandT_,
		typename WrappedT_ = ::std::equal_to<OperandT_>
	> struct StandardEqualityComparerWrapperFor {
		private: using Self_ = StandardEqualityComparerWrapperFor<OperandT_, WrappedT_>;
		public: using Operand = OperandT_;
		public: using Wrapped = WrappedT_;

		[[no_unique_address]] Wrapped underlying;

		constexpr auto operator ()(
			Operand const& left_, Operand const& right_
		) const noexcept(noexcept(underlying(left_, right_))) -> ::dcool::core::Boolean {
			return underlying(left_, right_);
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean {
			return true;
		}

		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <typename OperandT_> using DefaultEqualityComparerFor = ::dcool::core::StandardEqualityComparerWrapperFor<
		OperandT_, ::std::equal_to<OperandT_>
	>;

	template <::dcool::core::TriviallyCopyable OperandT_> constexpr auto hasEqualValueRepresentation(
		OperandT_ const& left_, OperandT_ const& right_
	) noexcept -> ::dcool::core::Boolean {
		if constexpr (::dcool::core::Scalar<OperandT_>) {
			return left_ == right_;
		}
		// C++20 atomic supports value representation comparison with CAS. This is evil but portable.
		::std::atomic<OperandT_> leftToCompare_(left_);
		OperandT_ rightValue_ = right_;
		return leftToCompare_.compare_exchange_strong(rightValue_, left_, ::std::memory_order::relaxed);
	}

	template <::dcool::core::TriviallyCopyable OperandT_> constexpr auto intelliHasEqualValueRepresentation(
		OperandT_ const& left_, OperandT_ const& right_
	) noexcept -> ::dcool::core::Boolean {
		return hasEqualValueRepresentation(left_, right_);
	}

	template <typename ValueT_> constexpr auto oneOf(ValueT_ const& value_) noexcept -> ::dcool::core::Boolean {
		return false;
	}

	template <typename ValueT_, typename... ValueTs_> constexpr auto oneOf(
		ValueT_ const& value_,
		::dcool::core::UndeducedType<ValueT_> const& first_,
		ValueTs_ const&... rests_
	) noexcept(noexcept(value_ == first_)) -> ::dcool::core::Boolean {
		if (value_ == first_) {
			return true;
		}
		return ::dcool::core::oneOf(value_, rests_...);
	}
}

#endif
