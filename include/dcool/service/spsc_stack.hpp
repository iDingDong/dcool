#ifndef DCOOL_SERVICE_SPSC_STACK_HPP_INCLUDED_
#	define DCOOL_SERVICE_SPSC_STACK_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

#	include <atomic>

namespace dcool::service {
	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class SpscStackConfigAdaptor_ {
			private: using Self_ = SpscStackConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;

			private: struct DefaultEngine_ {
				private: using Self_ = DefaultEngine_;

				public: [[no_unique_address]] Pool partPool;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}

				public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
				public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::pool'"
			);
		};
	}

	template <typename T_, typename ValueT_> concept SpscStackConfig = requires {
		typename ::dcool::service::detail_::SpscStackConfigAdaptor_<T_, ValueT_>;
	};

	template <
		typename ConfigT_, typename ValueT_
	> requires ::dcool::service::SpscStackConfig<ConfigT_, ValueT_> using SpscStackConfigAdaptor =
		::dcool::service::detail_::SpscStackConfigAdaptor_<ConfigT_, ValueT_>
	;

	template <
		typename ValueT_, ::dcool::service::SpscStackConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct SpscStackChassis {
		private: using Self_ = SpscStackChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::service::SpscStackConfigAdaptor<Config, Value>;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Engine = ConfigAdaptor_::Engine;

		private: struct Node_ {
			Value value_;
			Node_* next_;
		};

		private: ::std::atomic<Node_*> m_top_;

		public: SpscStackChassis(Self_ const&) = delete;
		public: SpscStackChassis(Self_&&) = delete;
		public: auto operator =(Self_ const&) = delete;
		public: auto operator =(Self_&&) = delete;

		public: void initialize(Engine& engine_) {
		}

		public: void uninitialize(Engine& engine_) {
			Node_* current_ = this->loadTop_(::std::memory_order::relaxed);
			while (current_ != ::dcool::core::nullPointer) {
				Node_* next_ = current_->next_;
				::dcool::resource::destroyPointerByPoolFor(engine_.pool(), current_);
				current_ = next_;
			}
		}

		public: void relocateTo(Self_& other_) {
			Node_* top_ = this->m_top_.exchange(::dcool::core::nullPointer, ::std::memory_order::acq_rel);
			other_.m_top_.store(top_, ::std::memory_order::release);
		}

		private: template <::dcool::core::Boolean synchronizedScenarioC__> constexpr void pushNode_(Node_* newNode_) {
			if constexpr (synchronizedScenarioC__) {
				this->m_top_.store(newNode_, ::std::memory_order::relaxed);
			} else {
				while (
					!(
						this->m_top_.compare_exchange_weak(
							newNode_->next, newNode_, ::std::memory_order::acq_rel, ::std::memory_order::acquire
						)
					)
				) {
				}
			}
		}

		public: template <
			::dcool::core::Boolean synchronizedScenarioC__, typename... ArgumentTs__
		> constexpr void emplace(Engine& engine_, ArgumentTs__&&... parameters_) {
			Node_* newNode_ = ::dcool::resource::createPointerByPoolFor<Node_>(
				engine_.pool(),
				Node_{
					.value_ = Value(::dcool::core::forward<ArgumentTs__>(parameters_)...)
					.next_ = this->m_top_.load(synchronizedScenarioC__ ? ::std::memory_order::relaxed : ::std::memory_order::acquire);
				}
			);
			this->pushNode_<synchronizedScenarioC__>(newNode_);
		}

		public: template <
			typename... ArgumentTs__
		> constexpr void emplace(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplace<false>(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <
			::dcool::core::Boolean synchronizedScenarioC__ = false
		> constexpr void push(Engine& engine_, Value const& value_) {
			this->emplace<synchronizedScenarioC__>(engine_, value_);
		}

		public: template <
			::dcool::core::Boolean synchronizedScenarioC__ = false
		> constexpr void push(Engine& engine_, Value&& value_) {
			this->emplace<synchronizedScenarioC__>(engine_, ::dcool::core::move(value_));
		}

		public: template <
			::dcool::core::Boolean synchronizedScenarioC__ = false
		> constexpr auto popValue(Engine& engine_) -> ::dcool::core::Optional<Value> {
			Node_* poppedNode_ = this->m_top_.load(
				synchronizedScenarioC__ ? ::std::memory_order::relaxed : ::std::memory_order::acquire
			);
			if constexpr (synchronizedScenarioC__) {
				do {
					if (poppedNode_ == ::dcool::core::nullPointer) {
						return ::dcool::core::nullOptional;
					}
				} while (
					!(
						::dcool::core::Boolean popResult_ = this->m_top_.compare_exchange_weak(
							poppedNode_, poppedNode_->next_, ::std::memory_order::acq_rel, ::std::memory_order::acquire
						)
					)
				);
			} else {
				if (poppedNode_ == ::dcool::core::nullPointer) {
					return ::dcool::core::nullOptional;
				}
				this->m_top_.store(poppedNode_->next_, ::std::memory_order::relaxed);
			}
			::dcool::core::Optional<Value> poppedValue_;
			try {
				poppedValue_.emplace(::dcool::core::move(poppedNode_->value_));
			} catch (...) {
				// This node might not be pushed to its original position if not 'synchronizedScenarioC__'
				this->pushNode_<synchronizedScenarioC__>(poppedNode_);
				throw;
			}
			::dcool::resource::destroyPointerByPoolFor(engine_.pool());
			return poppedValue_;
		}
	};

	template <typename ValueT_, ::dcool::service::SpscStackConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct SpscStack {
		private: using Self_ = SpscStack<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::service::SpscStackChassis<Value, Config>;
		private: using ConfigAdaptor_ = ::dcool::service::SpscStackConfigAdaptor<Config, Value>;
		public: using Pool = Chassis::Pool;
		public: using Engine = Chassis::Engine;

		private: [[no_unique_address]] mutable Engine m_engine_;
		private: Chassis m_chassis_;

		public: constexpr SpscStack() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr ~SpscStack() noexcept {
			this->chassis().uninitialize(this->mutableEngine());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		private: constexpr auto mutableEngine() const noexcept -> Engine& {
			return this->m_engine_;
		}
	};
}

#endif
