#ifndef DCOOL_UTILITY_ANY_HPP_INCLUDED_
#	define DCOOL_UTILITY_ANY_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <any>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::utility::detail_, HasTypeOpcode_, ExtractedOpcodeType_, Opcode
);
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::utility::detail_, HasTypeExtendedOperationExecutor_, ExtractedOperationExecutorType_, ExtendedOpterationExecutor
);
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::utility::detail_, HasValueSqueezedTankage_, extractedSqueezedTankage_, squeezedTankage
);

#	define DCOOL_UTILITY_ANY_BASIC_OPERATIONS \
	dcoolGetTypeInfo_, \
	dcoolGetStorageRequirement_, \
	dcoolGetRawPointer_, \
	dcoolGetRawPointerToMutable_, \
	dcoolCloneTo_, \
	dcoolRelocateTo_, \
	dcoolDestruct_

namespace dcool::utility {
	using BadAnyCast = ::std::bad_any_cast;

	namespace detail_ {
		enum class DefaultAnyOpcode_ {
			DCOOL_UTILITY_ANY_BASIC_OPERATIONS
		};

		template <typename ConfigT_> class AnyConfigAdaptor_ {
			private: using Self_ = AnyConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using DynamicHandle = ::dcool::resource::UnifiedHandleType<Pool>;
			public: using Opcode = ::dcool::utility::detail_::ExtractedOpcodeType_<
				Config, ::dcool::utility::detail_::DefaultAnyOpcode_
			>;
			public: using OperationExecutor = void (*)(Opcode, void*);
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

			static_assert(sizeof(DynamicHandle) > 0);
		};
	}

	template <typename T_> concept AnyConfig = requires {
		typename ::dcool::utility::detail_::AnyConfigAdaptor_<T_>;
	};

	template <typename ConfigT_> using AnyConfigAdaptor = ::dcool::utility::detail_::AnyConfigAdaptor_<ConfigT_>;

