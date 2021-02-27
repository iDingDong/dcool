#ifndef DCOOL_UTILITY_OVERLOADED_FUNCTION_HPP_INCLUDED_
#	define DCOOL_UTILITY_OVERLOADED_FUNCTION_HPP_INCLUDED_ 1

#	include <dcool/utility/any.hpp>
#	include <dcool/utility/function.hpp>

#	include <dcool/core.hpp>

namespace dcool::utility {
	namespace detail_ {
		template <
			::dcool::core::Complete ConfigT_, ::dcool::core::TypeList PrototypeT_
		> struct OverloadedFunctionConfigAdaptor_: public ::dcool::utility::AnyConfigAdaptor<ConfigT_> {
		};

		template <::dcool::core::TypeList PrototypesT_, typename ConfigT_> struct OverloadedFunctionChassis_;

		template <
			typename ConfigT_, typename PrototypeT_
		> requires (!::dcool::core::TypeList<PrototypeT_>) struct OverloadedFunctionChassis_<
			::dcool::core::Types<PrototypeT_>, ConfigT_
		>: private ::dcool::utility::FunctionChassis<PrototypeT_, ConfigT_> {
			private: using Self_ = OverloadedFunctionChassis_<::dcool::core::Types<PrototypeT_>, ConfigT_>;
			private: using Super_ = ::dcool::utility::FunctionChassis<PrototypeT_, ConfigT_>;
			public: using Prototypes = ::dcool::core::Types<PrototypeT_>;
			public: using Config = ConfigT_;

			public: using typename Super_::Engine;
			public: using typename Super_::ExtendedInformation;
			public: using Super_::exceptionSafetyStrategy;
			public: using Super_::initialize;
			public: using Super_::uninitialize;
			public: using Super_::valid;
			public: using Super_::storageRequirement;
			public: using Super_::typeInfo;
			public: using Super_::extendedInformation;
			public: using Super_::access;
			public: using Super_::value;
			public: using Super_::invokeSelf;

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
				this->Super_::cloneTo(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template swapWith<strategyC__, engineWillBeSwappedC__>(
					engine_, otherEngine_, static_cast<Super_&>(other_)
				);
			}

			public: template <
				::dcool::core::Index indexC__
			> constexpr auto immutablyInvocable(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				static_assert(
					indexC__ == 0, "'dcool::utility::OverloadedFunctionChassis::immutablyInvocable' found index out of range."
				);
				return this->Super_::immutablyInvocable(engine_);
			}
		};

		template <
			typename PrototypeT_, ::dcool::core::TypeList OtherPrototypesT_, typename ConfigT_
		> struct OverloadedFunctionChassisBase_;

		template <
			typename BaseT_, typename ConfigT_, typename OtherPrototypesT_, typename ReturnT_, typename... ParameterTs_
		> struct OverloadedFunctionChassisBaseUnderlyingConfig_ {
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::utility::detail_::OverloadedFunctionConfigAdaptor_<Config, OtherPrototypesT_>;
			private: using Engine = ConfigAdaptor_::Engine;
			private: using ImmutableInvoker_ = auto (*)(
				Engine& engine_, BaseT_ const& self_, ParameterTs_... parameters_
			) -> ReturnT_;
			private: using Invoker_ = auto (*)(Engine& engine_, BaseT_& self_, ParameterTs_... parameters_) -> ReturnT_;

			private: struct UnderlyingExtendedInformation_ {
				ImmutableInvoker_ immutableInvoker_;
				Invoker_ invoker_;
				::dcool::core::Boolean immutablyInvocable_;
				ConfigAdaptor_::ExtendedInformation extendedInformation_;

				template <typename ValueT__> constexpr UnderlyingExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept:
					immutableInvoker_(
						::dcool::utility::detail_::immutablyInvokeFunctionChassis_<BaseT_, ValueT__, ReturnT_, ParameterTs_...>
					),
					invoker_(::dcool::utility::detail_::invokeFunctionChassis_<BaseT_, ValueT__, ReturnT_, ParameterTs_...>),
					immutablyInvocable_(::dcool::utility::detail_::isImmutablyInvocable_<ValueT__, ParameterTs_...>),
					extendedInformation_(typed_)
				{
				}
			};

			public: struct Result_ {
				using Pool = ConfigAdaptor_::Pool;
				using ExtendedInformation = UnderlyingExtendedInformation_;
				using Engine = Engine;
				static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
				static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
					ConfigAdaptor_::exceptionSafetyStrategy
				;
			};
		};

