#ifndef DCOOL_CONCURRENCY_THREAD_POOL_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_THREAD_POOL_HPP_INCLUDED_ 1

#	include <dcool/container.hpp>
#	include <dcool/core.hpp>
#	include <dcool/coroutine.hpp>
#	include <dcool/resource.hpp>
#	include <dcool/utility.hpp>

#	include <atomic>
#	include <condition_variable>
#	include <mutex>
#	include <stop_token>
#	include <thread>
#	include <utility>

namespace dcool::concurrency {
	template <typename ConfigT_ = ::dcool::core::Empty<>> class ThreadPoolChassis {
		private: using Self_ = ThreadPoolChassis<ConfigT_>;
		public: using Config = ConfigT_;

		private: using Pool = ::dcool::resource::DefaultPool;
		private: using Thread_ = ::std::jthread;

		private: struct Engine {
			public: Pool partPool;

			public: constexpr auto pool() -> Pool {
				return this->partPool;
			}
		};

		private: struct ThreadsConfig_ {
			static constexpr ::dcool::core::Boolean stuffed = true;
			using Pool = Pool;
			using Engine = Engine;
		};

		private: struct TaskConfig_ {
			using Pool = Pool;
			using Engine = Engine;
		};

		private: struct TasksConfig_ {
			static constexpr ::dcool::core::Boolean circular = true;
			using Pool = Pool;
			using Engine = Engine;
		};

		private: using Threads_ = ::dcool::container::ListChassis<Thread_, ThreadsConfig_>;
		private: using Task_ = ::dcool::utility::FunctionChassis<void() noexcept, TaskConfig_>;
		private: using Tasks_ = ::dcool::container::ListChassis<Task_, TasksConfig_>;

		private: Threads_ m_threads_;
		private: Tasks_ m_tasks_;
		private: mutable ::std::condition_variable m_consumerBlocker_;
		private: mutable ::std::condition_variable m_destructorBlocker_;
		private: mutable ::std::mutex m_mutex_;
		private: ::dcool::core::Length m_busyCount_;
		private: mutable ::std::condition_variable m_busyCountBlocker_;
		private: mutable ::std::mutex m_busyCountMutex_;

		public: constexpr void initialize(Engine& engine_, ::dcool::core::Length parallelLimit_ = Thread_::hardware_concurrency()) {
			this->m_threads_.initialize(engine_);
			try {
				this->m_tasks_.initialize(engine_);
				try {
					for (::dcool::core::Length i = 0; i < parallelLimit_; ++i) {
						this->m_threads_.emplaceBack(engine_, [](
							::std::stop_token stopToken_, Self_& threadPool_, Engine& engine_
						) noexcept {
							while (!(stopToken_.stop_requested())) [[likely]] {
								Task_ fetched_;
								::dcool::core::Boolean isLastTask_;
								{
									::std::unique_lock locker_(threadPool_.m_mutex_);
									while (threadPool_.m_tasks_.vacant(engine_)) {
										if (stopToken_.stop_requested()) [[unlikely]] {
											return;
										}
										threadPool_.m_consumerBlocker_.wait(locker_);
									}
									{
										::std::lock_guard busyCountLocker_(threadPool_.m_busyCountMutex_);
										++(threadPool_.m_busyCount_);
									}
									threadPool_.m_tasks_.front(engine_).relocateTo<true>(engine_, engine_, fetched_);
									// This can be done without throw
									threadPool_.m_tasks_.popFront(engine_);
									isLastTask_ = threadPool_.m_tasks_.vacant(engine_);
								}
								if (isLastTask_) {
									threadPool_.m_destructorBlocker_.notify_one();
								}
								// The task is already marked to be noexcept
								fetched_.invokeSelf(engine_);
								::dcool::core::Boolean isLastNonIdle_;
								{
									::std::lock_guard busyCountLocker_(threadPool_.m_busyCountMutex_);
									--(threadPool_.m_busyCount_);
									isLastNonIdle_ = threadPool_.m_busyCount_ == 0;
								}
								if (isLastNonIdle_) {
									this->m_busyCountBlocker_.notify_all();
								}
								fetched_.uninitialize(engine_);
							}
						});
					}
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
			static_cast<void>(this->waitUntilNoPending(engine_));
			for (auto& thread_: this->m_threads_) {
				static_cast<void>(thread_.request_stop());
			}
			this->m_consumerBlocker_.notify_all();
			this->m_threads_.uninitialize(engine_);
		}

		public: constexpr auto waitUntilNoPending(Engine& engine_) const noexcept {
			::std::unique_lock locker_(this->m_mutex_);
			this->m_destructorBlocker_.wait(locker_, [this, &engine_]() noexcept -> ::dcool::core::Boolean {
				return this->m_tasks_.vacant(engine_);
			});
			return locker_;
		}

		public: constexpr auto waitUntilIdle(Engine& engine_) const noexcept {
			auto result_ = ::std::pair{ this->waitUntilNoPending(engine_), ::std::unique_lock(this->m_busyCountMutex_) };
			this->m_busyCountBlocker_.wait(result_.second, [this]() noexcept -> ::dcool::core::Boolean {
				return this->m_busyCount_ == 0;
			});
			return result_;
		}

		public: template <typename TaskT__> constexpr void enqueTask(Engine& engine_, TaskT__&& task_) noexcept {
			::std::lock_guard locker_(this->m_mutex_);
			this->m_tasks_.emplaceBack(engine_);
			try {
				this->m_tasks_.back(engine_).initialize(engine_, ::dcool::core::forward<TaskT__>(task_));
			} catch (...) {
				this->m_tasks_.popBack(engine_);
				throw;
			}
		}
	};
}

#endif
