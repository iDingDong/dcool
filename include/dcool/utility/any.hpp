#ifndef DCOOL_UTILITY_ANY_HPP_INCLUDED_
#	define DCOOL_UTILITY_ANY_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <any>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::utility::detail_, HasTypeExtendedInformationForAny_, ExtractedExtendedInformationForAnyType_, ExtendedInformation
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::utility::detail_, HasValueCopyableForAny_, extractedCopyableForAnyValue_, copyable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::utility::detail_, HasValueMoveableForAny_, extractedMoveableForAnyValue_, moveable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::utility::detail_, HasValueSqueezedTankageForAny_, extractedSqueezedTankageForAnyValue_, squeezedTankage
)

namespace dcool::utility {
	using BadAnyCast = ::std::bad_any_cast;

	namespace detail_ {
		template <::dcool::core::Complete ConfigT_> class AnyConfigAdaptor_ {
			private: using Self_ = AnyConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using DynamicHandle = ::dcool::resource::UnifiedHandleType<Pool>;
			public: using ExtendedInformation = ::dcool::utility::detail_::ExtractedExtendedInformationForAnyType_<
				Config, ::dcool::core::Pit
			>;
			public: static constexpr ::dcool::core::Boolean copyable =
				::dcool::utility::detail_::extractedCopyableForAnyValue_<Config>(true)
			;
			public: static constexpr ::dcool::core::Boolean moveable =
				::dcool::utility::detail_::extractedMoveableForAnyValue_<Config>(copyable)
			;
			public: static constexpr ::dcool::core::StorageRequirement squeezedTankage =
				::dcool::utility::detail_::extractedSqueezedTankageForAnyValue_<Config>(
					::dcool::core::storageRequirementFor<DynamicHandle>
				)
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

		template <typename ConfigT_> struct AnyChassis_ {
			private: using Self_ = AnyChassis_<ConfigT_>;
			public: using Config = ConfigT_;

			private: using ConfigAdaptor_ = ::dcool::utility::detail_::AnyConfigAdaptor_<Config>;
			public: using DynamicHandle = ConfigAdaptor_::DynamicHandle;
			public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
			public: using Engine = ConfigAdaptor_::Engine;
			public: static constexpr ::dcool::core::Boolean copyable = ConfigAdaptor_::copyable;
			public: static constexpr ::dcool::core::Boolean moveable = ConfigAdaptor_::moveable;
			public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;
			public: static constexpr ::dcool::core::Boolean noThrowMovable = ::dcool::core::atAnyCost(exceptionSafetyStrategy);
			private: template <typename ValueT__> static constexpr ::dcool::core::Boolean squeezable_ =
				::dcool::core::isStorable<ValueT__, squeezedTankage> &&
				((!::dcool::core::atAnyCost(exceptionSafetyStrategy)) || ::dcool::core::NoThrowMoveConstructible<ValueT__>)
			;

			private: using Storage_ = ::dcool::resource::StorageSqueezer<squeezedTankage, DynamicHandle>;
			private: using Cloner_ = ::dcool::core::ConditionalType<
				copyable,
				void (*)(Engine& engine_, Self_ const& self_, Engine& otherEngine_, Self_& other_),
				::dcool::core::Pit
			>;
			private: using Relocator_ = ::dcool::core::ConditionalType<
				moveable,
				void (*)(Engine& engine_, Self_& self_, Self_& other_) noexcept(noThrowMovable),
				::dcool::core::Pit
			>;
			private: using CrossRelocator_ = ::dcool::core::ConditionalType<
				moveable,
				void (*)(Engine& engine_, Self_& self_, Engine& otherEngine_, Self_& other_),
				::dcool::core::Pit
			>;
			private: using Destructor_ = void (*)(Engine& engine_, Self_& self_) noexcept;

			private: struct InformationTable_ {
				::dcool::core::StorageRequirement storageRequirement_;
				::dcool::core::TypeInfo const& typeInfo_;
				[[no_unique_address]] Cloner_ cloner_;
				[[no_unique_address]] Relocator_ relocator_;
				[[no_unique_address]] CrossRelocator_ crossRelocator_;
				Destructor_ destructor_;
				[[no_unique_address]] ExtendedInformation extendedInformation_;
			};

			private: InformationTable_ const* m_informationTable_;
			private: Storage_ m_storage_;
			private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

			private: template <typename ValueT__> static void clone_(
				Engine& engine_, Self_ const& from_, Engine& otherEngine_, Self_& to_
			) {
				if constexpr (copyable) {
					if constexpr (::dcool::core::NonVoid<ValueT__>) {
						to_.initialize<ValueT__>(otherEngine_, ::dcool::core::inPlace, from_.access<ValueT__>(engine_));
					} else {
						to_.initialize(otherEngine_);
					}
				} else {
					::dcool::core::terminate();
				}
			}

			private: template <typename ValueT__> static void relocate_(
				Engine& engine_, Self_& from_, Self_& to_
			) noexcept(noThrowMovable) {
				if constexpr (moveable) {
					if constexpr (::dcool::core::NonVoid<ValueT__>) {
						if constexpr (squeezable_<ValueT__>) {
							to_.initialize<ValueT__>(engine_, ::dcool::core::inPlace, ::dcool::core::move(from_.access<ValueT__>(engine_)));
							destruct_<ValueT__>(engine_, from_);
						} else {
							to_.m_storage_.activateAlternative();
							to_.m_storage_.alternative() = from_.m_storage_.alternative();
							from_.m_storage_.deactivateAlternative();
							to_.m_informationTable_ = ::dcool::core::addressOf(informationTable_<ValueT__>);
							from_.m_informationTable_ = ::dcool::core::addressOf(informationTable_<void>);
						}
					} else {
						to_.initialize(engine_);
						destruct_<ValueT__>(engine_, from_);
					}
				} else {
					::dcool::core::terminate();
				}
			}

			private: template <typename ValueT__> static void crossRelocate_(
				Engine& engine_, Self_& from_, Engine& otherEngine_, Self_& to_
			) {
				if constexpr (moveable) {
					if constexpr (::dcool::core::NonVoid<ValueT__>) {
						if constexpr (squeezable_<ValueT__>) {
							to_.initialize<ValueT__>(
								otherEngine_, ::dcool::core::inPlace, ::dcool::core::move(from_.access<ValueT__>(engine_))
							);
							destruct_<ValueT__>(engine_, from_);
						} else {
							if (engine_ == otherEngine_) {
								to_.m_storage_.activateAlternative();
								to_.m_storage_.alternative() = from_.m_storage_.alternative();
								from_.m_storage_.deactivateAlternative();
								to_.m_informationTable_ = ::dcool::core::addressOf(informationTable_<ValueT__>);
								from_.m_informationTable_ = ::dcool::core::addressOf(informationTable_<void>);
							} else {
								to_.initialize<ValueT__>(
									otherEngine_, ::dcool::core::inPlace, ::dcool::core::move(from_.access<ValueT__>(engine_))
								);
								destruct_<ValueT__>(engine_, from_);
							}
						}
					} else {
						to_.initialize(otherEngine_);
						destruct_<ValueT__>(engine_, from_);
					}
				} else {
					::dcool::core::terminate();
				}
			}

			private: template <typename ValueT__> static void destruct_(Engine& engine_, Self_& self_) noexcept {
				self_.destructSelf_<ValueT__>(engine_);
			}

			private: template <typename ValueT__> static constexpr InformationTable_ informationTable_ = {
				.storageRequirement_ = ::dcool::core::storageRequirementFor<ValueT__>,
				.typeInfo_ = typeid(ValueT__),
				.cloner_ = clone_<ValueT__>,
				.relocator_ = relocate_<ValueT__>,
				.crossRelocator_ = crossRelocate_<ValueT__>,
				.destructor_ = destruct_<ValueT__>,
				.extendedInformation_ = ExtendedInformation(::dcool::core::typed<ValueT__>)
			};

			public: constexpr void initialize(Engine& engine_) noexcept {
				this->m_informationTable_ = ::dcool::core::addressOf(informationTable_<void>);
			}

			public: template <typename ValueT__> constexpr void initialize(Engine& engine_, ValueT__&& value_) noexcept(
				noexcept(
					this->template initialize<::dcool::core::DecayedType<ValueT__>>(
						engine_, ::dcool::core::inPlace, ::dcool::core::forward<ValueT__>(value_)
					)
				)
			) {
				this->initialize<::dcool::core::DecayedType<ValueT__>>(
					engine_, ::dcool::core::inPlace, ::dcool::core::forward<ValueT__>(value_)
				);
			}

			public: template <typename ValueT__, typename... ArgumentTs__> constexpr void initialize(
				Engine& engine_, ::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
			) noexcept(
				squeezable_<ValueT__> && noexcept(ValueT__(::dcool::core::forward<ArgumentTs__>(parameters_)...))
			) requires ::dcool::core::SameAs<ValueT__, ::dcool::core::DecayedType<ValueT__>> {
				if constexpr (squeezable_<ValueT__>) {
					this->m_storage_.activateItem();
					new (this->rawPointer_<ValueT__>(engine_)) ValueT__(::dcool::core::forward<ArgumentTs__>(parameters_)...);
				} else {
					this->m_storage_.activateAlternative();
					this->m_storage_.alternative() = ::dcool::resource::createHandleByPoolFor<ValueT__>(
						engine_.pool(), ::dcool::core::forward<ArgumentTs__>(parameters_)...
					);
				}
				this->m_informationTable_ = ::dcool::core::addressOf(informationTable_<ValueT__>);
			}

			private: template <typename ValueT__> constexpr void destructSelf_(Engine& engine_) noexcept {
				if constexpr (::dcool::core::NonVoid<ValueT__>) {
					if constexpr (squeezable_<ValueT__>) {
						::dcool::core::destruct(this->access<ValueT__>(engine_));
						this->m_storage_.deactivateItem();
					} else {
						::dcool::resource::destroyHandleByPoolFor<ValueT__>(engine_.pool(), this->m_storage_.alternative());
						this->m_storage_.deactivateAlternative();
					}
				}
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				this->m_informationTable_->destructor_(engine_, *this);
			}

			public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const requires (copyable) {
				this->m_informationTable_->cloner_(engine_, *this, otherEngine_, other_);
			}

			public: template <
				::dcool::core::Boolean engineWillBeUniformedC__ = false
			> constexpr void relocateTo(
				Engine& engine_, Engine& otherEngine_, Self_& other_
			) noexcept(engineWillBeUniformedC__ && noThrowMovable) requires (moveable) {
				if constexpr (engineWillBeUniformedC__) {
					this->m_informationTable_->relocator_(engine_, *this, other_);
				} else {
					this->m_informationTable_->crossRelocator_(engine_, *this, otherEngine_, other_);
				}
			}

			public: template <
				::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
				::dcool::core::Boolean engineWillBeSwappedC__ = false
			> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) requires (moveable) {
				Self_ middleMan_;
				this->relocateTo<true>(engine_, engine_, middleMan_);
				struct UniqueType_ {
				};
				try {
					other_.relocateTo<engineWillBeSwappedC__>(otherEngine_, engine_, *this);
					try {
						middleMan_.relocateTo<engineWillBeSwappedC__>(engine_, otherEngine_, other_);
					} catch (...) {
						try {
							this->relocateTo<engineWillBeSwappedC__>(engine_, otherEngine_, other_);
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
						middleMan_.relocateTo<true>(engine_, engine_, *this);
					} catch (...) {
						::dcool::core::goWeak<strategyC__>();
						middleMan_.uninitialize(engine_);
						this->initialize(engine_);
					}
					throw;
				}
			}

			public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
				return this->m_informationTable_->storageRequirement_;
			}

			public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				return ::dcool::core::size(this->storageRequirement(engine_)) > 0;
			}

