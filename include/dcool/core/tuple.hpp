#ifndef DCOOL_CORE_TUPLE_HPP_INCLUDED_
#	define DCOOL_CORE_TUPLE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/type_transformer.hpp>
#	include <dcool/core/utility.hpp>

#	include <tuple>
#	include <type_traits>

namespace dcool::core {
	template <typename... Ts_> using Tuple = ::std::tuple<Ts_...>;

	using ::std::get;
	using ::std::apply;

	template <typename... Ts_> constexpr auto makeTuple(Ts_&&... elements_) {
		return ::std::make_tuple(::dcool::core::forward<Ts_>(elements_)...);
	}

	template <typename T_> constexpr ::dcool::core::Boolean isTuple = false;
	template <typename... ValueTs_> constexpr ::dcool::core::Boolean isTuple<::std::tuple<ValueTs_...>> = true;

	namespace detail_ {
		template <typename TupleT_, ::dcool::core::Index indexC_> struct TupleElement_;

		template <
			typename... Ts_, ::dcool::core::Index indexC_
		> struct TupleElement_<::std::tuple<Ts_...>, indexC_> {
			using Result_ = ::dcool::core::VariadicElementType<indexC_, Ts_...>;
		};
	}

	template <
		typename TupleT_, ::dcool::core::Index indexC_
	> using TupleElementType = ::dcool::core::detail_::TupleElement_<TupleT_, indexC_>::Result_;

	namespace detail_ {
		template <typename SequenceT_> struct SubTupleHelper_;

		template <::dcool::core::Index... indexCs_> struct SubTupleHelper_<::dcool::core::IntegerSequence<indexCs_...>> {
			template <typename TupleT__> using Result_ = ::dcool::core::Tuple<::dcool::core::TupleElementType<TupleT__, indexCs_>...>;

			template <typename TupleT__> static constexpr auto sub_(TupleT__&& tuple_) {
				return Result_(
					::dcool::core::moveIf<::dcool::core::RvalueReference<TupleT__&&>>(::dcool::core::get<indexCs_>(tuple_))...
				);
			}
		};
	}

	template <
		typename IndexSequenceT_, typename TupleT_
	> requires (::dcool::core::isTuple<::dcool::core::QualifiedReferenceRemovedType<TupleT_>>) constexpr auto sub(
		TupleT_&& tuple_
	) {
		return ::dcool::core::detail_::SubTupleHelper_<IndexSequenceT_>::sub_(::dcool::core::forward<TupleT_>(tuple_));
	}
}

#endif