	template <::dcool::utility::AnyConfig ConfigT_ = ::dcool::core::Empty<>> struct AnyChassis {
		private: using Self_ = AnyChassis<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::utility::AnyConfigAdaptor<Config>;
		public: using DynamicHandle = ConfigAdaptor_::DynamicHandle;
		public: using Opcode = ConfigAdaptor_::Opcode;
		public: using OperationExecutor = ConfigAdaptor_::OperationExecutor;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: using Storage_ = ::dcool::resource::StorageSqueezer<squeezedTankage, DynamicHandle>;

		private: struct GeneralParameter_ {
			Engine* const engine_;
			Self_ const* const self_;
		};

		private: struct GeneralMutableParameter_ {
			Engine* const engine_;
			Self_* const self_;
		};

		private: using ParameterToGetTypeInfo_ = ::dcool::core::TypeInfo const*;
		private: using ParameterToGetStorageRequirement_ = ::dcool::core::StorageRequirement;

		private: struct ParameterToGetRawPointer_ {
			GeneralParameter_ const input_;
			void const* output_;
		};

		private: struct ParameterToGetRawPointerToMutable_ {
			GeneralMutableParameter_ const input_;
			void* output_;
		};

		private: struct ParameterToCloneTo_ {
			GeneralParameter_ const input_;
			GeneralMutableParameter_ const other_;
		};

		private: struct ParameterToRelocateTo_ {
			::dcool::core::Boolean engineWillBeSwapped_;
			GeneralMutableParameter_ const input_;
			GeneralMutableParameter_ const other_;
		};

		private: using ParameterToDestruct_ = GeneralMutableParameter_;

		private: struct ParameterForExtendedOperation_ {
			Engine* const engine_;
			void* const customizedParameter_;
		};

		private: Storage_ m_storage_;
		private: OperationExecutor m_executor_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		private: static constexpr void nilOperationExecutor_(Opcode opcode_, void* parameter_) {
			switch (opcode_) {
				case Opcode::dcoolGetTypeInfo_: {
					*static_cast<ParameterToGetTypeInfo_*>(parameter_) = ::dcool::core::addressOf(typeid(void));
					break;
				}
				case Opcode::dcoolGetStorageRequirement_: {
					*static_cast<ParameterToGetStorageRequirement_*>(parameter_) = ::dcool::core::storageRequirement<0, 0>;
					break;
				}
				case Opcode::dcoolGetRawPointer_: {
					static_cast<ParameterToGetRawPointer_*>(parameter_)->output_ = ::dcool::core::nullPointer;
					break;
				}
				case Opcode::dcoolGetRawPointerToMutable_: {
					static_cast<ParameterToGetRawPointerToMutable_*>(parameter_)->output_ = ::dcool::core::nullPointer;
					break;
				}
				case Opcode::dcoolCloneTo_: {
					auto actualParameter_ = static_cast<ParameterToCloneTo_*>(parameter_);
					actualParameter_->other_.self_->initialize(*(actualParameter_->other_.engine_));
					break;
				}
				case Opcode::dcoolRelocateTo_: {
					auto actualParameter_ = static_cast<ParameterToRelocateTo_*>(parameter_);
					actualParameter_->other_.self_->initialize(*(actualParameter_->other_.engine_));
					break;
				}
				case Opcode::dcoolDestruct_: {
					break;
				}
				default: {
					auto actualParameter_ = static_cast<ParameterForExtendedOperation_*>(parameter_);
					actualParameter_->engine_->exetendedOperationExecutor()(
						::dcool::core::typed<void>, actualParameter_->customizedParameter_
					);
					break;
				}
			}
		}

		private: template <typename ValueT__> static constexpr void basicOperationExecutor_(Opcode opcode_, void* parameter_) {
			switch (opcode_) {
				case Opcode::dcoolGetTypeInfo_: {
					*static_cast<ParameterToGetTypeInfo_*>(parameter_) = ::dcool::core::addressOf(typeid(ValueT__));
					break;
				}
				case Opcode::dcoolGetStorageRequirement_: {
					*static_cast<ParameterToGetStorageRequirement_*>(parameter_) = ::dcool::core::storageRequirementFor<ValueT__>;
					break;
				}
				case Opcode::dcoolGetRawPointer_: {
					auto actualParameter_ = static_cast<ParameterToGetRawPointer_*>(parameter_);
					actualParameter_->output_ = actualParameter_->input_.self_->template rawPointer_<ValueT__>(
						*(actualParameter_->input_.engine_)
					);
					break;
				}
				case Opcode::dcoolGetRawPointerToMutable_: {
					auto actualParameter_ = static_cast<ParameterToGetRawPointerToMutable_*>(parameter_);
					actualParameter_->output_ = actualParameter_->input_.self_->template rawPointer_<ValueT__>(
						*(actualParameter_->input_.engine_)
					);
					break;
				}
				case Opcode::dcoolCloneTo_: {
					auto actualParameter_ = static_cast<ParameterToCloneTo_*>(parameter_);
					actualParameter_->other_.self_->template initialize<ValueT__>(
						*(actualParameter_->other_.engine_),
						::dcool::core::inPlace,
						actualParameter_->input_.self_->template access<ValueT__>(*(actualParameter_->input_.engine_))
					);
					break;
				}
				case Opcode::dcoolRelocateTo_: {
					auto actualParameter_ = static_cast<ParameterToRelocateTo_*>(parameter_);
					if (
						::dcool::core::isStorable<ValueT__, squeezedTankage> ||
						(
							(!(actualParameter_->engineWillBeSwapped_)) &&
							*(actualParameter_->input_.engine_) != *(actualParameter_->other_.engine_)
						)
					) {
						actualParameter_->other_.self_->template initialize<ValueT__>(
							*(actualParameter_->other_.engine_),
							::dcool::core::inPlace,
							::dcool::core::move(
								actualParameter_->input_.self_->template access<ValueT__>(*(actualParameter_->other_.engine_))
							)
						);
						actualParameter_->input_.self_->template destruct_<ValueT__>(*(actualParameter_->input_.engine_));
					} else {
						actualParameter_->other_.self_->m_storage_.activateAlternative();
						actualParameter_->other_.self_->m_storage_.alternative() = actualParameter_->input_.self_->m_storage_.alternative();
						actualParameter_->input_.self_->m_storage_.deactivateAlternative();
					}
					break;
				}
				case Opcode::dcoolDestruct_: {
					auto actualParameter_ = static_cast<ParameterToDestruct_*>(parameter_);
					actualParameter_->self_->template destruct_<ValueT__>(*(actualParameter_->engine_));
					break;
				}
				default: {
					auto actualParameter_ = static_cast<ParameterForExtendedOperation_*>(parameter_);
					actualParameter_->engine_->exetendedOperationExecutor()(
						::dcool::core::typed<ValueT__>, actualParameter_->customizedParameter_
					);
					break;
				}
			}
		}

		public: constexpr void initialize(Engine& engine_) noexcept {
			this->m_executor_ = nilOperationExecutor_;
		}

		public: template <typename ValueT__> constexpr void initialize(Engine& engine_, ValueT__&& value_) {
			this->initialize<::dcool::core::DecayedType<ValueT__>>(
				engine_, ::dcool::core::inPlace, ::dcool::core::forward<ValueT__>(value_)
			);
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
			Engine& engine_, ::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
		) {
			static_assert(!::dcool::core::Const<ValueT__>);
			if constexpr (::dcool::core::isStorable<ValueT__, squeezedTankage>) {
				this->m_storage_.activateItem();
				new (this->rawPointer_<ValueT__>(engine_)) ValueT__(::dcool::core::forward<ArgumentTs__>(parameters_)...);
			} else {
				this->m_storage_.activateAlternative();
				this->m_storage_.alternative() = ::dcool::resource::createHandleByPoolFor<ValueT__>(
					engine_.pool(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
				);
			}
			this->m_executor_ = basicOperationExecutor_<ValueT__>;
		}

		private: template <typename ValueT__> constexpr void destruct_(Engine& engine_) {
			if constexpr (::dcool::core::isStorable<ValueT__, squeezedTankage>) {
				::dcool::core::destruct(this->access<ValueT__>(engine_));
				this->m_storage_.deactivateItem();
			} else {
				::dcool::resource::destroyHandleByPoolFor<ValueT__>(engine_.pool(), this->m_storage_.alternative());
				this->m_storage_.deactivateAlternative();
			}
		}

		public: constexpr void uninitialize(Engine& engine_) {
			ParameterToDestruct_ parameter_ = { .engine_ = ::dcool::core::addressOf(engine_), .self_ = this };
			this->m_executor_(Opcode::dcoolDestruct_, ::dcool::core::addressOf(parameter_));
		}

		public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const {
			ParameterToCloneTo_ parameter_ = {
				.input_ = { .engine_ = ::dcool::core::addressOf(engine_), .self_ = this },
				.other_ = { .engine_ = ::dcool::core::addressOf(otherEngine_), .self_ = ::dcool::core::addressOf(other_) },
			};
			this->m_executor_(Opcode::dcoolCloneTo_, ::dcool::core::addressOf(parameter_));
		}

		public: template <
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			ParameterToRelocateTo_ parameter_ = {
				.engineWillBeSwapped_ = engineWillBeSwappedC__,
				.input_ = { .engine_ = ::dcool::core::addressOf(engine_), .self_ = this },
				.other_ = { .engine_ = ::dcool::core::addressOf(otherEngine_), .self_ = ::dcool::core::addressOf(other_) },
			};
			this->m_executor_(Opcode::dcoolRelocateTo_, ::dcool::core::addressOf(parameter_));
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			Self_ middleMan_;
			this->relocateTo<engineWillBeSwappedC__>(engine_, engine_, middleMan_);
			struct UniqueType_;
			try {
				other_.relocateTo<engineWillBeSwappedC__>(otherEngine_, engine_, *this);
				try {
					middleMan_.relocateTo<engineWillBeSwappedC__>(engine_, otherEngine_, other_);
				} catch (...) {
					try {
						this->relocateTo(engine_, otherEngine_, other_);
					} catch (...) {
						::dcool::core::goWeak<strategyC__>();
						middleMan_.uninitialize(engine_);
						other_.initialize(engine_);
						throw ::dcool::core::ExceptionSafetyDowngrade<UniqueType_>();
					}
					throw;
				}
			} catch (::dcool::core::ExceptionSafetyDowngrade<UniqueType_> const& exception_) {
				exception_.rethrowUnderlying();
			} catch (...) {
				try {
					middleMan_.relocateTo(engine_, engine_, *this);
				} catch (...) {
					::dcool::core::goWeak<strategyC__>();
					middleMan_.uninitialize(engine_);
					this->initialize(engine_);
				}
				throw;
			}
		}

		public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return ::dcool::core::size(this->storageRequirement(engine_)) > 0;
		}

