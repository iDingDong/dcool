#ifndef DCOOL_UTILITY_ANY_MUTEX_HPP_INCLUDED_
#	define DCOOL_UTILITY_ANY_MUTEX_HPP_INCLUDED_ 1

#	include <dcool/utility/overloaded_function.hpp>

#	include <dcool/core.hpp>

#	include <chrono>
#	include <stdexcept>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::utility::detail_,
	HasTypeSupportedTimePointsForAnyMutex_,
	extractedSupportedTimePointsForAnyMutexType_,
	SupportedTimePoints
)

namespace dcool::utility {
	namespace detail_ {
		struct AnyMutexTimedLockAttemptTag_ {
			constexpr explicit AnyMutexTimedLockAttemptTag_() noexcept = default;
		};

		inline constexpr ::dcool::utility::detail_::AnyMutexTimedLockAttemptTag_ anyMutexTimedLockAttempt_;

		template <typename SupportedTimePoints> struct AnyMutexTimedTryLockers_;

		template <typename... TimePointT_> struct AnyMutexTimedTryLockers_<::dcool::core::Types<TimePointT_...>> {
			using Result_ = ::dcool::core::Types<
				auto(::dcool::utility::detail_::AnyMutexTimedLockAttemptTag_, TimePointT_)->::dcool::core::Boolean...
			>;
		};

		template <typename ConfigT_> using AnyMutexSupportedTimePointsOfConfigType_ =
			::dcool::utility::detail_::extractedSupportedTimePointsForAnyMutexType_<ConfigT_, ::dcool::core::Types<>>
		;

		template <typename ConfigT_> using AnyMutexTimedTryLockersOfConfigType_ =
			::dcool::utility::detail_::AnyMutexTimedTryLockers_<
				::dcool::utility::detail_::AnyMutexSupportedTimePointsOfConfigType_<ConfigT_>
			>::Result_
		;
	}

	class BadAnyMutexAction: ::std::runtime_error {
		private: using Super_ = ::std::runtime_error;

		public: using Super_::Super_;
	};

	namespace detail_ {
		template <
			::dcool::core::Complete ConfigT_
		> class AnyMutexConfigAdaptor_: public ::dcool::utility::OverloadedFunctionConfigAdaptor<
			ConfigT_, ::dcool::utility::detail_::AnyMutexTimedTryLockersOfConfigType_<ConfigT_>
		> {
			public: using Config = ConfigT_;

			public: using SupportedTimePoints = ::dcool::utility::detail_::AnyMutexSupportedTimePointsOfConfigType_<Config>;
			public: static constexpr ::dcool::core::Boolean copyable =
				::dcool::utility::detail_::extractedCopyableForAnyValue_<Config>(false)
			;
			public: static constexpr ::dcool::core::Boolean moveable =
				::dcool::utility::detail_::extractedMoveableForAnyValue_<Config>(copyable)
			;
			public: static constexpr ::dcool::core::Boolean timedLockable =
				::dcool::utility::detail_::extractedCopyableForAnyValue_<Config>(false)
			;
		};

		template <typename AnyMutexChassisT_, typename MutexT_> constexpr void lockAnyMutexChassis_(
			typename AnyMutexChassisT_::Engine& engine_, AnyMutexChassisT_& chassis_
		) {
			if constexpr (::dcool::core::NonVoid<MutexT_>) {
				::dcool::core::generalLock(chassis_.template access<MutexT_>(engine_));
			} else {
				throw ::dcool::utility::BadAnyMutexAction("Unable to lock an empty 'dcool::utility::AnyMutex'.");
			}
		}

		template <typename AnyMutexChassisT_, typename MutexT_> constexpr auto trylockAnyMutexChassis_(
			typename AnyMutexChassisT_::Engine& engine_, AnyMutexChassisT_& chassis_
		) -> ::dcool::core::Boolean {
			if constexpr (::dcool::core::NonVoid<MutexT_>) {
				return ::dcool::core::generalTryLock(chassis_.template access<MutexT_>(engine_));
			}
			throw ::dcool::utility::BadAnyMutexAction("Unable to lock an empty 'dcool::utility::AnyMutex'.");
		}

