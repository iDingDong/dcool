#ifndef DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_
#	define DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/variant.hpp>

namespace dcool::core {
	template <typename ValueT_> struct TaskResult {
		private: using Self_ = TaskResult<ValueT_>;
		public: using Value = ValueT_;

		private: struct AbortTag_ {
		};

		private: struct RetryTag_ {
		};

		private: ::dcool::core::DefaultVariant<AbortTag_, RetryTag_, Value> m_value_ = AbortTag_();

		public: constexpr TaskResult() noexcept = default;

		public: constexpr TaskResult(Value const& result_) noexcept(noexcept(Value(result_))): m_value_(result_) {
		}

		public: constexpr TaskResult(
			Value&& result_
		) noexcept(noexcept(Value(::dcool::core::move(result_)))): m_value_(::dcool::core::move(result_)) {
		}

		private: constexpr TaskResult(RetryTag_ tag_) noexcept: m_value_(tag_) {
		}

		private: constexpr TaskResult(AbortTag_ tag_) noexcept: m_value_(tag_) {
		}

		public: static constexpr auto makeRetry() noexcept -> Self_ {
			return Self_(RetryTag_());
		}

		public: static constexpr auto makeAbort() noexcept -> Self_ {
			return Self_(AbortTag_());
		}

		public: constexpr auto aborted() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<AbortTag_>();
		}

		public: constexpr auto retryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->m_value_.template holding<RetryTag_>();
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
	};
}

#endif