		public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
			ParameterToGetTypeInfo_ parameter_;
			this->m_executor_(Opcode::dcoolGetTypeInfo_, ::dcool::core::addressOf(parameter_));
			return *parameter_;
		}

		public: constexpr auto storageRequirement(Engine& engine_) const noexcept {
			ParameterToGetStorageRequirement_ parameter_;
			this->m_executor_(Opcode::dcoolGetStorageRequirement_, ::dcool::core::addressOf(parameter_));
			return parameter_;
		}

		private: template <typename ValueT__> constexpr auto rawPointer_(Engine& engine_) const noexcept -> ValueT__ const* {
			if constexpr (::dcool::core::isStorable<ValueT__, squeezedTankage>) {
				return reinterpret_cast<ValueT__ const*>(::dcool::core::addressOf(this->m_storage_.item()));
			}
			return static_cast<ValueT__*>(
				::dcool::resource::adaptedFromHandleFor<ValueT__>(engine_.pool(), this->m_storage_.alternative())
			);
		}

		private: template <typename ValueT__> constexpr auto rawPointer_(Engine& engine_) noexcept -> ValueT__* {
			if constexpr (::dcool::core::isStorable<ValueT__, squeezedTankage>) {
				return reinterpret_cast<ValueT__*>(::dcool::core::addressOf(this->m_storage_.item()));
			}
			return static_cast<ValueT__*>(
				::dcool::resource::adaptedFromHandleFor<ValueT__>(engine_.pool(), this->m_storage_.alternative())
			);
		}

		public: constexpr auto storage(Engine& engine_) const noexcept -> void const* {
			ParameterToGetRawPointer_ parameter_ = { .input_ = { .engine_ = ::dcool::core::addressOf(engine_), .self_ = this } };
			this->m_executor_(Opcode::dcoolGetRawPointer_, ::dcool::core::addressOf(parameter_));
			return parameter_.output_;
		}

		public: constexpr auto storage(Engine& engine_) noexcept -> void* {
			ParameterToGetRawPointerToMutable_ parameter_ = {
				.input_ = { .engine_ = ::dcool::core::addressOf(engine_), .self_ = this }
			};
			this->m_executor_(Opcode::dcoolGetRawPointerToMutable_, ::dcool::core::addressOf(parameter_));
			return parameter_.output_;
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
			static_assert(!::dcool::core::Const<ValueT__>);
			return *::dcool::core::launder(this->rawPointer_<ValueT__>(engine_));
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
			static_assert(!::dcool::core::Const<ValueT__>);
			return *::dcool::core::launder(this->rawPointer_<ValueT__>(engine_));
		}

		public: template <typename ValueT__> constexpr auto value(Engine& engine_) const -> ValueT__ const& {
			static_assert(!::dcool::core::Const<ValueT__>);
			if (typeid(ValueT__) != this->typeInfo(engine_)) {
				throw ::dcool::utility::BadAnyCast();
			}
			return this->access<ValueT__>(engine_);
		}

		public: template <typename ValueT__> constexpr auto value(Engine& engine_) -> ValueT__& {
			static_assert(!::dcool::core::Const<ValueT__>);
			if (typeid(ValueT__) != this->typeInfo(engine_)) {
				throw ::dcool::utility::BadAnyCast();
			}
			return this->access<ValueT__>(engine_);
		}

		protected: constexpr void executeCustomizedOperation(
			Engine& engine_, Opcode opcode_, void* customizedParameter_
		) const {
			ParameterForExtendedOperation_ parameter_ = {
				.engine_ = ::dcool::core::addressOf(engine_), .customizedParameter_ = customizedParameter_
			};
			this->m_executor_(opcode_, ::dcool::core::addressOf(parameter_));
		}
	};

	template <::dcool::utility::AnyConfig ConfigT_ = ::dcool::core::Empty<>> struct Any {
		private: using Self_ = Any<ConfigT_>;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::AnyChassis<Config>;
		private: using ConfigAdaptor_ = ::dcool::utility::AnyConfigAdaptor<Config>;
		public: using DynamicHandle = ConfigAdaptor_::DynamicHandle;
		public: using Opcode = ConfigAdaptor_::Opcode;
		public: using OperationExecutor = ConfigAdaptor_::OperationExecutor;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr Any() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr Any(Self_ const& other_) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine, *this);
		}

		public: constexpr Any(Self_&& other_) {
			other_.chassis().relocateTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(this->mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr Any(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr Any(
			::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
		) {
			this->chassis().template initialize<ValueT__>(
				this->mutableEngine(), ::dcool::core::inPlace, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: constexpr ~Any() noexcept {
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

		protected: constexpr void executeCustomizedOperation(Opcode opcode_, void* customizedParameter_) const {
			this->chassis().executeCustomizedOperation(this->mutableEngine(), opcode_, customizedParameter_);
		}
	};
}

#endif