		template <
			typename OtherPrototypesT_, typename ConfigT_, typename ReturnT_, typename... ParameterTs_
		> struct OverloadedFunctionChassisBase_<auto(ParameterTs_...) -> ReturnT_, OtherPrototypesT_, ConfigT_>:
			private ::dcool::utility::detail_::OverloadedFunctionChassis_<
				OtherPrototypesT_,
				typename ::dcool::utility::detail_::OverloadedFunctionChassisBaseUnderlyingConfig_<
					OverloadedFunctionChassisBase_<auto(ParameterTs_...) -> ReturnT_, OtherPrototypesT_, ConfigT_>,
					ConfigT_,
					OtherPrototypesT_,
					ReturnT_,
					ParameterTs_...
				>::Result_
			>
		{
			private: using Self_ = OverloadedFunctionChassisBase_<auto(ParameterTs_...) -> ReturnT_, OtherPrototypesT_, ConfigT_>;
			private: using Super_ = ::dcool::utility::detail_::OverloadedFunctionChassis_<
				OtherPrototypesT_,
				typename ::dcool::utility::detail_::OverloadedFunctionChassisBaseUnderlyingConfig_<
					OverloadedFunctionChassisBase_<auto(ParameterTs_...) -> ReturnT_, OtherPrototypesT_, ConfigT_>,
					ConfigT_,
					OtherPrototypesT_,
					ReturnT_,
					ParameterTs_...
				>::Result_
			>;
			private: using Config = ConfigT_;

			public: using typename Super_::Engine;
			public: using Super_::exceptionSafetyStrategy;
			public: using Super_::initialize;
			public: using Super_::uninitialize;
			public: using Super_::valid;
			public: using Super_::storageRequirement;
			public: using Super_::immutablyInvocable;
			public: using Super_::typeInfo;
			public: using Super_::access;
			public: using Super_::value;
			public: using Super_::invokeSelf;

			private: using ConfigAdaptor_ = ::dcool::utility::detail_::OverloadedFunctionConfigAdaptor_<
				Config, typename OtherPrototypesT_::template PushFront<auto(ParameterTs_...) -> ReturnT_>
			>;
			public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
				this->Super_::cloneTo(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template swapWith<strategyC__, engineWillBeSwappedC__>(
					engine_, otherEngine_, static_cast<Super_&>(other_)
				);
			}

			public: template <
				::dcool::core::Index indexC__
			> constexpr auto immutablyInvocable(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				if constexpr (indexC__ > 0) {
					return this->Super_::template immutablyInvocable<indexC__ - 1>(engine_);
				}
				return this->Super_::extendedInformation(engine_).immutablyInvocable_;
			}

			public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation {
				return this->Super_::extendedInformation(engine_).extendedInformation_;
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) const -> ReturnT_ {
				return this->Super_::extendedInformation(engine_).immutableInvoker_(
					engine_, *this, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) -> ReturnT_ {
				return this->Super_::extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}
		};

		template <
			typename ConfigT_, typename PrototypeT_, typename... OtherPrototypeTs_
		> struct OverloadedFunctionChassis_<
			::dcool::core::Types<PrototypeT_, OtherPrototypeTs_...>, ConfigT_
		>: private ::dcool::utility::detail_::OverloadedFunctionChassisBase_<
			PrototypeT_, ::dcool::core::Types<OtherPrototypeTs_...>, ConfigT_
		> {
			private: using Self_ = OverloadedFunctionChassis_<
				::dcool::core::Types<PrototypeT_, OtherPrototypeTs_...>, ConfigT_
			>;
			private: using Super_ = ::dcool::utility::detail_::OverloadedFunctionChassisBase_<
				PrototypeT_, ::dcool::core::Types<OtherPrototypeTs_...>, ConfigT_
			>;
			public: using Prototypes = ::dcool::core::Types<PrototypeT_>;
			public: using Config = ConfigT_;

			public: using typename Super_::Engine;
			public: using typename Super_::ExtendedInformation;
			public: using Super_::exceptionSafetyStrategy;
			public: using Super_::initialize;
			public: using Super_::uninitialize;
			public: using Super_::valid;
			public: using Super_::storageRequirement;
			public: using Super_::immutablyInvocable;
			public: using Super_::typeInfo;
			public: using Super_::extendedInformation;
			public: using Super_::access;
			public: using Super_::value;
			public: using Super_::invokeSelf;

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
				this->Super_::cloneTo(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				this->Super_::template swapWith<strategyC__, engineWillBeSwappedC__>(
					engine_, otherEngine_, static_cast<Super_&>(other_)
				);
			}
		};
	}