		template <typename AnyMutexChassisT_, typename MutexT_> constexpr void unlockAnyMutexChassis_(
			typename AnyMutexChassisT_::Engine& engine_, AnyMutexChassisT_& chassis_
		) {
			if constexpr (::dcool::core::NonVoid<MutexT_>) {
				::dcool::core::generalUnlock(chassis_.template access<MutexT_>(engine_));
			} else {
				throw ::dcool::utility::BadAnyMutexAction("Unable to unlock an empty 'dcool::utility::AnyMutex'.");
			}
		}
	}

	template <typename T_> concept AnyMutexConfig = ::dcool::core::Complete<
		::dcool::utility::detail_::AnyMutexConfigAdaptor_<T_>
	>;

	template <::dcool::utility::AnyMutexConfig ConfigT_> using AnyMutexConfigAdaptor =
		::dcool::utility::detail_::AnyMutexConfigAdaptor_<ConfigT_>
	;

	template <::dcool::utility::AnyMutexConfig ConfigT_ = ::dcool::core::Empty<>> struct AnyMutexChassis {
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::utility::AnyMutexConfigAdaptor<Config>;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using ExtendedInformation = ConfigAdaptor_::ExtendedInformation;
		public: using SupportedTimePoints = ConfigAdaptor_::SupportedTimePoints;
		private: using TimedTryLockers_ = ::dcool::utility::detail_::AnyMutexTimedTryLockersOfConfigType_<Config>;
		public: static constexpr ::dcool::core::Boolean copyable = ConfigAdaptor_::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = ConfigAdaptor_::moveable;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = ConfigAdaptor_::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		private: template <typename ValueT__> struct Stored_ {
			ValueT__ value_;

			template <typename... ArgumentTs___> constexpr Stored_(
				ArgumentTs___&&... parameters_
			): value_(::dcool::core::forward<ArgumentTs___>(parameters_)...) {
			}

			template <typename TimePointT___> constexpr auto operator ()(
				::dcool::utility::detail_::AnyMutexTimedLockAttemptTag_,
				TimePointT___ const& timePoint_
			) -> ::dcool::core::Boolean {
				if constexpr (
					requires {
						{ this->value_.tryLockUntil(timePoint_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
					}
				) {
					return this->value_.tryLockUntil(timePoint_);
				} else if constexpr (
					requires {
						{ this->value_.try_lock_until(timePoint_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
					}
				) {
					return this->value_.try_lock_until(timePoint_);
				}
				throw ::dcool::utility::BadAnyMutexAction("Unable to timed lock the mutex with given type of time point.");
			}
		};

		private: using Locker_ = void(*)(Engine& engine_, AnyMutexChassis& self_);
		private: using TryLocker_ = auto(*)(Engine& engine_, AnyMutexChassis& self_) -> ::dcool::core::Boolean;
		private: using Unlocker_ = void(*)(Engine& engine_, AnyMutexChassis& self_);

		private: struct OverloadedFunctionExtendedInformation_ {
			::dcool::core::StorageRequirement actualStorageRequirement_;
			::dcool::core::TypeInfo const& actualTypeInfo_;
			Locker_ locker_;
			TryLocker_ tryLocker_;
			Unlocker_ unlocker_;
			::dcool::core::Boolean shareable_;
			[[no_unique_address]] ExtendedInformation extendedInformation_;

			constexpr OverloadedFunctionExtendedInformation_(
				::dcool::core::TypedTag<void> typed_
			) noexcept:
				actualStorageRequirement_(::dcool::core::storageRequirementFor<void>),
				actualTypeInfo_(typeid(void)),
				locker_(::dcool::utility::detail_::lockAnyMutexChassis_<AnyMutexChassis, void>),
				tryLocker_(::dcool::utility::detail_::trylockAnyMutexChassis_<AnyMutexChassis, void>),
				unlocker_(::dcool::utility::detail_::unlockAnyMutexChassis_<AnyMutexChassis, void>),
				shareable_(::dcool::core::SharedMutex<void>),
				extendedInformation_(typed_)
			{
			}

			template <typename ValueT__> constexpr OverloadedFunctionExtendedInformation_(
				::dcool::core::TypedTag<Stored_<ValueT__>>
			) noexcept:
				actualStorageRequirement_(::dcool::core::storageRequirementFor<ValueT__>),
				actualTypeInfo_(typeid(ValueT__)),
				locker_(::dcool::utility::detail_::lockAnyMutexChassis_<AnyMutexChassis, ValueT__>),
				tryLocker_(::dcool::utility::detail_::trylockAnyMutexChassis_<AnyMutexChassis, ValueT__>),
				unlocker_(::dcool::utility::detail_::unlockAnyMutexChassis_<AnyMutexChassis, ValueT__>),
				shareable_(::dcool::core::SharedMutex<ValueT__>),
				extendedInformation_(::dcool::core::typed<ValueT__>)
			{
			}
		};

		private: struct OverloadedFunctionConfig_ {
			using Pool = AnyMutexChassis::Pool;
			using ExtendedInformation = OverloadedFunctionExtendedInformation_;
			using Engine = AnyMutexChassis::Engine;
			static constexpr ::dcool::core::Boolean copyable = copyable;
			static constexpr ::dcool::core::Boolean moveable = moveable;
			static constexpr ::dcool::core::StorageRequirement squeezedTankage = squeezedTankage;
			static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
		};

		private: using Underlying_ = ::dcool::utility::OverloadedFunctionChassis<TimedTryLockers_, OverloadedFunctionConfig_>;

		private: Underlying_ m_underlying_;

		public: void initialize(Engine& engine_) noexcept {
			this->m_underlying_.initialize(engine_);
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
			Engine& engine_, ::dcool::core::InPlaceTag tag_, ArgumentTs__&&... parameters_
		) noexcept(
			noexcept(
				::dcool::core::declval<Underlying_&>().template initialize<ValueT__>(
					engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
				)
			)
		) {
			this->m_underlying_.template initialize<Stored_<ValueT__>>(
				engine_, tag_, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_underlying_.uninitialize(engine_);
		}

		public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, AnyMutexChassis& other_) const {
			this->m_underlying_.cloneTo(engine_, otherEngine_, other_.m_underlying_);
		}

		public: template <
			::dcool::core::Boolean engineWillBeUniformedC__ = false
		> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, AnyMutexChassis& other_) {
			this->m_underlying_.template relocateTo<engineWillBeUniformedC__>(engine_, otherEngine_, other_.m_underlying_);
		}

		public: public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, AnyMutexChassis& other_) {
			this->m_underlying_.template swapWith<strategyC__, engineWillBeSwappedC__>(engine_, otherEngine_, other_.m_underlying_);
		}

		public: constexpr auto valid(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.valid(engine_);
		}

		public: constexpr auto storageRequirement(Engine& engine_) const noexcept -> ::dcool::core::StorageRequirement {
			return this->m_underlying_.extendedInformation(engine_).actualStorageRequirement_;
		}

		public: constexpr auto typeInfo(Engine& engine_) const noexcept -> ::dcool::core::TypeInfo const& {
			return this->m_underlying_.extendedInformation(engine_).actualTypeInfo_;
		}

		public: constexpr auto extendedInformation(Engine& engine_) const noexcept -> ExtendedInformation const& {
			return this->m_underlying_.extendedInformation(engine_).extendedInformation_;
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) const noexcept -> ValueT__ const& {
			return this->m_underlying_.template access<Stored_<ValueT__>>(engine_).value_;
		}

		public: template <typename ValueT__> constexpr auto access(Engine& engine_) noexcept -> ValueT__& {
			return this->m_underlying_.template access<Stored_<ValueT__>>(engine_).value_;
		}

		public: template <typename ValueT__> constexpr auto value(Engine& engine_) const -> ValueT__ const& {
			return this->m_underlying_.template value<Stored_<ValueT__>>(engine_).value_;
		}

		public: template <typename ValueT__> constexpr auto value(Engine& engine_) -> ValueT__& {
			return this->m_underlying_.template value<Stored_<ValueT__>>(engine_).value_;
		}

		public: constexpr void lock(Engine& engine_) {
			this->m_underlying_.extendedInformation(engine_).locker_(engine_, *this);
		}

		public: constexpr auto tryLock(Engine& engine_) -> ::dcool::core::Boolean {
			return this->m_underlying_.extendedInformation(engine_).tryLocker_(engine_, *this);
		}

		public: template <typename TimePointT__> constexpr auto tryLockUntil(
			Engine& engine_, TimePointT__ const& timePoint_
		) -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			try {
				result_ = this->m_underlying_.invokeSelf(engine_, ::dcool::utility::detail_::anyMutexTimedLockAttempt_, timePoint_);
			} catch (::dcool::utility::BadFunctionCall const&) {
				throw ::dcool::utility::BadAnyMutexAction("Unable to timed lock an empty 'dcool::utility::AnyMutex'.");
			}
			return result_;
		}

		public: template <typename DurationT__> constexpr auto tryLockFor(
			Engine& engine_, DurationT__ const& duration_
		) -> ::dcool::core::Boolean {
			return this->tryLockUntil(engine_, ::std::chrono::steady_clock::now() + duration_);
		}

		public: constexpr void unlock(Engine& engine_) {
			this->m_underlying_.extendedInformation(engine_).unlocker_(engine_, *this);
		}
	};

	template <::dcool::utility::AnyMutexConfig ConfigT_ = ::dcool::core::Empty<>> struct AnyMutex {
		private: using Self_ = AnyMutex<ConfigT_>;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::AnyMutexChassis<Config>;
		public: using Engine = Chassis::Engine;
		public: using ExtendedInformation = Chassis::ExtendedInformation;
		public: static constexpr ::dcool::core::Boolean copyable = Chassis::copyable;
		public: static constexpr ::dcool::core::Boolean moveable = Chassis::moveable;
		public: static constexpr ::dcool::core::StorageRequirement squeezedTankage = Chassis::squeezedTankage;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			Chassis::exceptionSafetyStrategy
		;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr AnyMutex() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr AnyMutex(Self_ const& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
		}

		public: constexpr AnyMutex(Self_&& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().template relocateTo<true>(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(other_.mutableEngine());
		}

		public: template <typename ValueT__> requires (!::dcool::core::FormOfSame<ValueT__, Self_>) constexpr AnyMutex(
			ValueT__&& value_
		) {
			this->chassis().initialize(this->mutableEngine(), ::dcool::core::forward<ValueT__>(value_));
		}

		public: template <typename ValueT__, typename... ArgumentTs__> constexpr explicit AnyMutex(
			::dcool::core::TypedInPlaceTag<ValueT__>, ArgumentTs__&&... parameters_
		) {
			this->chassis().template initialize<ValueT__>(
				this->mutableEngine(), ::dcool::core::inPlace, ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
		}

		public: constexpr ~AnyMutex() noexcept {
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

		public: constexpr void lock() {
			this->chassis().lock(this->mutableEngine());
		}

		public: constexpr auto tryLock() -> ::dcool::core::Boolean {
			return this->chassis().tryLock(this->mutableEngine());
		}

		public: template <typename TimePointT__> constexpr auto tryLockUntil(
			TimePointT__ const& timePoint_
		) -> ::dcool::core::Boolean {
			return this->chassis().tryLockUntil(this->mutableEngine(), timePoint_);
		}

		public: template <typename DurationT__> constexpr auto tryLockFor(DurationT__ const& duration_) -> ::dcool::core::Boolean {
			return this->chassis().tryLockFor(this->mutableEngine(), duration_);
		}

		public: constexpr void unlock() {
			this->chassis().unlock(this->mutableEngine());
		}
	};
}

# endif
