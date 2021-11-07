#ifndef DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_
#	define DCOOL_CORE_TASK_RESULT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/variant.hpp>

namespace dcool::core {
	namespace detail_ {
		template <
			::dcool::core::NonVoid T1_,
			::dcool::core::NonVoid T2_,
			typename MayBeVoidT_
		> struct TaskResultUnderlying_ {
			using Result_ = ::dcool::core::DefaultVariant<T1_, T2_, MayBeVoidT_>;
		};

		template <::dcool::core::NonVoid T1_, ::dcool::core::NonVoid T2_> struct TaskResultUnderlying_<T1_, T2_, void> {
			using Result_ = ::dcool::core::DefaultVariant<T1_, T2_>;
		};
	}

	template <typename ValueT_> struct TaskResult {
		private: using Self_ = TaskResult<ValueT_>;
		public: using Value = ValueT_;

		private: static constexpr ::dcool::core::Boolean nonVoid_ = ::dcool::core::NonVoid<Value>;

		private: struct InstantRetryTag_ {
		};

		private: struct DelayedRetryTag_ {
		};

		private: using Underlying_ = ::dcool::core::detail_::TaskResultUnderlying_<
			InstantRetryTag_, DelayedRetryTag_, Value
		>::Result_;
		private: Underlying_ m_underlying_;

		public: constexpr TaskResult() noexcept = default;

		public: constexpr TaskResult(Value const& result_) noexcept(noexcept(Value(result_))): m_underlying_(result_) {
		}

		public: constexpr TaskResult(
			Value&& result_
		) noexcept(noexcept(Value(::dcool::core::move(result_)))): m_underlying_(::dcool::core::move(result_)) {
		}

		private: constexpr TaskResult(InstantRetryTag_ tag_) noexcept: m_underlying_(tag_) {
		}

		private: constexpr TaskResult(DelayedRetryTag_ tag_) noexcept: m_underlying_(tag_) {
		}

		public: static constexpr auto makeInstantRetry() noexcept -> Self_ {
			return Self_(InstantRetryTag_());
		}

		public: static constexpr auto makeDelayedRetry() noexcept -> Self_ {
			return Self_(DelayedRetryTag_());
		}

		public: static constexpr auto makeAbort() noexcept -> Self_ {
			return Self_();
		}

		public: constexpr auto aborted() const noexcept -> ::dcool::core::Boolean {
			return !(this->m_underlying_.valid());
		}

		public: constexpr auto instantRetryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.template holding<InstantRetryTag_>();
		}

		public: constexpr auto delayedRetryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.template holding<DelayedRetryTag_>();
		}

		public: constexpr auto retryRequested() const noexcept -> ::dcool::core::Boolean {
			return this->instantRetryRequested() || this->delayedRetryRequested();
		}

		public: constexpr auto done() const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.template holding<Value>();
		}

		public: constexpr auto accessValue() const noexcept -> Value const& requires (nonVoid_) {
			DCOOL_CORE_ASSERT(this->done());
			return this->m_underlying_.template access<Value>();
		}

		public: constexpr auto accessValue() noexcept -> Value& requires (nonVoid_) {
			DCOOL_CORE_ASSERT(this->done());
			return this->m_underlying_.template access<Value>();
		}

		public: template <typename TaskResultT__> static constexpr auto transformFrom(
			TaskResultT__&& other_
		) noexcept(
			noexcept(transformFrom(::dcool::core::forward<TaskResultT__>(other_), ::dcool::core::argumentReturner))
		) -> Self_ {
			return transformFrom(::dcool::core::forward<TaskResultT__>(other_), ::dcool::core::argumentReturner);
		}

		public: template <typename TaskResultT__, typename TransformerT__> static constexpr auto transformFrom(
			TaskResultT__&& other_, TransformerT__&& transformer_
		) noexcept(
			noexcept(
				Self_(
					Value(
						::dcool::core::invoke(
							::dcool::core::forward<TransformerT__>(transformer_),
							::dcool::core::moveIf<::dcool::core::RvalueReference<TaskResultT__&&>>(other_.accessValue())
						)
					)
				)
			)
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
