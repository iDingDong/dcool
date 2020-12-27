#ifndef DCOOL_CORE_FUNCTIONAL_HPP_INCLUDED_
#	define DCOOL_CORE_FUNCTIONAL_HPP_INCLUDED_ 1

namespace dcool::core {
	struct NoOp {
		template <typename... ArgumentTs_> void operator ()(ArgumentTs_&&...) noexcept {
		}
	};

	constexpr NoOp noOp;
}

#endif
