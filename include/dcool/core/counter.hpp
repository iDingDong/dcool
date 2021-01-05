#ifndef DCOOL_CORE_COUNTER_HPP_INCLUDED_
#	define DCOOL_CORE_COUNTER_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/functional.hpp>
#	include <dcool/core/member_detector.hpp>

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

	namespace detail_ {
		template <::dcool::core::Integer UnderlyingT_, ::dcool::core::CounterScenario scenarioC_> struct Counter_ {
			private: using Self_ = Counter_<UnderlyingT_, scenarioC_>;
			public: using Underlying = UnderlyingT_;
			public: static constexpr ::dcool::core::CounterScenario scenario = scenarioC_;

			private: static constexpr ::std::memory_order readOrder_ = ((
				scenario == ::dcool::core::CounterScenario::statisticsOnly
			) ? ::std::memory_order::relaxed : ::std::memory_order::acquire);

			private: static constexpr ::std::memory_order writeOrder_ = ((
				scenario == ::dcool::core::CounterScenario::statisticsOnly
			) ? ::std::memory_order::relaxed : ::std::memory_order::release);

			private: static constexpr ::std::memory_order readWriteOrder_ = ((
				scenario == ::dcool::core::CounterScenario::statisticsOnly
			) ? ::std::memory_order::relaxed : ::std::memory_order::acq_rel);

			private: ::std::atomic<Underlying> m_value_;

			public: constexpr explicit Counter_(Underlying initialCount_ = 0) noexcept: m_value_(initialCount_) {
			}

			public: constexpr auto increment(Underlying difference_ = 1) noexcept -> Underlying {
				return this->m_value_.fetch_add(difference_, readWriteOrder_) + difference_;
			}

			public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryIncrementIf(
				PredicatorT__&& predicator_, Underlying difference_ = 1
			) noexcept -> ::dcool::core::Boolean {
				Underlying previousValue_ = this->value();
				do {
					::dcool::core::Boolean confirmed = ::dcool::core::invoke(
						::dcool::core::forward<PredicatorT__>(predicator_), previousValue_
					);
					if (!confirmed) {
						return false;
					}
				} while (
					!(this->m_value_.compare_exchange_weak(previousValue_, previousValue_ + difference_, readWriteOrder_, readOrder_))
				);
				return true;
			}

			public: constexpr auto decrement(Underlying difference_ = 1) noexcept -> Underlying {
				return this->m_value_.fetch_sub(difference_, readWriteOrder_) - difference_;
			}

			public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryDecrementIf(
				PredicatorT__&& predicator_, Underlying difference_ = 1
			) noexcept -> ::dcool::core::Boolean {
				Underlying previousValue_ = this->value();
				do {
					::dcool::core::Boolean confirmed = ::dcool::core::invoke(
						::dcool::core::forward<PredicatorT__>(predicator_), previousValue_
					);
					if (!confirmed) {
						return false;
					}
				} while (
					!(this->m_value_.compare_exchange_weak(previousValue_, previousValue_ - difference_, readWriteOrder_, readOrder_))
				);
				return true;
			}

			public: constexpr auto value() const noexcept -> Underlying {
				return this->m_value_.load(readOrder_);
			}

			public: constexpr void setValue(Underlying value_) noexcept {
				this->m_value_.store(value_, writeOrder_);
			}
		};

		template <::dcool::core::Integer UnderlyingT_> struct Counter_<UnderlyingT_, ::dcool::core::CounterScenario::synchronized> {
			private: using Self_ = Counter_<UnderlyingT_, ::dcool::core::CounterScenario::synchronized>;
			public: using Underlying = UnderlyingT_;
			public: static constexpr ::dcool::core::CounterScenario scenario =
				::dcool::core::CounterScenario::synchronized
			;

			private: Underlying m_value_;

			public: constexpr explicit Counter_(Underlying initialCount_ = 0) noexcept: m_value_(initialCount_) {
			}

			public: constexpr auto increment(Underlying difference_ = 1) noexcept -> Underlying {
				return this->m_value_ += difference_;
			}

			public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryIncrementIf(
				PredicatorT__&& predicator_, Underlying difference_ = 1
			) noexcept -> ::dcool::core::Boolean {
				::dcool::core::Boolean confirmed = ::dcool::core::invoke(
					::dcool::core::forward<PredicatorT__>(predicator_), this->value()
				);
				if (!confirmed) {
					return false;
				}
				this->increment(difference_);
				return true;
			}

			public: constexpr auto decrement(Underlying difference_ = 1) noexcept -> Underlying {
				return this->m_value_ -= difference_;
			}

			public: template <::dcool::core::Predicator<Underlying> PredicatorT__> constexpr auto tryDecrementIf(
				PredicatorT__&& predicator_, Underlying difference_ = 1
			) noexcept -> ::dcool::core::Boolean {
				::dcool::core::Boolean confirmed = ::dcool::core::invoke(
					::dcool::core::forward<PredicatorT__>(predicator_), this->value()
				);
				if (!confirmed) {
					return false;
				}
				this->decrement(difference_);
				return true;
			}

			public: constexpr auto value() const noexcept -> Underlying {
				return this->m_value_;
			}

			public: constexpr void setValue(Underlying value_) noexcept {
				this->m_value_ = value_;
			}
		};
	}

	template <::dcool::core::CounterConfig ConfigT_ = ::dcool::core::Empty<>> using Counter = ::dcool::core::detail_::Counter_<
		typename ::dcool::core::CounterConfigAdaptor<ConfigT_>::Underlying, ::dcool::core::CounterConfigAdaptor<ConfigT_>::scenario
	>;
}

#endif
