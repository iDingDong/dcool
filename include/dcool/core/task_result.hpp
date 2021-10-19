#ifndef DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_
#	define DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/variant.hpp>

namespace dcool::core {
	template <typename ValueT_> struct TaskResult {
		private: using Self_ = TaskResult<ValueT_>;
		public: using Value = ValueT_;

		private: struct AbortTag_ {
		};

		private: struct InstantRetryTag_ {
		};

		private: struct DelayedRetryTag_ {
		};

		private: ::dcool::core::DefaultVariant<AbortTag_, InstantRetryTag_, DelayedRetryTag_, Value> m_value_ = AbortTag_();

		public: constexpr TaskResult() noexcept = default;

		public: constexpr TaskResult(Value const& result_) noexcept(noexcept(Value(result_))): m_value_(result_) {
		}

		public: constexpr TaskResult(
			Value&& result_
		) noexcept(noexcept(Value(::dcool::core::move(result_)))): m_value_(::dcool::core::move(result_)) {
		}

		private: constexpr TaskResult(InstantRetryTag_ tag_) noexcept: m_value_(tag_) {
		}

		private: constexpr TaskResult(DelayedRetryTag_ tag_) noexcept: m_value_(tag_) {
		}

		private: constexpr TaskResult(AbortTag_ tag_) noexcept: m_value_(tag_) {
		}

		public: static constexpr auto makeInstantRetry() noexcept -> Self_ {
			return Self_(InstantRetryTag_());
		}

		public: static constexpr auto makeDelayedRetry() noexcept -> Self_ {
			return Self_(DelayedRetryTag_());
		}

		public: static constexpr auto makeAbort() noexcept -> Self_ {
			return Self_(AbortTag_());
		}

		public: constexpr auto aborted() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<AbortTag_>();
		}

		public: constexpr auto instantRetryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<InstantRetryTag_>();
		}

		public: constexpr auto delayedRetryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<DelayedRetryTag_>();
		}

		public: constexpr auto retryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->instantRetryRequested() || this->delayedRetryRequested();
		}

		public: constexpr auto done() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<Value>();
		}

		public: constexpr auto accessValue() const noexcept -> Value const& {
			DCOOL_CORE_ASSERT(this->done());
			return this->m_value_.template access<Value>();
		}

		public: constexpr auto accessValue() noexcept -> Value& {
			DCOOL_CORE_ASSERT(this->done());
			return this->m_value_.template access<Value>();
		}

		public: template <typename TaskResultT__> static constexpr auto transformFrom(
			TaskResultT__&& other_
		) -> Self_ {
			return transformFrom(::dcool::core::forward<TaskResultT__>(other_), ::dcool::core::argumentReturner);
		}

		public: template <typename TaskResultT__, typename TransformerT__> static constexpr auto transformFrom(
			TaskResultT__&& other_, TransformerT__&& transformer_
		) -> Self_ {
			if (other_.aborted()) {
				return Self_::makeAbort();
			}
			if (other_.instantRetryRequested()) {
				return Self_::makeInstantRetry();
			}
			if (other_.delayedRetryRequested()) {
				return Self_::makeDelayedRetry();
			}
			return Self_(
				Value(
					::dcool::core::invoke(
						::dcool::core::forward<TransformerT__>(transformer_),
						::dcool::core::moveIf<::dcool::core::RvalueReference<TaskResultT__&&>>(other_.accessValue())
					)
				)
			);
		}

		public: template <typename TransformerT__> constexpr auto transformSelfBy(TransformerT__&& transformer_) const& {
			using TargetValue_ = ::dcool::core::DecayedType<
				decltype(::dcool::core::invoke(::dcool::core::forward<TransformerT__>(transformer_), this->accessValue()))
			>;
			using Target_ = ::dcool::core::TaskResult<TargetValue_>;
			return Target_::transformFrom(*this, ::dcool::core::forward<TransformerT__>(transformer_));
		}

		public: template <typename TransformerT__> constexpr auto transformSelfBy(TransformerT__&& transformer_)&& {
			using TargetValue_ = ::dcool::core::DecayedType<
				decltype(
					::dcool::core::invoke(::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::move(this->accessValue()))
				)
			>;
			using Target_ = ::dcool::core::TaskResult<TargetValue_>;
			return Target_::transformFrom(::dcool::core::move(*this), ::dcool::core::forward<TransformerT__>(transformer_));
		}
	};
}

#endif
