#ifndef DCOOL_CONCURRENCY_THREAD_POOL_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_THREAD_POOL_HPP_INCLUDED_ 1

#	include <dcool/container.hpp>
#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>
#	include <dcool/utility.hpp>

#	include <atomic>
#	include <condition_variable>
#	include <mutex>
#	include <stdexcept>
#	include <thread>

namespace dcool::concurrency {
	namespace detail_ {
		template <::dcool::core::Complete ConfigT_> class ThreadPoolConfigAdaptor_ {
			private: using Self_ = ThreadPoolConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;

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
		};
	}

	template <typename T_> concept ThreadPoolConfig = requires {
		typename ::dcool::concurrency::detail_::ThreadPoolConfigAdaptor_<T_>;
	};

	template <
		::dcool::concurrency::ThreadPoolConfig ConfigT_
	> using ThreadPoolConfigAdaptor = ::dcool::concurrency::detail_::ThreadPoolConfigAdaptor_<ConfigT_>;

	template <::dcool::concurrency::ThreadPoolConfig ConfigT_ = ::dcool::core::Empty<>> class ThreadPoolChassis {
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::detail_::ThreadPoolConfigAdaptor_<Config>;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Engine = ConfigAdaptor_::Engine;

		private: struct ThreadsConfig_ {
			static constexpr ::dcool::core::Boolean stuffed = true;
			using Pool = ThreadPoolChassis::Pool;
			using Engine = ThreadPoolChassis::Engine;
		};

		private: struct TaskConfig_ {
			using Pool = ThreadPoolChassis::Pool;
			using Engine = ThreadPoolChassis::Engine;
			static constexpr ::dcool::core::Boolean copyable = false;
		};

		private: struct TasksConfig_ {
			using Pool = ThreadPoolChassis::Pool;
			using Engine = ThreadPoolChassis::Engine;
		};

		private: using Task_ = ::dcool::utility::FunctionChassis<void () noexcept, TaskConfig_>;
		private: using Threads_ = ::dcool::container::ListChassis<::std::jthread, ThreadsConfig_>;
		private: using Tasks_ = ::dcool::container::BidirectionalLinkedChassis<Task_, TasksConfig_>;

		private: ::std::mutex m_mutex_;
		private: ::std::condition_variable m_blocker_;
		private: Threads_ m_threads_;
		private: Tasks_ m_tasks_;
		private: ::dcool::core::Length m_deparallelRequestCount_;
		private: ::dcool::core::Boolean m_operational_;

		private: static void taskConsumer_(ThreadPoolChassis& self_, Engine& engine_) noexcept {
			::std::unique_lock locker_(self_.m_mutex_);
			for (; ; ) {
				if (self_.m_deparallelRequestCount_ > 0) {
					--(self_.m_deparallelRequestCount_);
					break;
				}
				if (!(self_.m_tasks_.vacant(engine_))) {
					Task_ task_ = ::dcool::core::move(self_.m_tasks_.front(engine_));
					self_.m_tasks_.popFront(engine_);
					locker_.unlock();
					task_.invokeSelf(engine_);
					task_.uninitialize(engine_);
					locker_.lock();
				} else {
					if (!(self_.m_operational_)) {
						break;
					}
					self_.m_blocker_.wait(locker_);
				}
			}
		}

		public: constexpr void initialize(
			Engine& engine_, ::dcool::core::Length parallelLimit_ = ::std::jthread::hardware_concurrency()
		) {
			this->m_deparallelRequestCount_ = 0;
			this->m_operational_ = true;
			this->m_threads_.initialize(engine_);
			try {
				this->m_tasks_.initialize(engine_);
				try {
					this->enparallel(engine_, parallelLimit_);
				} catch (...) {
					this->m_tasks_.uninitialize(engine_);
					throw;
				}
			} catch (...) {
				this->m_threads_.uninitialize(engine_);
				throw;
			}
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			{
				::dcool::core::PhoneyLockGuard guard_(this->m_mutex_);
				this->m_operational_ = false;
			}
			this->m_blocker_.notify_all();
			this->m_threads_.uninitialize(engine_);
			this->m_tasks_.uninitialize(engine_);
		}

		public: template <typename TaskT__> constexpr void enqueTask(Engine& engine_, TaskT__&& task_) {
			::std::lock_guard locker_(this->m_mutex_);
			if (!(this->m_operational_)) {
				throw ::std::runtime_error("Thread pool no longer operational.");
			}
			this->m_tasks_.emplaceBack(engine_);
			try {
				this->m_tasks_.back(engine_).initialize(engine_, ::dcool::core::forward<TaskT__>(task_));
			} catch (...) {
				this->m_tasks_.popBack(engine_);
				throw;
			}
			this->m_blocker_.notify_one();
		}

		public: constexpr void enparallel(Engine& engine_, ::dcool::core::Length count_ = 1) {
			while (count_ > 0) {
				this->m_threads_.emplaceBack(engine_, taskConsumer_, ::std::ref(*this), ::std::ref(engine_));
				--count_;
			}
		}

		public: constexpr void deparallel(Engine& engine_, ::dcool::core::Length count_ = 1) noexcept {
			if (count_ == 0) {
				return;
			}
			{
				::dcool::core::PhoneyLockGuard guard_(this->m_mutex_);
				DCOOL_CORE_ASSERT(this->m_threads_.length(engine_) - this->m_deparallelRequestCount_ >= count_);
				this->m_deparallelRequestCount_ += count_;
			}
			if (count_ > 1) {
				this->m_blocker_.notify_all();
			} else {
				this->m_blocker_.notify_one();
			}
		}
	};

	template <typename ConfigT_ = ::dcool::core::Empty<>> class ThreadPool {
		private: using Self_ = ThreadPoolChassis<ConfigT_>;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::concurrency::ThreadPoolChassis<Config>;
		public: using Engine = Chassis::Engine;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr explicit ThreadPool(::dcool::core::Length parallelLimit_ = ::std::jthread::hardware_concurrency()) {
			this->chassis().initialize(this->engine(), parallelLimit_);
		}

		public: constexpr ~ThreadPool() noexcept {
			this->chassis().uninitialize(this->engine());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		public: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: template <typename TaskT__> constexpr void enqueTask(TaskT__&& task_) {
			this->chassis().enqueTask(this->engine(), ::dcool::core::forward<TaskT__>(task_));
		}

		public: constexpr void enparallel(::dcool::core::Length count_ = 1) {
			this->chassis().enparallel(this->engine(), count_);
		}

		public: constexpr void deparallel(::dcool::core::Length count_ = 1) noexcept {
			this->chassis().deparallel(this->engine(), count_);
		}
	};
}

#endif
