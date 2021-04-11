#ifndef DCOOL_CORE_COUNTER_HPP_INCLUDED_
#	define DCOOL_CORE_COUNTER_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/functional.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/type_transformer.hpp>

#	include <atomic>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core::detail_, HasTypeUnderlying_, ExtractedUnderlyingType_, Underlying)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(dcool::core::detail_, HasValueScenario_, extractedScenarioValue_, scenario)

namespace dcool::core {
	enum class CounterScenario {
		logicDependent,
		statisticsOnly,
		synchronized
	};

	namespace detail_ {
		template <typename ConfigT_> class CounterConfigAdaptor_ {
			private: using Self_ = CounterConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Underlying = ::dcool::core::detail_::ExtractedUnderlyingType_<Config, ::dcool::core::SignedMaxInteger>;
			public: static constexpr ::dcool::core::CounterScenario scenario =
				::dcool::core::detail_::extractedScenarioValue_<Config>(::dcool::core::CounterScenario::synchronized)
			;
		};
	}

	template <typename T_> concept CounterConfig = ::dcool::core::Integer<
		typename ::dcool::core::detail_::CounterConfigAdaptor_<T_>::Underlying
	>;

	template <
		::dcool::core::CounterConfig ConfigT_
	> using CounterConfigAdaptor = ::dcool::core::detail_::CounterConfigAdaptor_<ConfigT_>;

	template <::dcool::core::CounterConfig ConfigT_> struct Counter {
		private: using Self_ = Counter<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdator_ = ::dcool::core::CounterConfigAdaptor<Config>;
		public: using Underlying = ConfigAdator_::Underlying;
		public: static constexpr ::dcool::core::CounterScenario scenario = ConfigAdator_::scenario;

		private: static constexpr ::std::memory_order readOrder_ = ((
			scenario == ::dcool::core::CounterScenario::statisticsOnly
		) ? ::std::memory_order::relaxed : ::std::memory_order::acquire);

		private: static constexpr ::std::memory_order writeOrder_ = ((
			scenario == ::dcool::core::CounterScenario::statisticsOnly
		) ? ::std::memory_order::relaxed : ::std::memory_order::release);

		private: static constexpr ::std::memory_order readWriteOrder_ = ((
			scenario == ::dcool::core::CounterScenario::statisticsOnly
		) ? ::std::memory_order::relaxed : ::std::memory_order::acq_rel);

		private: ::dcool::core::ConditionalType<
			scenario == ::dcool::core::CounterScenario::synchronized, Underlying, ::std::atomic<Underlying>
		> m_value_;

		public: struct PositiveToken {
			private: using Self_ = PositiveToken;
			private: using Refered_ = Counter<Config>;
			private: friend Refered_;

			private: Refered_* m_counter_;
			private: Underlying m_value_;

			private: constexpr PositiveToken(
				Refered_& counter_, Underlying value_
			) noexcept: m_counter_(::dcool::core::addressOf(counter_)), m_value_(value_) {
				counter_.increment(value_);
			}

			public: PositiveToken(Self_ const&) = delete;

			public: constexpr PositiveToken(Self_&& other_) noexcept: m_counter_(other_.m_counter_), m_value_(other_.m_value_) {
				other_.m_value_ = 0;
			}

			public: constexpr ~PositiveToken() noexcept {
				this->m_counter_->decrement(this->m_value_);
			}

			public: auto operator =(Self_ const& other_) -> Self_& = delete;

			public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
				::dcool::core::intelliSwap(this->m_counter_, other_.m_counter_);
				::dcool::core::intelliSwap(this->m_value_, other_.m_value_);
			}
		};

		public: struct NegativeToken {
			private: using Self_ = NegativeToken;
			private: using Refered_ = Counter<Config>;
			private: friend Refered_;

			private: Refered_* m_counter_;
			private: Underlying m_value_;

			private: constexpr NegativeToken(
				Refered_& counter_, Underlying value_
			) noexcept: m_counter_(::dcool::core::addressOf(counter_)), m_value_(value_) {
				counter_.decrement(value_);
			}

			public: NegativeToken(Self_ const&) = delete;

			public: constexpr NegativeToken(Self_&& other_) noexcept: m_counter_(other_.m_counter_), m_value_(other_.m_value_) {
				other_.m_value_ = 0;
			}

			public: constexpr ~NegativeToken() noexcept {
				this->m_counter_->increment(this->m_value_);
			}

			public: auto operator =(Self_ const& other_) -> Self_& = delete;

			public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
				::dcool::core::intelliSwap(this->m_counter_, other_.m_counter_);
				::dcool::core::intelliSwap(this->m_value_, other_.m_value_);
			}
		};

		public: constexpr explicit Counter(Underlying initialCount_ = 0) noexcept: m_value_(initialCount_) {
		}

		public: constexpr auto increment(Underlying difference_ = 1) noexcept -> Underlying {
			Underlying result_;
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				this->m_value_ += difference_;
				result_ = this->m_value_;
			} else {
				result_ = this->m_value_.fetch_add(difference_, readWriteOrder_) + difference_;
			}
			return result_;
		}

		public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryIncrementIf(
			PredicatorT__&& predicator_, Underlying difference_ = 1
		) noexcept -> ::dcool::core::Boolean {
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				if (::dcool::core::invoke(::dcool::core::forward<PredicatorT__>(predicator_), this->m_value_)) {
					this->increment(difference_);
					return true;
				}
			} else {
				Underlying previousValue_ = this->value();
				while (::dcool::core::invoke(predicator_, previousValue_)) {
					if (
						this->m_value_.compare_exchange_weak(previousValue_, previousValue_ + difference_, readWriteOrder_, readOrder_)
					) {
						return true;
					}
				}
			}
			return false;
		}

		public: constexpr auto positiveBorrow(Underlying difference_ = 1) noexcept -> PositiveToken {
			return PositiveToken(*this, difference_);
		}

		public: constexpr auto decrement(Underlying difference_ = 1) noexcept -> Underlying {
			Underlying result_;
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				this->m_value_ -= difference_;
				result_ = this->m_value_;
			} else {
				result_ = this->m_value_.fetch_sub(difference_, readWriteOrder_) - difference_;
			}
			return result_;
		}

		public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryDecrementIf(
			PredicatorT__&& predicator_, Underlying difference_ = 1
		) noexcept -> ::dcool::core::Boolean {
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				if (::dcool::core::invoke(::dcool::core::forward<PredicatorT__>(predicator_), this->m_value_)) {
					this->decrement(difference_);
					return true;
				}
			} else {
				Underlying previousValue_ = this->value();
				while (::dcool::core::invoke(predicator_, previousValue_)) {
					if (
						this->m_value_.compare_exchange_weak(previousValue_, previousValue_ - difference_, readWriteOrder_, readOrder_)
					) {
						return true;
					}
				}
			}
			return false;
		}

		public: constexpr auto negativeBorrow(Underlying difference_ = 1) noexcept -> NegativeToken {
			return NegativeToken(*this, difference_);
		}

		public: constexpr auto value() const noexcept -> Underlying {
			Underlying result_;
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				result_ = this->m_value_;
			} else {
				result_ = this->m_value_.load(readOrder_);
			}
			return result_;
		}

		public: constexpr void setValue(Underlying value_) noexcept {
			if constexpr (scenario == ::dcool::core::CounterScenario::synchronized) {
				this->m_value_ = value_;
			} else {
				this->m_value_.store(value_, writeOrder_);
			}
		}
	};
}

#endif
