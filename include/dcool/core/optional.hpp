#ifndef DCOOL_CORE_OPTIONAL_HPP_INCLUDED_
#	define DCOOL_CORE_OPTIONAL_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/utility.hpp>

#	include <optional>

namespace dcool::core {
	using BadOptionalAccess = ::std::bad_optional_access;
	using NullOptional = ::std::nullopt_t;

	constexpr ::dcool::core::NullOptional nullOptional = ::std::nullopt;

	namespace detail_ {
		template <
			typename T_, typename OptionalT_
		> concept OptionalDirectionInitializeArgumentFor_ = !::dcool::core::FormOfOneOf<
			T_, OptionalT_, ::dcool::core::InPlaceTag, ::dcool::core::NullOptional
		>;

		template <typename T_, typename ValueT_> concept OptionalConfigWithInvalidValue_ = requires {
			{ T_::invalidValue() } -> ::dcool::core::ConvertibleTo<ValueT_>;
		};

		template <
			typename T_, typename ValueT_
		> concept OptionalConfigWithInvalidValueVerifier_ = requires (ValueT_ const& value_) {
			{ T_::isInvalidValue(value_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <
			typename ValueT_, typename ConfigT_
		> constexpr auto isInvalidValueForOptional_(ValueT_ const& value_) -> ::dcool::core::Boolean {
			return value_ == ConfigT_::invalidValue();
		}

		template <
			typename ValueT_, ::dcool::core::detail_::OptionalConfigWithInvalidValueVerifier_<ValueT_> ConfigT_
		> constexpr auto isInvalidValueForOptional_(ValueT_ const& value_) -> ::dcool::core::Boolean {
			return value_ == ConfigT_::invalidValue();
		}

		template <typename ConfigT_, typename ValueT_> struct OptionalConfigAdaptor_ {
			private: using Self_ = OptionalConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: static constexpr ::dcool::core::Boolean independentValidity = true;
		};

		template <
			typename ValueT_, ::dcool::core::detail_::OptionalConfigWithInvalidValue_<ValueT_> ConfigT_
		> struct OptionalConfigAdaptor_<ConfigT_, ValueT_> {
			private: using Self_ = OptionalConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: static constexpr ::dcool::core::Boolean independentValidity = false;

			public: static constexpr auto invalidValue() -> Value {
				return Config::invalidValue();
			}

			public: static constexpr auto isInvalidValue(Value const& value_) -> ::dcool::core::Boolean {
				return ::dcool::core::detail_::isInvalidValueForOptional_<Value, Config>(value_);
			}
		};
	}

	template <typename T_, typename ValueT_> concept OptionalConfig = requires {
		typename ::dcool::core::detail_::OptionalConfigAdaptor_<T_, ValueT_>;
	};

	template <
		typename T_, typename ValueT_
	> requires OptionalConfig<
		T_, ValueT_
	> using OptionalConfigAdaptor = ::dcool::core::detail_::OptionalConfigAdaptor_<T_, ValueT_>;

	namespace detail_ {
		template <typename ValueT_, typename ConfigT_> struct Optional_ {
			public: using Self_ = Optional_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using Underlying_ = ::std::optional<Value>;
			public: static constexpr ::dcool::core::Boolean independentValidity = false;

			private: Underlying_ m_underlying_;

			public: constexpr Optional_() noexcept = default;
			public: constexpr Optional_(Self_ const& other_) = default;
			public: constexpr Optional_(Self_&& other_) = default;

			public: constexpr Optional_(::dcool::core::NullOptional) noexcept: m_underlying_(::std::nullopt) {
			}

			public: template <
				::dcool::core::detail_::OptionalDirectionInitializeArgumentFor_<Self_> ValueT__
			> constexpr explicit(
				::dcool::core::ConvertibleTo<ValueT__&&, Value>
			) Optional_(ValueT__&& value_) noexcept(
				noexcept(Underlying_(::dcool::core::forward<ValueT__>(value_)))
			): m_underlying_(::dcool::core::forward<ValueT__>(value_)) {
			}

			public: template <typename... ArgumentTs_> constexpr explicit Optional_(
				::dcool::core::InPlaceTag, ArgumentTs_&&... parameters_
			) noexcept(
				noexcept(Underlying_(::std::in_place, ::dcool::core::forward<ArgumentTs_>(parameters_)...))
			): m_underlying_(::std::in_place, ::dcool::core::forward<ArgumentTs_>(parameters_)...) {
			}

			public: constexpr ~Optional_() noexcept = default;
			public: constexpr auto operator =(Self_ const& other_) -> Self_& = default;
			public: constexpr auto operator =(Self_&& other_) -> Self_& = default;

			public: constexpr auto operator =(::dcool::core::NullOptional) -> Self_& {
				this->m_underlying_ = ::std::nullopt;
				return *this;
			}

			public: template <
				::dcool::core::detail_::OptionalDirectionInitializeArgumentFor_<Self_> ValueT__
			> constexpr auto operator =(ValueT__&& value_) -> Self_& {
				this->m_underlying_ = ::dcool::core::forward<ValueT__>(value_);
				return *this;
			}

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

		template <
			typename ValueT_, ::dcool::core::detail_::OptionalConfigWithInvalidValue_<ValueT_> ConfigT_
		> struct Optional_<ValueT_, ConfigT_> {
			public: using Self_ = Optional_<ValueT_, ConfigT_>;
			public: using Value = ValueT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::core::OptionalConfigAdaptor<Config, Value>;
			public: static constexpr ::dcool::core::Boolean independentValidity = true;

			private: Value m_value_;

			public: constexpr Optional_(): m_value_(ConfigAdaptor_::invalidValue()) {
			}

			// A bug prevent us from using 'Self_' or 'Optional_<ValueT_, ConfigT_>' in the copy constructor.
			// See document/dependency_bugs#Bug_3 for mor details.
			public: constexpr Optional_(Optional_ const& other_) = default;
			public: constexpr Optional_(Self_&& other_) = default;

			public: constexpr Optional_(::dcool::core::NullOptional) noexcept(noexcept(Self_())): Self_() {
			}

			public: template <
				::dcool::core::detail_::OptionalDirectionInitializeArgumentFor_<Self_> ValueT__
			> constexpr Optional_(ValueT__&& value_) noexcept(
				noexcept(Value(::dcool::core::forward<ValueT__>(value_)))
			): m_value_(::dcool::core::forward<ValueT__>(value_)) {
			}

			public: constexpr ~Optional_() noexcept = default;
			// A bug prevent us from using 'Self_' or 'Optional_<ValueT_, ConfigT_>' in the copy assignment operator.
			// See document/dependency_bugs#Bug_3 for mor details.
			public: constexpr auto operator =(Optional_ const& other_) -> Self_& = default;
			public: constexpr auto operator =(Self_&& other_) -> Self_& = default;

			public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
				return ConfigAdaptor_::isInvalidValue(this->access());
			}

			public: constexpr auto operator =(::dcool::core::NullOptional) -> Self_& {
				this->reset();
				return *this;
			}

			public: template <
				::dcool::core::detail_::OptionalDirectionInitializeArgumentFor_<Self_> ValueT__
			> constexpr auto operator =(ValueT__&& value_) -> Self_& {
				this->m_value_ = ::dcool::core::forward<ValueT__>(value_);
				return *this;
			}

			public: constexpr auto access() const noexcept -> Value const& {
				return this->m_value_;
			}

			public: constexpr auto access() noexcept -> Value& {
				return this->m_value_;
			}

			public: constexpr auto value() const -> Value const& {
				if (!(this->valid())) {
					throw ::dcool::core::BadOptionalAccess();
				}
				return this->access();
			}

			public: constexpr auto value() -> Value& {
				if (!(this->valid())) {
					throw ::dcool::core::BadOptionalAccess();
				}
				return this->access();
			}

			public: constexpr void reset() noexcept {
				this->m_value_ = ConfigAdaptor_::invalidValue();
			}
		};
	}

	template <
		typename ValueT_, OptionalConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> using Optional = ::dcool::core::detail_::Optional_<ValueT_, ConfigT_>;

	namespace detail_ {
		template <typename ValueT_, ValueT_ invalidValueC_> struct CompactOptional_ {
			struct Config_ {
				static constexpr auto invalidValue() -> ValueT_ {
					return invalidValueC_;
				}
			};

			using Result_ = ::dcool::core::Optional<ValueT_, Config_>;
		};
	}

	template <
		typename ValueT_, ValueT_ invalidValueC_
	> using CompactOptional = ::dcool::core::detail_::CompactOptional_<ValueT_, invalidValueC_>::Result;
}

#endif
