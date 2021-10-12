#ifndef DCOOL_VIGOR_DOUBLE_BUFFER_HPP_INCLUDED_
#	define DCOOL_VIGOR_DOUBLE_BUFFER_HPP_INCLUDED_

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <mutex>
#	include <shared_mutex>

namespace dcool::service {
	namespace detail_ {
		template <typename ValueT_, typename EngineT_> struct SealerStorage_ {
			private: using Self_ = SealerStorage_<ValueT_, EngineT_>;
			public: using Value = ValueT_;
			public: using Engine = EngineT_;

			private: using PoolAdaptor_ = ::dcool::resource::PoolAdaptorFor<
				::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<Engine&>().pool())>, Value
			>;
			public: using Handle = PoolAdaptor_::UnifiedHandle;
			public: using ConstHandle = PoolAdaptor_::UnifiedConstHandle;

			private: Handle m_value_;

			public: constexpr SealerStorage_() noexcept = default;
			public: SealerStorage_(Self_ const&) = delete;
			public: SealerStorage_(Self_&&) = delete;
			public: constexpr ~SealerStorage_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: template <typename... ArgumentTs_> constexpr void initialize(Engine& engine_, ArgumentTs_&&... parameters_) {
				this->m_value_ = ::dcool::resource::createHandleByPoolFor<Value>(
					engine_.pool(), ::dcool::core::forward<ArgumentTs_>(parameters_)...
				);
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				::dcool::resource::destroyHandleByPoolFor<Value>(engine_.pool(), this->m_value_);
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				other_.m_value_ = this->m_value_;
			}

			private: constexpr auto valueAddress_(Engine& engine_) const noexcept -> Value const* {
				return static_cast<Value const*>(PoolAdaptor_::fromHandle(engine_.pool(), this->m_value_));
			}

			private: constexpr auto valueAddress_(Engine& engine_) noexcept -> Value* {
				return static_cast<Value*>(PoolAdaptor_::fromHandle(engine_.pool(), this->m_value_));
			}

			public: constexpr auto value(Engine& engine_) const noexcept -> Value const& {
				return *(this->valueAddress_(engine_));
			}

			public: constexpr auto value(Engine& engine_) noexcept -> Value& {
				return *(this->valueAddress_(engine_));
			}
		};

		template <::dcool::core::NoThrowMoveAssignable ValueT_, typename EngineT_> struct SealerStorage_<ValueT_, EngineT_> {
			private: using Self_ = SealerStorage_<ValueT_, EngineT_>;
			public: using Value = ValueT_;
			public: using Engine = EngineT_;

			private: ::dcool::core::StorageFor<Value> m_storage_;

			public: constexpr SealerStorage_() = default;
			public: SealerStorage_(Self_ const&) = delete;
			public: SealerStorage_(Self_&&) = delete;
			public: constexpr ~SealerStorage_() = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: template <typename... ArgumentTs_> constexpr void initialize_(ArgumentTs_&&... parameters_) {
				new (this->valueAddress_()) Value(::dcool::core::forward<ArgumentTs_>(parameters_)...);
			}

			public: template <typename... ArgumentTs_> constexpr void initialize(Engine& engine_, ArgumentTs_&&... parameters_) {
				this->initialize_(::dcool::core::forward<ArgumentTs_>(parameters_)...);
			}

			public: constexpr void uninitialize_() noexcept {
				this->value().~Value();
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				this->uninitialize_();
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				other_.initialize_(::dcool::core::move(this->value()));
				this->uninitialize_();
			}

			private: constexpr auto valueAddress_() const noexcept -> Value const* {
				return static_cast<Value const*>(::dcool::core::addressOf(this->m_storage_));
			}

			private: constexpr auto valueAddress_() noexcept -> Value* {
				return static_cast<Value*>(::dcool::core::addressOf(this->m_storage_));
			}

			public: constexpr auto value(Engine& engine_) const noexcept -> Value const& {
				return *::dcool::core::launder(this->valueAddress_(engine_));
			}

			public: constexpr auto value(Engine& engine_) noexcept -> Value& {
				return *::dcool::core::launder(this->valueAddress_(engine_));
			}
		};

