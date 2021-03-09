#ifndef DCOOL_CORE_COMPARE_HPP_INCLUDED_
#	define DCOOL_CORE_COMPARE_HPP_INCLUDED_ 1

#	include <concepts>
#	include <compare>

namespace dcool::core {
	template <typename T_, typename OtherT_ = T_> concept EqualityComparable = ::std::equality_comparable_with<T_, OtherT_>;
	template <typename T_> concept TotallyOrdered = ::std::totally_ordered<T_>;

	using PartialOrdering = ::std::partial_ordering;
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
}

#endif
