#ifndef DCOOL_CORE_BASIC_HPP_INCLUDED_
#	define DCOOL_CORE_BASIC_HPP_INCLUDED_ 1

#	include <utility>
#	include <exception>
#	include <cstdint>

namespace dcool::core {
	using ::std::declval;
	using ::std::terminate;

	using Boolean = bool;
	using Size = decltype(sizeof(char));
	using Difference = decltype(::dcool::core::declval<char*>() - ::dcool::core::declval<char*>());
	using Alignment = decltype(alignof(char));
	using Length = ::dcool::core::Size;
	using SignedMaxInteger = ::std::intmax_t;
	using	UnsignedMaxInteger = ::std::uintmax_t;

	template <typename T_> constexpr auto constantize(T_& input_) -> T_ const& {
		return input_;
	}

	using PostDisambiguator = int;

	template <typename... Ts_> struct Empty {
	};

	template <typename T_, typename DistinguisherT_ = void> inline T_ instance;

	template <typename T_> constexpr auto addressOf(T_& toPoint_) noexcept {
		return ::std::addressof(toPoint_);
	}
};

#endif
