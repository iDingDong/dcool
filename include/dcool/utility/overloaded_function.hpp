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

		template <::dcool::utility::FunctionPrototype PrototypeT_> struct OverloadedFunctionChassisBase_;

#	define DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(qualifiler_, referenceCategory_) \
		template < \
			::dcool::core::Boolean noThrowC_, typename ResultT_, typename... ParameterTs_ \
		> struct OverloadedFunctionChassisBase_< \
			auto(ParameterTs_...) qualifiler_ referenceCategory_ noexcept(noThrowC_) -> ResultT_ \
		> { \
			private: using Self_ = OverloadedFunctionChassisBase_< \
				auto(ParameterTs_...) qualifiler_ referenceCategory_ noexcept(noThrowC_) -> ResultT_ \
			>; \
			 \
			private: using Prototype_ = auto(ParameterTs_...) qualifiler_ referenceCategory_ noexcept(noThrowC_) -> ResultT_; \
			private: using PrototypeInfo_ = ::dcool::utility::detail_::FunctionPrototypeInfo_<Prototype_>; \
			private: using Result_ = PrototypeInfo_::Result_; \
			private: static constexpr ::dcool::core::Boolean noThrow_ = PrototypeInfo_::noThrow_; \
			 \
			public: template <typename ChassisT__> struct ExtendedInformation_ { \
				typename PrototypeInfo_::Invoker_<ChassisT__, typename ChassisT__::Engine> invoker_; \
				 \
				template <typename ValueT__> constexpr ExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept: \
					invoker_(PrototypeInfo_::template invoker_<ChassisT__, typename ChassisT__::Engine, ValueT__>) \
				{ \
				} \
			}; \
			 \
			public: template <typename ChassisT__> static constexpr auto invokeChassis_( \
				ChassisT__ qualifiler_& self_, typename ChassisT__::Engine& engine_, ParameterTs_... parameters_ \
			) noexcept(noThrow_) -> Result_ requires ( \
				PrototypeInfo_::lvalueReferenceAllowed_ \
			) { \
				return ::dcool::core::get<ExtendedInformation_<ChassisT__>>(self_.baseExtendedInformations_(engine_)).invoker_( \
					engine_, self_, ::dcool::core::forward<ParameterTs_>(parameters_)... \
				); \
			} \
			 \
			public: template <typename ChassisT__> static constexpr auto invokeChassis_( \
				ChassisT__ qualifiler_&& self_, typename ChassisT__::Engine& engine_, ParameterTs_... parameters_ \
			) noexcept(noThrow_) -> Result_ requires ( \
				PrototypeInfo_::rvalueReferenceAllowed_ \
			) { \
				return ::dcool::core::get<ExtendedInformation_<ChassisT__>>(self_.baseExtendedInformations_(engine_)).invoker_( \
					engine_, self_, ::dcool::core::forward<ParameterTs_>(parameters_)... \
				); \
			} \
		}

		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(, );
		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(, &);
		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(, &&);
		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(const, );
		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(const, &);
		DCOOL_UTILITY_DEFINE_OVERLOADED_FUNCTION_CHASSIS_BASE_(const, &&);
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
		typename ConfigT_, ::dcool::utility::FunctionPrototype... PrototypeTs_
	> struct OverloadedFunctionChassis<
		::dcool::core::Types<PrototypeTs_...>, ConfigT_
	>: private ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>... {
		private: using Self_ = OverloadedFunctionChassis<::dcool::core::Types<PrototypeTs_...>, ConfigT_>;
		public: using Prototypes = ::dcool::core::Types<PrototypeTs_...>;
		public: using Config = ConfigT_;
		public: template <
			::dcool::utility::FunctionPrototype PrototypeT__
		> friend struct ::dcool::utility::detail_::OverloadedFunctionChassisBase_;

		private: using ConfigAdaptor_ = ::dcool::utility::OverloadedFunctionConfigAdaptor<Config, Prototypes>;
		public: using Engine = ConfigAdaptor_::Engine;
		private: using BaseExtendedInformations_ = ::dcool::core::Tuple<
			typename ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>::template ExtendedInformation_<Self_>...
		>;
		public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
		public: static constexpr ::dcool::core::Boolean copyable = ConfigAdaptor_::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = ConfigAdaptor_::moveable;
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
			static constexpr ::dcool::core::Boolean copyable = copyable;
			static constexpr ::dcool::core::Boolean moveable = moveable;
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
			return this->m_underlying_.valid(engine_);
		}

		public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
			return this->m_underlying_.storageRequirement(engine_);
		}

		public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
			return this->m_underlying_.typeInfo(engine_);
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

		private: using ::dcool::utility::detail_::OverloadedFunctionChassisBase_<PrototypeTs_>::invokeChassis_...;

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		) const& noexcept(noexcept(invokeChassis_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...))) {
			return invokeChassis_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		) const&& noexcept(
			noexcept(
				invokeChassis_(::dcool::core::move(*this), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...)
			)
		) {
			return invokeChassis_(::dcool::core::move(*this), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		)& noexcept(noexcept(invokeChassis_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...))) {
			return invokeChassis_(*this, engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr decltype(auto) invokeSelf(
			Engine& engine_, ArgumentTs__&&... parameters_
		)&& noexcept(
			noexcept(
				invokeChassis_(::dcool::core::move(*this), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...)
			)
		) {
			return invokeChassis_(::dcool::core::move(*this), engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
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
		public: static constexpr ::dcool::core::Boolean copyable = Chassis::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = Chassis::moveable;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = Chassis::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			Chassis::exceptionSafetyStrategy
		;

			private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr OverloadedFunction() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr OverloadedFunction(Self_ const& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
		}

		public: constexpr OverloadedFunction(Self_&& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().template relocateTo<true>(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(other_.mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr OverloadedFunction(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr explicit OverloadedFunction(
			::dcool::core::TypedInPlaceTag<ValueT__>, ArgumentTs__&&... parameters_
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

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(
			ArgumentTs_&&... parameters_
		) const& noexcept(
			noexcept(
				this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...)
			)
		) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(
			ArgumentTs_&&... parameters_
		) const&& noexcept(
			noexcept(
				::dcool::core::move(*this).chassis().invokeSelf(
					this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
				)
			)
		) {
			return ::dcool::core::move(*this).chassis().invokeSelf(
				this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_)& noexcept(
			noexcept(
				this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...)
			)
		) {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) invokeSelf(ArgumentTs_&&... parameters_)&& noexcept(
			noexcept(
				::dcool::core::move(*this).chassis().invokeSelf(
					this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
				)
			)
		) {
			return ::dcool::core::move(*this).chassis().invokeSelf(
				this->mutableEngine(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(
			ArgumentTs_&&... parameters_
		) const& noexcept(
			noexcept(this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...))
		) {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(
			ArgumentTs_&&... parameters_
		) const&& noexcept(
			noexcept(::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...))
		) {
			return ::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(ArgumentTs_&&... parameters_)& noexcept(
			noexcept(this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...))
		) {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}

		public: template <typename... ArgumentTs_> constexpr decltype(auto) operator ()(ArgumentTs_&&... parameters_)&& noexcept(
			noexcept(::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...))
		) {
			return ::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs_>(parameters_)...);
		}
	};

	template <
		::dcool::utility::FunctionPrototype... PrototypeTs_
	> using DefaultOverloadedFunctionChassis = ::dcool::utility::OverloadedFunctionChassis<
		::dcool::core::Types<PrototypeTs_...>
	>;

	template <
		::dcool::utility::FunctionPrototype... PrototypeTs_
	> using DefaultOverloadedFunction = ::dcool::utility::OverloadedFunction<
		::dcool::core::Types<PrototypeTs_...>
	>;
}

#endif
