#ifndef DCOOL_CORE_RELAY_HPP_INCLUDED_
#	define DCOOL_CORE_RELAY_HPP_INCLUDED_

#	include <dcool/core/concept.hpp>
#	include <dcool/core/type_transformer.hpp>

namespace dcool::core {
	namespace detail_ {
		template <
			typename ValueT_, typename InputValueT_
		> constexpr decltype(auto) valueToUnderlying_(InputValueT_&& value_) noexcept {
			if constexpr (::dcool::core::Reference<ValueT_>) {
				return ::dcool::core::addressOf(value_);
			}
			return ::dcool::core::forward<InputValueT_>(value_);
		}
	}

	template <typename ValueT_> struct Relay {
		private: using Self_ = Relay<ValueT_>;
		public: using Value = ValueT_;

		private: static constexpr ::dcool::core::Boolean storingReference_ = ::dcool::core::Reference<Value>;

		private: using Underlying_ = ::dcool::core::ConditionalType<
			storingReference_, ::dcool::core::ReferenceRemovedType<Value>*, Value
		>;

		private: [[no_unique_address]] Underlying_ m_underlying_;

		public: template <typename ValueT__> constexpr explicit(!::dcool::core::ConvertibleTo<ValueT__&&, Value>) Relay(
			ValueT__&& value_
		) requires (!::dcool::core::FormOfSame<Self_, ValueT__>): m_underlying_(
			::dcool::core::detail_::valueToUnderlying_<Value>(::dcool::core::forward<ValueT__>(value_))
		) {
		}

		public: constexpr auto value() const& noexcept -> Value {
			if constexpr (storingReference_) {
				return static_cast<Value>(*(this->m_underlying_));
			}
			return this->m_underlying_;
		}

		public: constexpr auto value()&& noexcept -> Value {
			if constexpr (storingReference_) {
				return static_cast<Value>(*(this->m_underlying_));
			}
			return ::dcool::core::move(this->m_underlying_);
		}
	};
}

#endif