		template <typename ConfigT_, typename ValueT_> class SealerConfigAdaptor_ {
			private: using Self_ = SealerConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using Mutex = ::dcool::core::ExtractedMutexType<Config, ::dcool::core::Empty<Self_>>;
			public: using Pool = ::dcool::resource::PoolType<Config>;

			private: struct DefaultEngine_ {
				public: [[no_unique_address]] Mutex partMutex;
				public: [[no_unique_address]] Pool partPool;

				public: constexpr DefaultEngine_() noexcept = default;

				public: constexpr DefaultEngine_(Self_ const& other_) noexcept: partPool(other_.pool()) {
				}

				public: constexpr DefaultEngine_(Self_&& other_) noexcept: partPool(::dcool::core::move(other_.pool())) {
				}

				public: constexpr ~DefaultEngine_() noexcept = default;

				public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& {
					this->pool() = other_.pool();
					return *this;
				}

				public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
					this->pool() = ::dcool::core::move(other_.pool());
					return *this;
				}

				public: constexpr auto mutex() noexcept -> Mutex& {
					return this->partMutex;
				}

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			public: using Storage = ::dcool::service::detail_::SealerStorage_<Value, Engine>;

			static_assert(::dcool::resource::PoolFor<Pool, Value>);
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::pool'"
			);
		};
	}

	template <typename T_, typename ValueT_> concept SealerConfig = requires {
		typename ::dcool::service::detail_::SealerConfigAdaptor_<T_, ValueT_>;
	};

	template <typename ConfigT_, typename ValueT_> requires ::dcool::service::SealerConfig<
		ConfigT_, ValueT_
	> using SealerConfigAdaptor = ::dcool::service::detail_::SealerConfigAdaptor_<ConfigT_, ValueT_>;

	template <typename ValueT_, ::dcool::service::SealerConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct SealerChassis {
		private: using Self_ = SealerChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::service::SealerConfigAdaptor<Config, Value>;
		private: using Storage_ = ConfigAdaptor_::Storage;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Mutex = ConfigAdaptor_::Mutex;

		private: Storage_ m_storage_;

		public: constexpr SealerChassis() = default;
		public: SealerChassis(Self_ const&) = delete;
		public: SealerChassis(Self_&&) = delete;
		public: constexpr ~SealerChassis() = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		public: template <typename... ArgumentTs__> constexpr void initialize(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->m_storage_.initialize(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_storage_.uninitialize(engine_);
		}

		public: template <typename ValueT__, typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::service::SealerChassis<ValueT__, ConfigT__>::Engine& otherEngine_,
			::dcool::service::SealerChassis<ValueT__, ConfigT__>& other_
		) const {
			other_.initialize(otherEngine_, this->copiedValue(engine_));
		}

		public: constexpr void relocateTo(Self_& other_) noexcept {
			this->m_storage_.relocateTo(other_.m_storage_);
		}

		private: constexpr auto value_(Engine& engine_) const noexcept -> Value const& {
			return this->m_storage_.value(engine_);
		}

		private: constexpr auto value_(Engine& engine_) noexcept -> Value& {
			return this->m_storage_.value(engine_);
		}

		public: constexpr auto copiedValue(Engine& engine_) const noexcept -> Value {
			::dcool::core::PhoneySharedLockGuard<Mutex> guard_(engine_.mutex());
			return this->value_(engine_);
		}

		private: template <typename... ArgumentTs__> constexpr auto emplace_(Engine& engine_, ArgumentTs__&&... parameters_) {
			Storage_ newStorage_;
			newStorage_.initialize(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			this->m_storage_.uninitialize(engine_);
			newStorage_.relocateTo(this->m_storage_);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(Engine& engine_, ArgumentTs__&&... parameters_) {
			::dcool::core::PhoneyLockGuard<Mutex> guard_(engine_.mutex());
			this->emplace(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <::dcool::core::Invocable<Value const&> TransactionT__> constexpr void runTransaction(
			Engine& engine_, TransactionT__&& transaction_
		) {
			::dcool::core::PhoneyLockGuard<Mutex> guard_(engine_.mutex());
			this->emplace_(
				engine_,
				::dcool::core::invoke(
					::dcool::core::forward<TransactionT__>(transaction_), ::std::ref(::dcool::core::constantize(this->value_(engine_)))
				)
			);
		}

		public: template <::dcool::core::Invocable<Value const&> ObserverT__> constexpr void runObserver(
			Engine& engine_, ObserverT__&& observer_
		) const {
			::dcool::core::PhoneySharedLockGuard<Mutex> guard_(engine_.mutex());
			::dcool::core::invoke(
				::dcool::core::forward<ObserverT__>(observer_),
				::std::ref(::dcool::core::constantize(this->value_(engine_)))
			);
		}
	};

	template <typename ValueT_, ::dcool::service::SealerConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct Sealer {
		private: using Self_ = Sealer<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::service::SealerConfigAdaptor<Config, Value>;
		public: using Chassis = ::dcool::service::SealerChassis<Value, Config>;
		public: using Engine = ConfigAdaptor_::Engine;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		public: constexpr Sealer() {
			this->chassis().initialize(this->engine_());
		}

		public: template <typename... ArgumentTs__> constexpr Sealer(::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_) {
			this->chassis().initialize(this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr Sealer(Self_ const& other_): m_engine_(other_.engine_()) {
			other_.chassis().cloneTo(other_.engine_(), this->engine_(), this->chassis());
		}

		public: constexpr ~Sealer() noexcept {
			this->chassis().uninitialize(this->engine_());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			this->store(other_.copiedValue());
			return *this;
		}

		public: constexpr auto swapWith(Self_& other_) {
			::dcool::core::intelliSwap(this->engine_(), other_.engine_());
			this->chassis().swapWith(other_.chassis());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		private: constexpr auto engine_() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->engine_();
		}

		public: constexpr auto copiedValue() const noexcept -> Value {
			return this->chassis().copiedValue(this->engine_());
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(ArgumentTs__&&... parameters_) {
			this->chassis().emplace(this->engine_(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <::dcool::core::Invocable<Value const&> TransactionT__> constexpr void runTransaction(
			TransactionT__&& transaction_
		) {
			this->chassis().runTransaction(this->engine_(), ::dcool::core::forward<TransactionT__>(transaction_));
		}

		public: template <
			::dcool::core::Invocable<Value const&> ObserverT__
		> constexpr void runObserver(ObserverT__&& observer_) const {
			this->chassis().runObserver(this->engine_(), ::dcool::core::forward<ObserverT__>(observer_));
		}
	};

	namespace detail_ {
		struct ThreadSafeSealerConfig_ {
			using Mutex = ::dcool::core::MinimalSharedMutex;
		};
	}

	template <typename ValueT_> using ThreadSafeSealer = ::dcool::service::Sealer<
		ValueT_, ::dcool::service::detail_::ThreadSafeSealerConfig_
	>;
}

#endif