			public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
				return this->m_informationTable_->typeInfo_;
			}

			public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation const& {
				return this->m_informationTable_->extendedInformation_;
			}

			private: template <
				typename ValueT__
			> constexpr auto rawPointer_(Engine& engine_) const noexcept -> ValueT__ const* requires ::dcool::core::SameAs<
				ValueT__, ::dcool::core::DecayedType<ValueT__>
			> {
				if constexpr (squeezable_<ValueT__>) {
					return reinterpret_cast<ValueT__ const*>(::dcool::core::addressOf(this->m_storage_.item()));
				}
				return static_cast<ValueT__ const*>(
					::dcool::resource::adaptedFromHandleFor<ValueT__>(engine_.pool(), this->m_storage_.alternative())
				);
			}

			private: template <
				typename ValueT__
			> constexpr auto rawPointer_(Engine& engine_) noexcept -> ValueT__* requires ::dcool::core::SameAs<
				ValueT__, ::dcool::core::DecayedType<ValueT__>
			> {
				if constexpr (squeezable_<ValueT__>) {
					return reinterpret_cast<ValueT__*>(::dcool::core::addressOf(this->m_storage_.item()));
				}
				return static_cast<ValueT__*>(
					::dcool::resource::adaptedFromHandleFor<ValueT__>(engine_.pool(), this->m_storage_.alternative())
				);
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
				DCOOL_CORE_ASSERT(typeid(ValueT__) == this->typeInfo(engine_));
				return *::dcool::core::launder(this->rawPointer_<ValueT__>(engine_));
			}

