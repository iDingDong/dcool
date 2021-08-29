#ifndef DCOOL_CORE_COMPARE_HPP_INCLUDED_
#	define DCOOL_CORE_COMPARE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/basic.hpp>

#	include <concepts>
#	include <compare>
#	include <functional>

namespace dcool::core {
	template <typename T_, typename OtherT_ = T_> concept EqualityComparable = ::std::equality_comparable_with<T_, OtherT_>;
	template <typename T_> concept TotallyOrdered = ::std::totally_ordered<T_>;

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
}

#endif
