#ifndef DCOOL_CORE_TUPLE_HPP_INCLUDED_
#	define DCOOL_CORE_TUPLE_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/type_transformer.hpp>
#	include <dcool/core/utility.hpp>

#	include <tuple>

namespace dcool::core {
	template <typename... Ts_> using Tuple = ::std::tuple<Ts_...>;

	using ::std::get;
	using ::std::apply;

	template <typename... Ts_> constexpr auto makeTuple(Ts_&&... elements_) {
		return ::std::make_tuple(::dcool::core::forward<Ts_>(elements_)...);
	}

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
}

#endif
