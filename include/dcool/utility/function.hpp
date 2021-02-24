#ifndef DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_
#	define DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_

#	include <dcool/utility/any.hpp>

#	include <dcool/core.hpp>

#	include <functional>

namespace dcool::utility {
	using BadFunctionCall = ::std::bad_function_call;

	namespace detail_ {
		enum class DefaultFunctionOpcode_ {
			DCOOL_UTILITY_FUNCTION_BASIC_OPERATIONS
		};

		template <typename PrototypeT_, typename ConfigT_> struct FunctionChassis_;

		template <typename ConfigT_, typename PrototypeT_> class FunctionConfigAdaptor_;

		template <
			typename ConfigT_, typename ReturnT_, typename... ParameterTs_
		> class FunctionConfigAdaptor_<ConfigT_, ReturnT_(ParameterTs_...)> {
			private: using Self_ = AnyConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Prototype = ReturnT_(ParameterTs_...);
			public: using Return = ReturnT_;
			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using DynamicHandle = ::dcool::resource::UnifiedHandleType<Pool>;
			public: using ExtendedInformation = ::dcool::utility::detail_::ExtractedExtendedInformationType_<
				Config, ::dcool::core::Pit
			>;
			public: static constexpr ::dcool::core::StorageRequirement squeezedTankage =
				::dcool::utility::detail_::extractedSqueezedTankage_<Config>(::dcool::core::storageRequirementFor<DynamicHandle>)
			;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				::dcool::core::exceptionSafetyStrategyOf<Config>
			;

			private: struct DefaultEngine_ {
				private: using Self_ = DefaultEngine_;

				public: [[no_unique_address]] Pool partPool;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}

				public: friend auto operator ==(Self_ const&, Self_ const&) -> ::dcool::core::Boolean = default;
				public: friend auto operator !=(Self_ const&, Self_ const&) -> ::dcool::core::Boolean = default;
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::pool'"
			);

			static_assert(sizeof(DynamicHandle) > 0);
		};