			public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
				DCOOL_CORE_ASSERT(typeid(ValueT__) == this->typeInfo(engine_));
				return *::dcool::core::launder(this->rawPointer_<ValueT__>(engine_));
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) const -> ValueT__ const& {
				if (typeid(ValueT__) != this->typeInfo(engine_)) {
					throw ::dcool::utility::BadAnyCast();
				}
				return this->access<ValueT__>(engine_);
			}

			public: template <typename ValueT__> constexpr auto value(Engine& engine_) -> ValueT__& {
				if (typeid(ValueT__) != this->typeInfo(engine_)) {
					throw ::dcool::utility::BadAnyCast();
				}
				return this->access<ValueT__>(engine_);
			}
		};
	}

	template <typename T_> concept AnyConfig = requires {
		typename ::dcool::utility::detail_::AnyConfigAdaptor_<T_>;
	};

	template <typename ConfigT_> using AnyConfigAdaptor = ::dcool::utility::detail_::AnyConfigAdaptor_<ConfigT_>;

	template <
		::dcool::utility::AnyConfig ConfigT_ = ::dcool::core::Empty<>
	> using AnyChassis = ::dcool::utility::detail_::AnyChassis_<ConfigT_>;

	template <::dcool::utility::AnyConfig ConfigT_ = ::dcool::core::Empty<>> struct Any {
		private: using Self_ = Any<ConfigT_>;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::AnyChassis<Config>;
		private: using ConfigAdaptor_ = ::dcool::utility::detail_::AnyConfigAdaptor_<Config>;
		public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean copyable = Chassis::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = Chassis::moveable;
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
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
		}

		public: constexpr Any(Self_&& other_) {
			other_.chassis().relocateTo<true>(other_.mutableEngine(), other_->mutableEngine(), this->chassis());
			this->m_engine_ = ::dcool::core::move(other_->mutableEngine());
			other_.chassis().initialize(this->mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr Any(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr explicit Any(
			::dcool::core::TypedInPlaceTag<ValueT__>, ArgumentTs__&&... parameters_
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

		public: constexpr auto storageRequirement() const noexcept -> ::dcool::core::StorageRequirement {
			return this->chassis().storageRequirement(this->mutableEngine());
		}

		public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().valid(this->mutableEngine());
		}

		public: constexpr auto typeInfo() const noexcept -> ::dcool::core::TypeInfo const& {
			return this->chassis().typeInfo(this->mutableEngine());
		}

		public: constexpr auto extendedInformation() const noexcept -> ExtendedInformation const& {
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
	};
}

#endif
