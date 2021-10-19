#ifndef DCOOL_CORE_BASIC_HPP_INCLUDED_
#	define DCOOL_CORE_BASIC_HPP_INCLUDED_ 1

#	include <cstddef>
#	include <cstdint>
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
	using ForwardOffset = ::dcool::core::Size;
	using Index = ::dcool::core::Length;
	using PointerInteger = ::std::uintptr_t;
	using SignedMaxInteger = ::std::intmax_t;
	using	UnsignedMaxInteger = ::std::uintmax_t;
	using NullPointer = decltype(nullptr);
	using TypeInfo = ::std::type_info;

	template <typename ValueT_> using PointerFor = ValueT_*;

	constexpr ::dcool::core::NullPointer nullPointer = nullptr;

	template <typename ValueT_> constexpr ::dcool::core::PointerFor<ValueT_> nullPointerFor = ::dcool::core::nullPointer;

	template <typename T_> constexpr auto constantize(T_& input_) -> T_ const& {
		return input_;
	}

	template <typename T_> constexpr auto constantizePointer(T_* input_) -> T_ const* {
		return input_;
	}

	using PostDisambiguator = int;

	template <typename... Ts_> struct [[maybe_unused]] Empty {
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
		private: using Self_ = Pit;

		public: template <typename... ArgumentTs_> constexpr Pit(ArgumentTs_&&... parameters_) noexcept {
		}

		public: template <typename ArgumentT_> constexpr auto operator =(ArgumentT_&& argument_) noexcept -> Self_& {
			return *this;
		}
	};

	constexpr ::dcool::core::Pit pit;

	template <typename ValueT_, ::dcool::core::Boolean validC_> struct StaticOptional {
		static consteval auto valid() noexcept -> ::dcool::core::Boolean {
			return false;
		}

		[[no_unique_address]] ::dcool::core::Pit value;
	};

	template <typename ValueT_> struct StaticOptional<ValueT_, true> {
		static consteval auto valid() noexcept -> ::dcool::core::Boolean {
			return true;
		}

		ValueT_ value;
	};

	template <typename T_> constexpr auto addressOf(T_& toPoint_) noexcept {
		return ::std::addressof(toPoint_);
	}

	template <typename T_> constexpr auto referenceToSame(T_ const& left_, T_ const& right_) {
		return ::dcool::core::addressOf(left_) == ::dcool::core::addressOf(right_);
	}

	using ::std::launder;
	using ::std::forward;
	using ::std::move;

	template <::dcool::core::Boolean moveC_, typename ValueT_> constexpr decltype(auto) moveIf(ValueT_& value_) noexcept {
		if constexpr (moveC_) {
			return ::dcool::core::move(value_);
		} else {
			return value_;
		}
	}
}

#endif
