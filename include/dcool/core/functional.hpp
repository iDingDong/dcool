#ifndef DCOOL_CORE_FUNCTIONAL_HPP_INCLUDED_
#	define DCOOL_CORE_FUNCTIONAL_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>

#	include <concepts>
#	include <functional>

namespace dcool::core {
	struct NoOp {
		template <typename... ArgumentTs_> void operator ()(ArgumentTs_&&...) noexcept {
		}
	};

	constexpr NoOp noOp;

	using ::std::invoke;

	template <typename T_, typename... ArgumentTs_> concept Invocable = ::std::invocable<T_, ArgumentTs_...>;

	template <typename T_, typename... ArgumentTs_> concept Predicator = requires(T_&& predicator_, ArgumentTs_&&... arguments_) {
		{
			::dcool::core::invoke(::dcool::core::forward<T_>(predicator_), ::dcool::core::forward<ArgumentTs_>(arguments_)...)
		} -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
	};

	template <typename T_, typename FunctorT_> concept UnaryArgumentFor = ::dcool::core::Invocable<FunctorT_, T_>;
}

#endif
