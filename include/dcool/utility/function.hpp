#ifndef DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_
#	define DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_

#	include <dcool/utility/any.hpp>

#	include <dcool/core.hpp>

#	include <functional>

namespace dcool::utility {
	using BadFunctionCall = ::std::bad_function_call;

	namespace detail_ {
		template <typename PrototypeT_> struct FunctionPrototypeInfo_;

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...) noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean constQualified_ = false;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						functionChassis_.template access<CallableT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...)& noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = false;
			static constexpr ::dcool::core::Boolean constQualified_ = false;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						functionChassis_.template access<CallableT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...)&& noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = false;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean constQualified_ = false;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						::dcool::core::move(functionChassis_.template access<CallableT__>(engine_)),
						::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...) const noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean constQualified_ = true;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						functionChassis_.template access<CallableT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...) const& noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = false;
			static constexpr ::dcool::core::Boolean constQualified_ = true;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						functionChassis_.template access<CallableT__>(engine_), ::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};

		template <
			typename ResultT_, ::dcool::core::Boolean noThrowC_, typename... ParameterTs_
		> struct FunctionPrototypeInfo_<auto(ParameterTs_...) const&& noexcept(noThrowC_) -> ResultT_> {
			using Result_ = ResultT_;
			static constexpr ::dcool::core::Boolean noThrow_ = noThrowC_;
			static constexpr ::dcool::core::Boolean lvalueReferenceAllowed_ = false;
			static constexpr ::dcool::core::Boolean rvalueReferenceAllowed_ = true;
			static constexpr ::dcool::core::Boolean constQualified_ = true;

			template <
				typename FunctionChassisT__,
				typename EngineT__,
				typename CallableT__
			> static constexpr auto invokeFunctionChassis_(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_ {
				if constexpr (::dcool::core::NonVoid<CallableT__>) {
					return ::dcool::core::invoke(
						::dcool::core::move(functionChassis_.template access<CallableT__>(engine_)),
						::dcool::core::forward<ParameterTs_>(parameters_)...
					);
				}
				if constexpr (!noThrow_) {
					throw ::dcool::utility::BadFunctionCall();
				}
				::dcool::core::terminate();
			}

			template <typename FunctionChassisT__, typename EngineT__> using Invoker_ = auto(*)(
				EngineT__& engine_, FunctionChassisT__ const& functionChassis_, ParameterTs_... parameters_
			) noexcept(noThrow_) -> Result_;
			template <
				typename FunctionChassisT__, typename EngineT__, typename CallableT__
			> static constexpr Invoker_<FunctionChassisT__, EngineT__> invoker_ = &(
				invokeFunctionChassis_<FunctionChassisT__, EngineT__, CallableT__>
			);
		};
	}

	template <typename T_> concept FunctionPrototype = ::dcool::core::Complete<
		::dcool::utility::detail_::FunctionPrototypeInfo_<T_>
	>;

	namespace detail_ {
		template <
			::dcool::core::Complete ConfigT_, ::dcool::utility::FunctionPrototype PrototypeT_
		> struct FunctionConfigAdaptor_: public ::dcool::utility::AnyConfigAdaptor<ConfigT_> {
		};

		template <::dcool::utility::FunctionPrototype PrototypeT_, typename ConfigT_> struct FunctionChassis_ {
			public: using Prototype = PrototypeT_;
			public: using Config = ConfigT_;

			private: using PrototypeInfo_ = ::dcool::utility::detail_::FunctionPrototypeInfo_<Prototype>;
			private: using ConfigAdaptor_ = ::dcool::utility::detail_::FunctionConfigAdaptor_<Config, Prototype>;
			public: using Result = PrototypeInfo_::Result_;
			public: using Pool = ConfigAdaptor_::Pool;
			public: using Engine = ConfigAdaptor_::Engine;
			public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
			private: using Invoker_ = typename PrototypeInfo_::Invoker_<FunctionChassis_, Engine>;
			public: static constexpr ::dcool::core::Boolean copyable = ConfigAdaptor_::copyable;
			public: static constexpr ::dcool::core::Boolean moveable = ConfigAdaptor_::moveable;
			public: static constexpr ::dcool::core::Boolean noexceptInvocable = PrototypeInfo_::noThrow_;
			public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			private: struct AnyExtendedInformation_ {
				Invoker_ invoker_;
				ExtendedInformation extendedInformation_;

				template <typename ValueT__> constexpr AnyExtendedInformation_(::dcool::core::TypedTag<ValueT__> typed_) noexcept:
					invoker_(PrototypeInfo_::template invoker_<FunctionChassis_, Engine, ValueT__>),
					extendedInformation_(typed_)
				{
				}
			};

			private: struct AnyConfig_ {
				using Pool = FunctionChassis_::Pool;
				using ExtendedInformation = AnyExtendedInformation_;
				using Engine = FunctionChassis_::Engine;
				static constexpr ::dcool::core::Boolean copyable = copyable;
				static constexpr ::dcool::core::Boolean moveable = moveable;
				static constexpr ::dcool::core::StorageRequirement squeezedTankage = squeezedTankage;
				static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
			};

			private: using Underlying_ = ::dcool::utility::AnyChassis<AnyConfig_>;

			private: Underlying_ m_underlying_;

			public: void initialize(Engine& engine_) noexcept {
				this->m_underlying_.initialize(engine_);
			}

			public: template <typename ValueT__> constexpr void initialize(Engine& engine_, ValueT__&& value_) noexcept(
				noexcept(::dcool::core::declval<Underlying_&>().initialize(engine_, ::dcool::core::forward<ValueT__>(value_)))
			) {
				this->m_underlying_.initialize(engine_, ::dcool::core::forward<ValueT__>(value_));
			}

			public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
				Engine& engine_, ::dcool::core::InPlaceTag tag_, ArgumentTs__&&... parameters_
			) noexcept(
				noexcept(
					::dcool::core::declval<Underlying_&>().template initialize<ValueT__>(
						engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
					)
				)
			) {
				this->m_underlying_.template initialize<ValueT__>(engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				this->m_underlying_.uninitialize(engine_);
			}

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, FunctionChassis_& other_) const {
				this->m_underlying_.cloneTo(engine_, otherEngine_, other_.m_underlying_);
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, FunctionChassis_& other_) {
				this->m_underlying_.template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, other_.m_underlying_);
			}

			public: public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, FunctionChassis_& other_) {
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

			public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
				Engine& engine_, ArgumentTs__&&... parameters_
			) const& noexcept(noexceptInvocable) -> Result requires (
				PrototypeInfo_::lvalueReferenceAllowed_ && PrototypeInfo_::constQualified_
			) {
				return this->m_underlying_.extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}

			public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
				Engine& engine_, ArgumentTs__&&... parameters_
			) const&& noexcept(noexceptInvocable) -> Result requires (
				PrototypeInfo_::rvalueReferenceAllowed_ && PrototypeInfo_::constQualified_
			) {
				return this->m_underlying_.extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}

			public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
				Engine& engine_, ArgumentTs__&&... parameters_
			)& noexcept(noexceptInvocable) -> Result requires (PrototypeInfo_::lvalueReferenceAllowed_) {
				return this->m_underlying_.extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}

			public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
				Engine& engine_, ArgumentTs__&&... parameters_
			)&& noexcept(noexceptInvocable) -> Result requires (PrototypeInfo_::rvalueReferenceAllowed_) {
				return this->m_underlying_.extendedInformation(engine_).invoker_(
					engine_, *this, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}
		};
	}

	template <typename T_, typename PrototypeT_> concept FunctionConfig = ::dcool::core::Complete<
		::dcool::utility::detail_::FunctionConfigAdaptor_<T_, PrototypeT_>
	>;

	template <typename ConfigT_, typename PrototypeT_> requires ::dcool::utility::FunctionConfig<
		ConfigT_, PrototypeT_
	> using FunctionConfigAdaptor = ::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, PrototypeT_>;

	template <
		typename PrototypeT_, ::dcool::utility::FunctionConfig<PrototypeT_> ConfigT_ = ::dcool::core::Empty<>
	> using FunctionChassis = ::dcool::utility::detail_::FunctionChassis_<PrototypeT_, ConfigT_>;

	template <
		::dcool::utility::FunctionPrototype PrototypeT_,
		::dcool::utility::FunctionConfig<PrototypeT_> ConfigT_ = ::dcool::core::Empty<>
	> struct Function {
		private: using Self_ = Function<PrototypeT_, ConfigT_>;
		public: using Prototype = PrototypeT_;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::FunctionChassis<Prototype, Config>;
		public: using Result = Chassis::Result;
		public: using Engine = Chassis::Engine;
		public: using ExtendedInformation = Chassis::ExtendedInformation;
		public: static constexpr ::dcool::core::Boolean copyable = Chassis::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = Chassis::moveable;
		public: static constexpr ::dcool::core::Boolean noexceptInvocable = Chassis::noexceptInvocable;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = Chassis::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			Chassis::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr Function() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr Function(Self_ const& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
		}

		public: constexpr Function(Self_&& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().template relocateTo<true>(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(other_.mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr Function(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr explicit Function(
			::dcool::core::TypedInPlaceTag<ValueT__>, ArgumentTs__&&... parameters_
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

		public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
			ArgumentTs__&&... parameters_
		) const& noexcept(noexceptInvocable) -> Result {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
			ArgumentTs__&&... parameters_
		) const&& noexcept(noexceptInvocable) -> Result {
			return ::dcool::core::move(this->chassis()).invokeSelf(
				this->mutableEngine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
			ArgumentTs__&&... parameters_
		)& noexcept(noexceptInvocable) -> Result {
			return this->chassis().invokeSelf(this->mutableEngine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto invokeSelf(
			ArgumentTs__&&... parameters_
		)&& noexcept(noexceptInvocable) -> Result {
			return ::dcool::core::move(this->chassis()).invokeSelf(
				this->mutableEngine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto operator ()(
			ArgumentTs__&&... parameters_
		) const& noexcept(noexceptInvocable) -> Result {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto operator ()(
			ArgumentTs__&&... parameters_
		) const&& noexcept(noexceptInvocable) -> Result {
			return ::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto operator ()(
			ArgumentTs__&&... parameters_
		)& noexcept(noexceptInvocable) -> Result {
			return this->invokeSelf(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto operator ()(
			ArgumentTs__&&... parameters_
		)&& noexcept(noexceptInvocable) -> Result {
			return ::dcool::core::move(*this).invokeSelf(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}
	};

	template <typename... CallableTs_> struct Overloaded: CallableTs_... {
		using CallableTs_::operator()...;
	};
}

#endif
