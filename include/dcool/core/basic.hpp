#ifndef DCOOL_CORE_BASIC_HPP_INCLUDED_
#	define DCOOL_CORE_BASIC_HPP_INCLUDED_ 1

#	include <cstdint>
#	include <cstddef>
#	include <new>
#	include <typeinfo>
#	include <utility>

namespace dcool::core {
	using ::std::declval;

	using Byte = ::std::byte;
	using Boolean = bool;
	using Size = decltype(sizeof(char));
	using Difference = decltype(::dcool::core::declval<char*>() - ::dcool::core::declval<char*>());
	using Alignment = decltype(alignof(char));
	using Length = ::dcool::core::Size;
	using Index = ::dcool::core::Length;
	using SignedMaxInteger = ::std::intmax_t;
	using	UnsignedMaxInteger = ::std::uintmax_t;
	using NullPointer = decltype(nullptr);
	using TypeInfo = ::std::type_info;

	constexpr ::dcool::core::NullPointer nullPointer = nullptr;

	template <typename T_> constexpr auto constantize(T_& input_) -> T_ const& {
		return input_;
	}

	template <typename T_> constexpr auto constantizePointer(T_* input_) -> T_ const* {
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

	struct Pit {
		template <typename... ArgumentTs_> constexpr Pit(ArgumentTs_&&... parameters_) noexcept {
		}
	};

	template <typename ValueT_, ::dcool::core::Boolean validC_> struct StaticOptional {
		[[no_unique_address]] ::dcool::core::Pit value;
	};

	template <typename ValueT_> struct StaticOptional<ValueT_, true> {
		ValueT_ value;
	};

	template <typename T_> constexpr auto addressOf(T_& toPoint_) noexcept {
		return ::std::addressof(toPoint_);
	}

	using ::std::launder;
}

#endif
