#ifndef DCOOL_CORE_LAYOUT_HPP_INCLUDED_
#	define DCOOL_CORE_LAYOUT_HPP_INCLUDED_

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/raw_pointer_operation.hpp>
#	include <dcool/core/type_list.hpp>
#	include <dcool/core/storage.hpp>

#	include <algorithm>

namespace dcool::core {
	namespace detail_ {
		template <typename T_, typename... Ts_> constexpr auto maxAlignmentOfAtLeastOne_() noexcept -> ::dcool::core::Alignment {
			if constexpr (sizeof...(Ts_) > 0) {
				return ::std::max(alignof(T_), ::dcool::core::detail_::maxAlignmentOfAtLeastOne_<Ts_...>());
			}
			return alignof(T_);
		}

		template <typename... Ts_> constexpr auto maxAlignmentOf_() noexcept -> ::dcool::core::Alignment {
			if constexpr (sizeof...(Ts_) > 0) {
				return ::dcool::core::detail_::maxAlignmentOfAtLeastOne_<Ts_...>();
			}
			return 1;
		}
	}

	template <
		typename... Ts_
	> constexpr ::dcool::core::Alignment maxAlignmentOf = ::dcool::core::detail_::maxAlignmentOf_<Ts_...>();

	namespace detail_ {

		template <::dcool::core::Index indexC_, typename T_, typename... Ts_> struct At_ {
			using Result_ = ::dcool::core::detail_::At_<indexC_ - 1, Ts_...>::Result_;
		};

		template <typename T_, typename... Ts_> struct At_<0, T_, Ts_...> {
			using Result_ = T_;
		};
	}

	template <
		::dcool::core::Index indexC_, typename... Ts_
	> requires(indexC_ < sizeof...(Ts_)) using AtType = ::dcool::core::detail_::At_<indexC_, Ts_...>::Result_;

	enum class LayoutStrategy {
		compact,
		orderedCompact
	};

	namespace detail_ {
		template <
			::dcool::core::Size baseC_, typename NextMemberT_
		> constexpr ::dcool::core::ForwardOffset nextCompactMemberOffset_ = ::dcool::core::matchAlignment(
			baseC_, alignof(NextMemberT_)
		);

		template <
			::dcool::core::Size baseC_, ::dcool::core::Index indexC_, typename FirstT_, typename... RestTs_
		> constexpr ::dcool::core::ForwardOffset compactMemberOffsetAtHelper_ =
			::dcool::core::detail_::compactMemberOffsetAtHelper_<
				::dcool::core::detail_::nextCompactMemberOffset_<baseC_, FirstT_> + sizeof(FirstT_), indexC_ - 1, RestTs_...
			>
		;

		template <
			::dcool::core::Size baseC_, typename FirstT_, typename... RestTs_
		> constexpr ::dcool::core::ForwardOffset compactMemberOffsetAtHelper_<
			baseC_, 0, FirstT_, RestTs_...
		> = ::dcool::core::detail_::nextCompactMemberOffset_<baseC_, FirstT_>;
	}

	template <
		::dcool::core::Index indexC_, ::dcool::core::Object... MemberTs_
	> requires (indexC_ < sizeof...(MemberTs_)) constexpr ::dcool::core::ForwardOffset compactMemberOffsetAt =
		::dcool::core::detail_::compactMemberOffsetAtHelper_<0, indexC_, MemberTs_...>
	;

	template <
		::dcool::core::Object... MemberTs_
	> constexpr ::dcool::core::Size compactOccupation = ::dcool::core::compactMemberOffsetAt<
		sizeof...(MemberTs_), MemberTs_..., char
	>;

	template <::dcool::core::LayoutStrategy strategyC_, ::dcool::core::Object... MemberTs_> struct BasicLayout;

	template <typename... MemberTs_> struct BasicLayout<::dcool::core::LayoutStrategy::orderedCompact, MemberTs_...> {
		public: template <::dcool::core::Index indexC__> using MemberType = ::dcool::core::AtType<indexC__, MemberTs_...>;
		public: static constexpr ::dcool::core::Alignment alignment = ::dcool::core::maxAlignmentOf<MemberTs_...>;
		public: static constexpr ::dcool::core::Size occupation = ::dcool::core::compactOccupation<MemberTs_...>;
		public: static constexpr ::dcool::core::Size size = ::dcool::core::matchAlignment(occupation, alignment);
		public: template <::dcool::core::Index indexC__> static constexpr ::dcool::core::ForwardOffset memberOffsetAt =
			::dcool::core::compactMemberOffsetAt<indexC__, MemberTs_...>
		;

		public: using Storage = ::dcool::core::AlignedStorage<::dcool::core::storageRequirement<size, alignment>>;

		public: template <
			::dcool::core::Index indexC__, typename T__
		> static constexpr auto memberAt(
			T__* object_
		) noexcept -> ::dcool::core::IdenticallyQualifiedType<MemberType<indexC__>, T__>* {
			return reinterpret_cast<::dcool::core::IdenticallyQualifiedType<MemberType<indexC__>, T__>*>(
				::dcool::core::stepByByte(object_, memberOffsetAt<indexC__>)
			);
		}
	};

	template <typename... MemberTs_> using OrderedCompactBasicLayout = ::dcool::core::BasicLayout<
		::dcool::core::LayoutStrategy::orderedCompact, MemberTs_...
	>;
}

#endif