	template <typename T_, typename PrototypesT_> concept OverloadedFunctionConfig = ::dcool::core::Complete<
		::dcool::utility::detail_::OverloadedFunctionConfigAdaptor_<T_, PrototypesT_>
	>;

	template <typename ConfigT_, typename PrototypesT_> requires ::dcool::utility::OverloadedFunctionConfig<
		ConfigT_, PrototypesT_
	> using OverloadedFunctionConfigAdaptor = ::dcool::utility::detail_::OverloadedFunctionConfigAdaptor_<ConfigT_, PrototypesT_>;

	template <
		::dcool::core::TypeList PrototypesT_,
		::dcool::utility::OverloadedFunctionConfig<PrototypesT_> ConfigT_ = ::dcool::core::Empty<>
	> using OverloadedFunctionChassis = ::dcool::utility::detail_::OverloadedFunctionChassis_<PrototypesT_, ConfigT_>;

	template <
		::dcool::core::TypeList PrototypesT_,
		::dcool::utility::OverloadedFunctionConfig<PrototypesT_> ConfigT_ = ::dcool::core::Empty<>
	> struct OverloadedFunction {
		private: using Self_ = OverloadedFunction<PrototypesT_, ConfigT_>;
		public: using Prototypes = PrototypesT_;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::OverloadedFunctionChassis<Prototypes, Config>;
		public: using ExtendedInformation = Chassis::ExtendedInformation;
		public: using Engine = Chassis::Engine;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = Chassis::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			Chassis::exceptionSafetyStrategy
		;

			private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr OverloadedFunction() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr OverloadedFunction(Self_ const& other_) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine, *this);
		}

		public: constexpr OverloadedFunction(Self_&& other_) {
			other_.chassis().relocateTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(this->mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr OverloadedFunction(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr OverloadedFunction(
			::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
		) {
			this->chassis().template initialize<ValueT__>(
				this->mutableEngine(), ::dcool::core::inPlace, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: constexpr ~OverloadedFunction() noexcept {
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

		public: template <
			::dcool::core::Index indexC__
		> constexpr auto immutablyInvocable() const noexcept -> ::dcool::core::Boolean {
				return this->chassis().template immutablyInvocable<indexC__>(this->mutableEngine());
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

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_) const {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(ArgumentTs_&&... parameters_) const {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(ArgumentTs_&&... parameters_) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}
	};

	template <typename... PrototypesT_> using DefaultOverloadedFunctionChassis = ::dcool::utility::OverloadedFunctionChassis<
		::dcool::core::Types<PrototypesT_...>
	>;

	template <typename... PrototypesT_> using DefaultOverloadedFunction = ::dcool::utility::OverloadedFunction<
		::dcool::core::Types<PrototypesT_...>
	>;
}

#endif
