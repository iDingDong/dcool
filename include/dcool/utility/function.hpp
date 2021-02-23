#ifndef DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_
#	define DCOOL_UTILITY_FUNCTION_HPP_INCLUDED_

#	include <dcool/utility/any.hpp>

#	include <dcool/core.hpp>

#	include <functional>

#	define DCOOL_UTILITY_FUNCTION_BASIC_OPERATIONS \
	DCOOL_UTILITY_ANY_BASIC_OPERATIONS, \
	dcoolGetImmutablyInvocability_, \
	dcoolInvoke_, \
	dcoolInvokeMutable_

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
			public: using ArgumentPack = ::dcool::core::Tuple<ParameterTs_...>;
			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using DynamicHandle = ::dcool::resource::UnifiedHandleType<Pool>;
			public: using Opcode = ::dcool::utility::detail_::ExtractedOpcodeType_<
				Config, ::dcool::utility::detail_::DefaultFunctionOpcode_
			>;
			public: using ExtendedOpterationExecutor = ::dcool::utility::detail_::ExtractedOperationExecutorType_<
				Config, ::dcool::core::ComparableNoOp
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
				public: [[no_unique_address]] ExtendedOpterationExecutor partExetendedOperationExecutor;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}

				public: constexpr auto exetendedOperationExecutor() noexcept -> ExtendedOpterationExecutor& {
					return this->partExetendedOperationExecutor;
				}

				public: friend auto operator ==(Self_ const&, Self_ const&) -> ::dcool::core::Boolean = default;
				public: friend auto operator !=(Self_ const&, Self_ const&) -> ::dcool::core::Boolean = default;
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::pool'"
			);

			public: using ParameterToGetImmutablyInvocability = ::dcool::core::Boolean;

			public: struct ParameterToInvoke {
				struct {
					Engine* const engine;
					::dcool::utility::detail_::FunctionChassis_<Prototype, Config> const* const self;
					ArgumentPack argumentPack;
				} input;
				[[no_unique_address]] ::dcool::core::StorageFor<Return> output;
			};

			public: struct ParameterToInvokeMutable {
				struct {
					Engine* const engine;
					::dcool::utility::detail_::FunctionChassis_<Prototype, Config>* const self;
					ArgumentPack argumentPack;
				} input;
				[[no_unique_address]] ::dcool::core::StorageFor<Return> output;
			};

			public: struct ParameterForExtendedOperation {
				Engine* const engine;
				void* customizedParameter;
			};

			private: struct AnyExtendedOpterationExecutor_ {
				constexpr void operator()(::dcool::core::TypedTag<void> typedTag_, Opcode opcode_, void* parameter_) {
					switch (opcode_) {
						case Opcode::dcoolGetImmutablyInvocability_: {
							*static_cast<ParameterToGetImmutablyInvocability*>(parameter_) = false;
							break;
						}
						case Opcode::dcoolInvoke_:
						case Opcode::dcoolInvokeMutable_: {
							throw ::dcool::utility::BadFunctionCall();
							break;
						}
						default: {
							auto actualParameter_ = static_cast<ParameterForExtendedOperation*>(parameter_);
							actualParameter_->engine->exetendedOperationExecutor()(
								typedTag_, opcode_, actualParameter_->customizedParameter
							);
							break;
						}
					}
				}

				template <::dcool::core::NonVoid ValueT__> constexpr void operator()(
					::dcool::core::TypedTag<ValueT__> typedTag_, Opcode opcode_, void* parameter_
				) {
					switch (opcode_) {
						case Opcode::dcoolGetImmutablyInvocability_: {
							*static_cast<ParameterToGetImmutablyInvocability*>(parameter_) =
								::dcool::core::Invocable<ValueT__ const&, ParameterTs_...>
							;
							break;
						}
						case Opcode::dcoolInvoke_: {
							if constexpr (::dcool::core::Invocable<ValueT__ const&, ParameterTs_...>) {
								auto actualParameter_ = static_cast<ParameterToInvoke*>(parameter_);
								if constexpr (::dcool::core::NonVoid<Return>) {
									new (reinterpret_cast<Return*>(::dcool::core::addressOf(actualParameter_->output))) Return(
										::dcool::core::apply(
											actualParameter_->input.self->template access<ValueT__>(*(actualParameter_->input.engine)),
											::dcool::core::move(actualParameter_->input.argumentPack)
										)
									);
								} else {
									::dcool::core::apply(
										actualParameter_->input.self->template access<ValueT__>(*(actualParameter_->input.engine)),
										actualParameter_->input.argumentPack
									);
								}
							} else {
								throw ::dcool::utility::BadFunctionCall();
							}
							break;
						}
						case Opcode::dcoolInvokeMutable_: {
							auto actualParameter_ = static_cast<ParameterToInvokeMutable*>(parameter_);
							if constexpr (::dcool::core::NonVoid<Return>) {
								new (reinterpret_cast<Return*>(::dcool::core::addressOf(actualParameter_->output))) Return(
									::dcool::core::apply(
										actualParameter_->input.self->template access<ValueT__>(*(actualParameter_->input.engine)),
										::dcool::core::move(actualParameter_->input.argumentPack)
									)
								);
							} else {
								::dcool::core::apply(
									actualParameter_->input.self->template access<ValueT__>(*(actualParameter_->input.engine)),
									actualParameter_->input.argumentPack
								);
							}
							break;
						}
						default: {
							auto actualParameter_ = static_cast<ParameterForExtendedOperation*>(parameter_);
							actualParameter_->engine->exetendedOperationExecutor()(
								typedTag_, opcode_, actualParameter_->customizedParameter
							);
							break;
						}
					}
				}
			};

			private: struct AnyEngine_ {
				private: using Self_ = AnyEngine_;

				public: Engine* underlying;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->underlying->pool();
				}

				public: constexpr auto extendedOperationExecutor() noexcept -> AnyExtendedOpterationExecutor_& {
					struct UniqueType_ {
					};
					return ::dcool::core::instance<AnyExtendedOpterationExecutor_, UniqueType_>;
				}

				public: friend constexpr auto operator ==(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Boolean {
					return (*(left_.underlying)) == (*(right_.underlying));
				}

				public: friend auto operator !=(Self_ const&, Self_ const&) -> ::dcool::core::Boolean = default;
			};

			public: struct AnyConfig {
				using Pool = Pool;
				using Opcode = Opcode;
				using ExtendedOpterationExecutor = AnyExtendedOpterationExecutor_;
				using Engine = AnyEngine_;
				static constexpr ::dcool::core::StorageRequirement squeezedTankage = squeezedTankage;
				static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
			};
		};

		template <
			typename ConfigT_, typename ReturnT_, typename... ParameterTs_
		> struct FunctionChassis_<ReturnT_(ParameterTs_...), ConfigT_>: private ::dcool::utility::AnyChassis<
			typename ::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, ReturnT_(ParameterTs_...)>::AnyConfig
		> {
			private: using Self_ = FunctionChassis_<ReturnT_(ParameterTs_...), ConfigT_>;
			private: using Super_ = ::dcool::utility::AnyChassis<
				typename ::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, ReturnT_(ParameterTs_...)>::AnyConfig
			>;
			public: using Return = ReturnT_;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::utility::detail_::FunctionConfigAdaptor_<ConfigT_, ReturnT_(ParameterTs_...)>;
			public: using Prototype = ConfigAdaptor_::Prototype;
			public: using ArgumentPack = ConfigAdaptor_::ArgumentPack;
			public: using Opcode = ConfigAdaptor_::Opcode;
			public: using Engine = ConfigAdaptor_::Engine;
			private: using AnyEngine_ = Super_::Engine;
			private: using ParameterToGetImmutablyInvocability_ = ConfigAdaptor_::ParameterToGetImmutablyInvocability;
			private: using ParameterToInvoke_ = ConfigAdaptor_::ParameterToInvoke;
			private: using ParameterToInvokeMutable_ = ConfigAdaptor_::ParameterToInvokeMutable;
			private: using ParameterForExtendedOperation_ = ConfigAdaptor_::ParameterForExtendedOperation;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			public: using Super_::storage;
			public: using Super_::access;
			public: using Super_::value;

			public: template <typename... ArgumentTs__> constexpr void initialize(
				Engine& engine_, ArgumentTs__&&... parameters_
			) noexcept(
				noexcept(
					::dcool::core::declval<Self_&>().Super_::initialize(
						::dcool::core::declval<AnyEngine_&>(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
					)
				)
			) {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				this->Super_::initialize(anyEngine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			}

			public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
				Engine& engine_, ::dcool::core::InPlaceTag inPlace_, ArgumentTs__&&... parameters_
			) noexcept(
				noexcept(
					::dcool::core::declval<Self_&>().Super_::template initialize<ValueT__>(
						::dcool::core::declval<AnyEngine_&>(), inPlace_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
					)
				)
			) {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				this->Super_::template initialize<ValueT__>(anyEngine_, inPlace_, ::dcool::core::forward<ParameterTs_>(parameters_)...);
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				this->Super_::uninitialize(anyEngine_);
			}

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				AnyEngine_ otherAnyEngine_ = { .underlying = ::dcool::core::addressOf(otherEngine_) };
				this->Super_::cloneTo(anyEngine_, otherAnyEngine_, static_cast<Super_&>(other_));
			}

			public: template <
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				AnyEngine_ otherAnyEngine_ = { .underlying = ::dcool::core::addressOf(otherEngine_) };
				this->Super_::template relocateTo<engineWillBeSwappedC__>(anyEngine_, otherAnyEngine_, static_cast<Super_&>(other_));
			}

			public: public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				AnyEngine_ otherAnyEngine_ = { .underlying = ::dcool::core::addressOf(otherEngine_) };
				this->Super_::template swapWith<strategyC__, engineWillBeSwappedC__>(
					anyEngine_, otherAnyEngine_, static_cast<Super_&>(other_)
				);
			}

			public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::valid(anyEngine_);
			}

			public: constexpr auto immutablyInvocable(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				ParameterToGetImmutablyInvocability_ parameter_;
				static_assert(::dcool::core::isSame<ParameterToGetImmutablyInvocability_, ::dcool::core::Boolean>);
				this->Super_::executeCustomizedOperation(
					anyEngine_, Opcode::dcoolGetImmutablyInvocability_, ::dcool::core::addressOf(parameter_)
				);
				return parameter_;
			}

			public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::typeInfo(anyEngine_);
			}

			public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::storageRequirement(anyEngine_);
			}

			public: constexpr auto storage(Engine& engine_) const noexcept -> void const* {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::storage(anyEngine_);
			}

			public: constexpr auto storage(Engine& engine_) noexcept -> void* {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::storage(anyEngine_);
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::template access<ValueT__>(anyEngine_);
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::template access<ValueT__>(anyEngine_);
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) const -> ValueT__ const& {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::template value<ValueT__>(anyEngine_);
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) -> ValueT__& {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				return this->Super_::template value<ValueT__>(anyEngine_);
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) const -> Return {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				ParameterToInvoke_ parameter_ = {
					.input = {
						.engine = ::dcool::core::addressOf(engine_),
						.self = this,
						.argumentPack = ArgumentPack(::dcool::core::forward<ParameterTs_>(parameters_)...)
					}
				};
				this->Super_::executeCustomizedOperation(anyEngine_, Opcode::dcoolInvoke_, ::dcool::core::addressOf(parameter_));
				if constexpr (::dcool::core::NonVoid<Return>) {
					return ::dcool::core::move(
						*::dcool::core::launder(reinterpret_cast<Return*>(::dcool::core::addressOf(parameter_.output)))
					);
				}
			}

			public: constexpr auto invokeSelf(Engine& engine_, ParameterTs_... parameters_) -> Return {
				AnyEngine_ anyEngine_ = { .underlying = ::dcool::core::addressOf(engine_) };
				ParameterToInvokeMutable_ parameter_ = {
					.input = {
						.engine = ::dcool::core::addressOf(engine_),
						.self = this,
						.argumentPack = ArgumentPack(::dcool::core::forward<ParameterTs_>(parameters_)...)
					}
				};
				this->Super_::executeCustomizedOperation(anyEngine_, Opcode::dcoolInvokeMutable_, ::dcool::core::addressOf(parameter_));
				if constexpr (::dcool::core::NonVoid<Return>) {
					return ::dcool::core::move(
						*::dcool::core::launder(reinterpret_cast<Return*>(::dcool::core::addressOf(parameter_.output)))
					);
				}
			}

			protected: constexpr void executeCustomizedOperation(Engine& engine_, Opcode opcode_, void* customizedParameter_) const {
				ParameterForExtendedOperation_ parameter_ = {
					.engine_ = ::dcool::core::addressOf(engine_), .customizedParameter_ = customizedParameter_
				};
				this->m_executor_(opcode_, ::dcool::core::addressOf(parameter_));
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
		public: using Opcode = ConfigAdaptor_::Opcode;
		public: using Engine = ConfigAdaptor_::Engine;
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

		public: constexpr auto immutablyInvocable() const noexcept -> ::dcool::core::Boolean {
				return this->chassis().immutablyInvocable(this->mutableEngine());
			}

		public: constexpr auto typeInfo() const noexcept -> ::dcool::core::TypeInfo const& {
			return this->chassis().typeInfo(this->mutableEngine());
		}

		public: constexpr auto storageRequirement() const noexcept {
			return this->chassis().storageRequirement(this->mutableEngine());
		}

		public: constexpr auto storage() const noexcept -> void const* {
			return this->chassis().storage(this->mutableEngine());
		}

		public: constexpr auto storage() noexcept -> void* {
			return this->chassis().storage(this->mutableEngine());
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

		protected: constexpr void executeCustomizedOperation(Opcode opcode_, void* customizedParameter_) const {
			this->chassis().executeCustomizedOperation(this->mutableEngine(), opcode_, customizedParameter_);
		}
	};
}

#endif
