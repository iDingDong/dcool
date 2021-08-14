#ifndef DCOOL_SERVICE_TIMER_HPP_INCLUDED_
#	define DCOOL_SERVICE_TIMER_HPP_INCLUDED_ 1

#	include <dcool/container.hpp>
#	include <dcool/core.hpp>
#	include <dcool/coroutine.hpp>
#	include <dcool/utility.hpp>

#	include <chrono>
#	include <condition_variable>
#	include <functional>
#	include <thread>
#	include <mutex>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::service::detail_, HasValueMinimalSizeForTimer_, extractedMinimalSizeValueForTimer_, minimalSize
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::service::detail_, HasTypeDurationForTimer_, ExtractedDurationTypeForTimer_, minimalSize
)

namespace dcool::service {
	enum class TimerActionResult {
		done,
		reset,
		repeat
	};

	namespace detail_ {
		template <typename ConfigT_> class QuickTimerConfigAdaptor_ {
			private: using Self_ = QuickTimerConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: static constexpr ::dcool::core::Boolean minimalSize =
				::dcool::service::detail_::extractedMinimalSizeValueForTimer_<Config>(false)
			;
		};
	}

	template <typename T_> concept QuickTimerConfig = requires {
		typename ::dcool::service::detail_::QuickTimerConfigAdaptor_<T_>;
	};

	template <
		::dcool::service::QuickTimerConfig ConfigT_
	> using QuickTimerConfigAdaptor = ::dcool::service::detail_::QuickTimerConfigAdaptor_<ConfigT_>;

	template <::dcool::service::QuickTimerConfig ConfigT_ = ::dcool::core::Empty<>> class QuickTimer {
		private: using Self_ = QuickTimer<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::service::QuickTimerConfigAdaptor<Config>;
		public: using ActionResult = ::dcool::service::TimerActionResult;
		public: static constexpr ::dcool::core::Boolean minimalSize = ConfigAdaptor_::minimalSize;
		private: static constexpr ::dcool::core::Boolean quickStop_ = !minimalSize;

		private: struct QuickStopHelper_ {
			mutable ::std::condition_variable blocker_;
			mutable ::std::mutex mutex_;
		};

		private: [[no_unique_address]] ::dcool::core::StaticOptional<QuickStopHelper_, quickStop_> m_helper_;
		private: ::std::jthread m_performer_;

		public: constexpr QuickTimer() noexcept = default;

		public: template <typename DurationT__, typename ActionT__> constexpr QuickTimer(
			DurationT__ duration_, ActionT__&& action_
		): m_performer_(this->makePerformer_(duration_, ::dcool::core::forward<ActionT__>(action_))) {
		}

		public: QuickTimer(Self_ const&) = delete;
		public: QuickTimer(Self_&&) = delete;

		private: template <typename DurationT__, typename ActionT__> constexpr auto makePerformer_(
			DurationT__ duration_, ActionT__ action_
		) -> ::std::jthread {
			return ::std::jthread(
				[](::std::stop_token token_, Self_& timer_, auto initialTime_, DurationT__ duration_, ActionT__& action_) {
					auto nextTime_ = initialTime_ + duration_;
					for (; ; ) {
						if constexpr (quickStop_) {
							::std::unique_lock locker_(timer_.m_helper_.value.mutex_);
							::dcool::core::Boolean stopped_ = timer_.m_helper_.value.blocker_.wait_until(
								locker_,
								nextTime_,
								[&token_]() {
									return token_.stop_requested();
								}
							);
							if (stopped_) {
								return;
							}
						} else {
							if (token_.stop_requested()) {
								return;
							}
							::std::this_thread::sleep_until(nextTime_);
							if (token_.stop_requested()) {
								return;
							}
						}
						ActionResult result_ = ::dcool::core::invoke(action_);
						switch (result_) {
							case ActionResult::done:
							return;
							case ActionResult::reset:
							nextTime_ = ::std::chrono::steady_clock::now();
							[[fallthrough]];
							case ActionResult::repeat:
							nextTime_ += duration_;
							break;
						}
					}
				},
				::std::ref(*this),
				::std::chrono::steady_clock::now(),
				duration_,
				::std::ref(action_)
			);
		}

		public: constexpr ~QuickTimer() noexcept {
			this->stop();
		}

		public: constexpr void waitUntilDone() {
			this->m_performer_.join();
		}

		public: constexpr void stop() noexcept {
			static_cast<void>(this->m_performer_.request_stop());
			if constexpr (quickStop_) {
				this->m_helper_.value.blocker_.notify_one();
			}
		}
	};

	namespace detail_ {
		template <typename ConfigT_> class TimerConfigAdaptor_ {
			private: using Self_ = TimerConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Duration = ::dcool::service::detail_::ExtractedDurationTypeForTimer_<
				Config, ::std::chrono::steady_clock::duration
			>;
		};
	}

	template <typename T_> concept TimerConfig = requires {
		typename ::dcool::service::detail_::TimerConfigAdaptor_<T_>;
	};
}

#endif
