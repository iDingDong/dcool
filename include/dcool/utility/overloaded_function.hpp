#ifndef DCOOL_UTILITY_OVERLOADED_FUNCTION_HPP_INCLUDED_
#	define DCOOL_UTILITY_OVERLOADED_FUNCTION_HPP_INCLUDED_ 1

#	include <dcool/utility/any.hpp>
#	include <dcool/utility/function.hpp>

#	include <dcool/core.hpp>

namespace dcool::utility {
	namespace detail_ {
		template <
			::dcool::core::Complete ConfigT_, ::dcool::core::TypeList PrototypesT_
		> struct OverloadedFunctionConfigAdaptor_: public ::dcool::utility::AnyConfigAdaptor<ConfigT_> {
		};

		template <typename PrototypeT_> struct OverloadedFunctionChassisBase_;

		template <
			::dcool::core::Boolean noexceptC_, typename ResultT_, typename... ParameterTs_
		> struct OverloadedFunctionChassisBase_<auto (ParameterTs_...) noexcept(noexceptC_) -> ResultT_> {
			private: using Self_ = OverloadedFunctionChassisBase_<auto (ParameterTs_...) -> ResultT_>;
			private: using Return_ = ResultT_;

			private: template <typename ChassisT__> using ImmutableInvoker_ = auto (*)(
				typename ChassisT__::Engine& engine_, ChassisT__ const& self_, ParameterTs_... parameters_
			) -> Return_;
			private: template <typename ChassisT__> using Invoker_ = auto (*)(
				typename ChassisT__::Engine& engine_, ChassisT__& self_, ParameterTs_... parameters_
			) -> Return_;

			public: template <typename ChassisT__> struct ExtendedInformation_ {
				ImmutableInvoker_<ChassisT__> immutableInvoker_;
				Invoker_<ChassisT__> invoker_;
				::dcool::core::Boolean immutablyInvocable_;

				template <typename ValueT__> constexpr ExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept:
					immutableInvoker_(
						::dcool::utility::detail_::immutablyInvokeFunctionChassis_<
							ChassisT__, ValueT__, noexceptC_, Return_, ParameterTs_...
						>
					),
					invoker_(
						::dcool::utility::detail_::invokeFunctionChassis_<ChassisT__, ValueT__, noexceptC_, Return_, ParameterTs_...>
					),
					immutablyInvocable_(::dcool::utility::detail_::isImmutablyInvocable_<ValueT__, ParameterTs_...>)
				{
				}
			};

			public: template <typename ChassisT__> static constexpr auto invoke_(
				ChassisT__ const& self_, typename ChassisT__::Engine& engine_, ParameterTs_... parameters_
			) noexcept(noexceptC_) -> Return_ {
				return ::dcool::core::get<ExtendedInformation_<ChassisT__>>(self_.baseExtendedInformations_(engine_)).immutableInvoker_(
					engine_, self_, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}

			public: template <typename ChassisT__> static constexpr auto invoke_(
				ChassisT__& self_, typename ChassisT__::Engine& engine_, ParameterTs_... parameters_
			) noexcept(noexceptC_) -> Return_ {
				return ::dcool::core::get<ExtendedInformation_<ChassisT__>>(self_.baseExtendedInformations_(engine_)).invoker_(
					engine_, self_, ::dcool::core::forward<ParameterTs_>(parameters_)...
				);
			}

			public: template <typename ChassisT__> static constexpr auto immutablyInvocable_(
				ChassisT__ const& self_, typename ChassisT__::Engine& engine_
			) noexcept -> ::dcool::core::Boolean {
				return ::dcool::core::get<ExtendedInformation_<ChassisT__>>(
					self_.baseExtendedInformations_(engine_)
				).immutablyInvocable_;
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
	> struct OverloadedFunctionChassis;

	template <
		typename ConfigT_, typename... PrototypeTs_
	> struct OverloadedFunctionChassis<
		::dcool::core::Types<PrototypeTs_...>, ConfigT_
	>: private ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>... {
		private: using Self_ = OverloadedFunctionChassis<::dcool::core::Types<PrototypeTs_...>, ConfigT_>;
		public: using Prototypes = ::dcool::core::Types<PrototypeTs_...>;
		public: using Config = ConfigT_;
		public: template <typename PrototypeT__> friend struct ::dcool::utility::detail_::OverloadedFunctionChassisBase_;

		private: using ConfigAdaptor_ = ::dcool::utility::OverloadedFunctionConfigAdaptor<Config, Prototypes>;
		public: using Engine = ConfigAdaptor_::Engine;
		private: using BaseExtendedInformations_ = ::dcool::core::Tuple<
			typename ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>::template ExtendedInformation_<Self_>...
		>;
		public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: struct UnderlyingExtendedInformation_ {
			BaseExtendedInformations_ baseExtendedInformations_;
			ExtendedInformation userExtendedInformation_;

			template <typename ValueT__> constexpr UnderlyingExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept:
				baseExtendedInformations_(::dcool::core::useRightValue<PrototypeTs_, typed_>...), userExtendedInformation_(typed_)
			{
			}
		};

		private: struct UnderlyingConfig_ {
			using Pool = ConfigAdaptor_::Pool;
			using ExtendedInformation = UnderlyingExtendedInformation_;
			using Engine = Engine;
			static constexpr ::dcool::core::StorageRequirement squeezedTankage = squeezedTankage;
			static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
		};

		private: using Underlying_ = ::dcool::utility::AnyChassis<UnderlyingConfig_>;

		private: Underlying_ m_underlying_;

		public: constexpr void initialize(Engine& engine_) noexcept {
			this->m_underlying_.initialize(engine_);
		}

		public: template <typename ValueT__> constexpr void initialize(Engine& engine_, ValueT__&& value_) noexcept(
			noexcept(
				noexcept(
					this->template initialize<::dcool::core::DecayedType<ValueT__>>(
						engine_, ::dcool::core::inPlace, ::dcool::core::forward<ValueT__>(value_)
					)
				)
			)
		) {
			this->initialize<::dcool::core::DecayedType<ValueT__>>(
				engine_, ::dcool::core::inPlace, ::dcool::core::forward<ValueT__>(value_)
			);
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
			Engine& engine_, ::dcool::core::InPlaceTag tag_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				::dcool::core::declval<Self_&>().m_underlying_.template initialize<ValueT__>(
					engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				)
			)
		) {
			this->m_underlying_.template initialize<ValueT__>(engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_underlying_.uninitialize(engine_);
		}

		public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
			this->m_underlying_.cloneTo(engine_, otherEngine_, other_.underlying_);
		}

		public: template <
			::dcool::core::Boolean engineWillBeUniformedC__ = false
		> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			this->m_underlying_.template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, other_.m_underlying_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			this->m_underlying_.template swapWith<strategyC__, engineWillBeSwappedC__>(engine_, otherEngine_, other_.m_underlying_);
		}

		public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_(engine_).valid(engine_);
		}

		public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
			return this->m_underlying_(engine_).storageRequirement(engine_);
		}

