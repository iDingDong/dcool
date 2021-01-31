#ifndef DCOOL_CORE_BASIC_HPP_INCLUDED_
#	define DCOOL_CORE_BASIC_HPP_INCLUDED_ 1

#	include <cstdint>
#	include <cstddef>
#	include <exception>
#	include <new>
#	include <utility>

namespace dcool::core {
	using ::std::declval;
	using ::std::terminate;

	using Byte = ::std::byte;
	using Boolean = bool;
	using Size = decltype(sizeof(char));
	using Difference = decltype(::dcool::core::declval<char*>() - ::dcool::core::declval<char*>());
	using Alignment = decltype(alignof(char));
	using Length = ::dcool::core::Size;
	using SignedMaxInteger = ::std::intmax_t;
	using	UnsignedMaxInteger = ::std::uintmax_t;
	using NullPointer = decltype(nullptr);

	constexpr ::dcool::core::NullPointer nullPointer = nullptr;

	template <typename T_> constexpr auto constantize(T_& input_) -> T_ const& {
		return input_;
	}

	using PostDisambiguator = int;

	template <typename... Ts_> struct Empty {
	};

	namespace detail_ {
		template <typename... Ts_> struct StandardLayoutBreakerBase_ {
			private: [[no_unique_address]] ::dcool::core::Empty<Ts_...> m_helper_;
		};
	}

	template <typename... Ts_> struct StandardLayoutBreaker: public ::dcool::core::detail_::StandardLayoutBreakerBase_<Ts_...> {
		private: [[no_unique_address]] ::dcool::core::Empty<Ts_...> m_helper_;
	};

	template <typename T_, typename DistinguisherT_ = void> inline T_ instance;

	template <typename T_> constexpr auto addressOf(T_& toPoint_) noexcept {
		return ::std::addressof(toPoint_);
	}

	using ::std::launder;
}

#endif