		template <
			typename ConfigT_, typename ReturnT_, typename... ParameterTs_
		> struct FunctionChassis_<ReturnT_(ParameterTs_...), ConfigT_> {
			private: using Self_ = FunctionChassis_<ReturnT_(ParameterTs_...), ConfigT_>;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, ReturnT_(ParameterTs_...)>;
			public: using Prototype = ConfigAdaptor_::Prototype;
			public: using Return = ConfigAdaptor_::Return;
			public: using Pool = ConfigAdaptor_::Pool;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
			public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			private: using ImmutableInvoker_ = auto (*)(Engine& engine_, Self_ const& self_, ParameterTs_... parameters_) -> Return;
			private: using Invoker_ = auto (*)(Engine& engine_, Self_& self_, ParameterTs_... parameters_) -> Return;

			private: struct AnyExtendedInformation_ {
				ImmutableInvoker_ immutableInvoker_;
				Invoker_ invoker_;
				::dcool::core::Boolean immutablyInvocable_;
				ExtendedInformation extendedInformation_;

				template <typename ValueT__> constexpr AnyExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept:
					immutableInvoker_(immutablyInvoke_<ValueT__>),
					invoker_(invoke_<ValueT__>),
					immutablyInvocable_(isImmutablyInvocable_<ValueT__>()),
					extendedInformation_(typed_)
				{
				}
			};

			private: struct AnyConfig_ {
				using Pool = Pool;
				using ExtendedInformation = AnyExtendedInformation_;
				using Engine = Engine;
				static constexpr ::dcool::core::StorageRequirement squeezedTankage = squeezedTankage;
				static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
			};

			private: using Underlying_ = ::dcool::utility::AnyChassis<AnyConfig_>;

			private: Underlying_ m_underlying_;

			private: template <typename ValueT__> static constexpr auto immutablyInvoke_(
				Engine& engine_, Self_ const& self_, ParameterTs_... parameters_
			) -> Return {
				if constexpr (isImmutablyInvocable_<ValueT__>()) {
					return ::dcool::core::invoke(self_.access<ValueT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...);
				}
				throw ::dcool::utility::BadFunctionCall();
			}

			private: template <typename ValueT__> static constexpr auto invoke_(
				Engine& engine_, Self_& self_, ParameterTs_... parameters_
			) -> Return {
				if constexpr (::dcool::core::NonVoid<ValueT__>) {
					return ::dcool::core::invoke(self_.access<ValueT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...);
				}
				throw ::dcool::utility::BadFunctionCall();
			}

			private: template <typename ValueT__> static consteval auto isImmutablyInvocable_() -> ::dcool::core::Boolean {
				if constexpr (::dcool::core::NonVoid<ValueT__>) {
					return ::dcool::core::Invocable<ValueT__ const&, ParameterTs_...>;
				}
				return false;
			}

			public: void initialize(Engine& engine_) noexcept {
				this->m_underlying_.initialize(engine_);
			}

			public: template <typename ValueT__> constexpr void initialize(Engine& engine_, ValueT__&& value_) noexcept(
				noexcept(::dcool::core::declval<Underlying_&>().initialize(engine_, ::dcool::core::forward<ValueT__>(value_)))
			) {
				this->m_underlying_.initialize(engine_, ::dcool::core::forward<ValueT__>(value_));
			}

			public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
				Engine& engine_, ::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
			) noexcept(
				noexcept(
					::dcool::core::declval<Underlying_&>().template initialize<ValueT__>(
						engine_,::dcool::core::forward<ArgumentTs__>(parameters_)...
					)
				)
			) {
				this->m_underlying_.template initialize<ValueT__>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				this->m_underlying_.uninitialize(engine_);
			}

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
				this->m_underlying_.cloneTo(engine_, otherEngine_, other_.m_underlying_);
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->m_underlying_.template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, other_.m_underlying_);
			}

			public: public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->m_underlying_.template swapWith<strategyC__, engineWillBeSwappedC__>(engine_, otherEngine_, other_.m_underlying_);
			}

			public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				return this->m_underlying_.valid(engine_);
			}

			public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
				return this->m_underlying_.storageRequirement(engine_);
			}

			public: constexpr auto immutablyInvocable(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				return this->m_underlying_.extendedInformation(engine_).immutablyInvocable_;
			}

			public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
				return this->m_underlying_.typeInfo(engine_);
			}

			public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation const& {
				return this->m_underlying_.extendedInformation(engine_).extendedInformation_;
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
				return this->m_underlying_.template access<ValueT__>(engine_);
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
				return this->m_underlying_.template access<ValueT__>(engine_);
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) const -> ValueT__ const& {
				return this->m_underlying_.template value<ValueT__>(engine_);
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) -> ValueT__& {
				return this->m_underlying_.template value<ValueT__>(engine_);
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) const -> Return {
				return this->m_underlying_.extendedInformation(engine_).immutableInvoker_(
					engine_, *this, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) -> Return {
				return this->m_underlying_.extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}
		};
	}

	template <typename T_, typename PrototypeT_> concept FunctionConfig = requires {
		typename ::dcool::utility::detail_::FunctionConfigAdaptor_<T_, PrototypeT_>;
	};

	template <
		typename ConfigT_, typename PrototypeT_
	> requires ::dcool::utility::FunctionConfig<ConfigT_, PrototypeT_> using FunctionConfigAdaptor =
		::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, PrototypeT_>
	;

	template <
		typename PrototypeT_, ::dcool::utility::FunctionConfig<PrototypeT_> ConfigT_ = ::dcool::core::Empty<>
	> using FunctionChassis = ::dcool::utility::detail_::FunctionChassis_<PrototypeT_, ConfigT_>;

	template <
		typename PrototypeT_, ::dcool::utility::FunctionConfig<PrototypeT_> ConfigT_ = ::dcool::core::Empty<>
	> struct Function;

	template <
		typename ConfigT_, typename ReturnT_, typename... ParameterTs_
	> struct Function<ReturnT_(ParameterTs_...), ConfigT_> {
		private: using Self_ = Function<ReturnT_(ParameterTs_...), ConfigT_>;
		public: using Return = ReturnT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::utility::FunctionConfigAdaptor<ConfigT_, ReturnT_(ParameterTs_...)>;
		public: using Prototype = ConfigAdaptor_::Prototype;
		public: using Chassis = ::dcool::utility::FunctionChassis<Prototype, Config>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr Function() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr Function(Self_ const& other_) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine, *this);
		}

		public: constexpr Function(Self_&& other_) {
			other_.chassis().relocateTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(this->mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr Function(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr Function(
			::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
		) {
			this->chassis().template initialize<ValueT__>(
				this->mutableEngine(), ::dcool::core::inPlace, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: constexpr ~Function() noexcept {
			this->chassis().uninitialize(this->mutableEngine());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) -> Self_& {
			this->swapWith(other_);
			return *this;
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr auto operator =(
			ValueT__&& value_
		) -> Self_& {
			Self_ middleMan_(::dcool::core::forward<ValueT__>(value_));
			this->swapWith(middleMan_);
			return *this;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void swapWith(Self_& other_) {
			this->chassis().template swapWith<strategyC__, true>(this->mutableEngine(), other_.mutableEngine(), other_.chassis());
			::dcool::core::intelliSwap(this->mutableEngine(), other_.mutableEngine());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		protected: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		protected: constexpr auto mutableEngine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().valid(this->mutableEngine());
		}

		public: constexpr auto storageRequirement() const noexcept {
			return this->chassis().storageRequirement(this->mutableEngine());
		}

		public: constexpr auto immutablyInvocable() const noexcept -> ::dcool::core::Boolean {
				return this->chassis().immutablyInvocable(this->mutableEngine());
			}

		public: constexpr auto typeInfo() const noexcept -> ::dcool::core::TypeInfo const& {
			return this->chassis().typeInfo(this->mutableEngine());
		}

		public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation const& {
			return this->chassis().extendedInformation(this->mutableEngine());
		}

		public: template <typename ValueT__> constexpr auto access() const noexcept -> ValueT__ const& {
			return this->chassis().template access<ValueT__>(this->mutableEngine());
		}

		public: template <typename ValueT__> constexpr auto access() noexcept -> ValueT__& {
			return this->chassis().template access<ValueT__>(this->mutableEngine());
		}

		public: template <typename ValueT__> constexpr auto value() const -> ValueT__ const& {
			return this->chassis().template value<ValueT__>(this->mutableEngine());
		}

		public: template <typename ValueT__> constexpr auto value() -> ValueT__& {
			return this->chassis().template value<ValueT__>(this->mutableEngine());
		}

		public: constexpr auto invokeSelf(ParameterTs_... parameters_) const -> Return {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ParameterTs_>(parameters_)...);
		}

		public: constexpr auto invokeSelf(ParameterTs_... parameters_) -> Return {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ParameterTs_>(parameters_)...);
		}

		public: constexpr auto operator()(ParameterTs_... parameters_) const -> Return {
			return this->invokeSelf(::dcool::core::forward<ParameterTs_>(parameters_)...);
		}

		public: constexpr auto operator()(ParameterTs_... parameters_) -> Return {
			return this->invokeSelf(::dcool::core::forward<ParameterTs_>(parameters_)...);
		}
	};
}

#endif