		public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
			return this->m_underlying_(engine_).typeInfo(engine_);
		}

		private: constexpr auto baseExtendedInformations_(Engine& engine_) const noexcept -> BaseExtendedInformations_ const& {
			return this->m_underlying_.extendedInformation(engine_).baseExtendedInformations_;
		}

		public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation const& {
			return this->m_underlying_.extendedInformation(engine_).userExtendedInformation_;
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
			return this->m_underlying_.template access<ValueT__>(engine_);
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
			return this->m_underlying_.template access<ValueT__>(engine_);
		}

		private: using ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>::invoke_...;

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		) const noexcept(
			noexcept(
				invoke_(::dcool::core::declval<Self_ const&>(), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...)
			)
		) {
			return invoke_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				invoke_(::dcool::core::declval<Self_&>(), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...)
			)
		) {
			return invoke_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::Index indexC__
		> constexpr auto immutablyInvocable(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return ::dcool::utility::detail_::OverloadedFunctionChassisBase_<
				::dcool::core::VariadicElementType<indexC__, PrototypeTs_...>
			>::immutablyInvocable_(*this, engine_);
		}
	};

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

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_) const noexcept(
			noexcept(
				::dcool::core::declval<Self_ const&>().chassis().invokeSelf(
					this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
				)
			)
		) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_) noexcept(
			noexcept(
				::dcool::core::declval<Self_&>().chassis().invokeSelf(
					this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
				)
			)
		) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(
			ArgumentTs_&&... parameters_
		) const noexcept(
			noexcept(
				::dcool::core::declval<Self_ const&>().invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...)
			)
		) {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(ArgumentTs_&&... parameters_) noexcept(
			noexcept(
				::dcool::core::declval<Self_&>().invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...)
			)
		) {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}
	};

	template <typename... PrototypeTs_> using DefaultOverloadedFunctionChassis = ::dcool::utility::OverloadedFunctionChassis<
		::dcool::core::Types<PrototypeTs_...>
	>;

	template <typename... PrototypeTs_> using DefaultOverloadedFunction = ::dcool::utility::OverloadedFunction<
		::dcool::core::Types<PrototypeTs_...>
	>;
}

#endif
