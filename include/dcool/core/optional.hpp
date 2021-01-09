#ifndef DCOOL_CORE_OPTIONAL_HPP_INCLUDED_
#	define DCOOL_CORE_OPTIONAL_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>

#	include <optional>

namespace dcool::core {
	template <typename ValueT_> struct Optional;

	namespace detail_ {
		template <typename T_, typename OptionalValueT_> concept OptionalDirectionInitializeArgumentFor_ =
			!::dcool::core::FormOfOneOf<T_, ::dcool::core::Optional<OptionalValueT_>, InPlaceTag>
		;
	}

	// TODO: Specialize this to support optimization for types with (user-specified) trap representation.
	template <typename ValueT_> struct Optional {
		public: using Self_ = Optional<ValueT_>;
		public: using Value = ValueT_;

		private: using Underlying_ = ::std::optional<Value>;

		private: Underlying_ m_underlying_;

		public: constexpr Optional() noexcept = default;
		public: constexpr Optional(Self_ const& other_) = default;
		public: constexpr Optional(Self_&& other_) = default;

		public: template <
			::dcool::core::detail_::OptionalDirectionInitializeArgumentFor_<Value> ValueT__
		> constexpr Optional(ValueT__&& value_): m_underlying_(::dcool::core::forward<ValueT__>(value_)) {
		}

		public: constexpr ~Optional() noexcept = default;
		public: constexpr auto operator =(Self_ const& other_) -> Self_& = default;
		public: constexpr auto operator =(Self_&& other_) -> Self_& = default;

		public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.has_value();
		}

		public: constexpr auto access() const noexcept -> Value const& {
			return *(this->m_underlying_);
		}

		public: constexpr auto access() noexcept -> Value& {
			return *(this->m_underlying_);
		}

		public: constexpr auto value() const -> Value const& {
			return this->m_underlying_.value();
		}

		public: constexpr auto value() -> Value& {
			return this->m_underlying_.value();
		}

		public: template <typename... ArgumentTs__> constexpr void emplace(ArgumentTs__&&... arguments_) noexcept(
			noexcept(::dcool::core::declval<Underlying_&>().emplace(::dcool::core::forward<ArgumentTs__>(arguments_)...))
		) {
			return this->m_underlying_.emplace(::dcool::core::forward<ArgumentTs__>(arguments_)...);
		}

		public: constexpr void reset() noexcept {
			this->m_underlying_.reset();
		}
	};
}

#endif
