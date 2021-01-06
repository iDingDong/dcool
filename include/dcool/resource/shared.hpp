#ifndef DCOOL_RESOURCE_SHARED_HPP_INCLUDED_
#	define DCOOL_RESOURCE_SHARED_HPP_INCLUDED_ 1

#	include <dcool/resource/dismissor.hpp>
#	include <dcool/resource/unique_pointer.hpp>

#	include <dcool/core.hpp>

#	include <limits>
#	include <type_traits>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_, HasValueAtomicallyCounted_, extractedAtomicallyCountedValue_, atomicallyCounted
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_, HasValueMaxStrongReferenceCount_, extractedMaxStrongReferenceCountValue_, maxStrongReferenceCount
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::resource::detail_, HasValueMaxWeakReferenceCount_, extractedMaxWeakReferenceCountValue_, maxWeakReferenceCount
)

namespace dcool::resource {
	template <typename T_> concept IntrusiveStronglyReferenceable = requires (T_ referenced_) {
		referenced_.recordStrongReference();
		referenced_.unrecordStrongReference();
	};

	template <typename T_> concept IntrusivelyShareable =
		::dcool::resource::IntrusiveStronglyReferenceable<T_> &&
		requires (T_ referenced_) {
			{ referenced_.expiredForStrongReference() } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
			{ referenced_.lockForStrongReference() } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
			referenced_.recordWeakReference();
			referenced_.unrecordWeakReference();
		}
	;

	template <::dcool::resource::IntrusiveStronglyReferenceable ValueT_> struct IntrusiveStrong {
		private: using Self_ = IntrusiveStrong<ValueT_>;
		public: using Value = ValueT_;

		public: using Reference = Value&;
		public: using Pointer = Value*;
		public: template <::dcool::resource::IntrusivelyShareable> friend struct IntrusiveWeak;

		private: Pointer m_pointer_ = ::dcool::core::nullPointer;

		public: constexpr IntrusiveStrong() noexcept = default;

		public: constexpr IntrusiveStrong(::dcool::core::NullPointer nullPointer_) noexcept: Self_() {
		}

		public: constexpr explicit IntrusiveStrong(Pointer rawPointer_) noexcept: m_pointer_(rawPointer_) {
			if (this->valid()) {
				this->dereferenceSelf().recordStrongReference();
			}
		}

		private: struct LockTag_ {
		};

		private: constexpr explicit IntrusiveStrong(LockTag_, Pointer rawPointer_) noexcept: m_pointer_(rawPointer_) {
			static_assert(
				::dcool::resource::IntrusivelyShareable<Value>, "Only intrusively shareable object can be lock for strong reference."
			);
			if (this->valid()) {
				if (!(this->dereferenceSelf().lockForStrongReference())) {
					this->m_pointer_ = ::dcool::core::nullPointer;
				}
			}
		}

		public: constexpr IntrusiveStrong(Self_ const& other_) noexcept: Self_(other_.rawPointer()) {
		}

		public: constexpr IntrusiveStrong(Self_&& other_) noexcept: m_pointer_(other_.release_()) {
		}

		private: constexpr void destructSelf_() noexcept {
			if (this->valid()) {
				this->dereferenceSelf().unrecordStrongReference();
			}
		}

		public: constexpr ~IntrusiveStrong() noexcept {
			this->destructSelf_();
		}

		public: constexpr auto operator =(::dcool::core::NullPointer nullPointer_) noexcept {
			this->reset();
		}

		public: constexpr auto operator =(Pointer rawPointer_) noexcept -> Self_& {
			Self_ middleMan_(rawPointer_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			this->swapWith(other_);
			return *this;
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			::dcool::core::intelliswap(this->m_pointer_, other_.m_pointer_);
		}

		public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->rawPointer() != ::dcool::core::nullPointer;
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Reference {
			return ::dcool::core::dereference(this->rawPointer());
		}

		public: constexpr auto operator *() const noexcept -> Reference {
			return this->dereferenceSelf();
		}

		public: constexpr auto rawPointer() const noexcept -> Pointer {
			return this->m_pointer_;
		}

		public: constexpr auto operator ->() const noexcept -> Pointer {
			return this->rawPointer();
		}

		private: constexpr auto release_() noexcept -> Pointer {
			Pointer result_ = this->pointer();
			this->m_pointer_ = ::dcool::core::nullPointer;
			return result_;
		}

		public: constexpr void reset() noexcept {
			this->destructSelf_();
			this->m_pointer_ = ::dcool::core::nullPointer;
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <::dcool::resource::IntrusivelyShareable ValueT_> struct IntrusiveWeak {
		private: using Self_ = IntrusiveWeak<ValueT_>;
		public: using Value = ValueT_;

		public: using Strong = ::dcool::resource::IntrusiveStrong<Value>;
		public: using Reference = Value&;
		public: using Pointer = Value*;

		private: Pointer m_pointer_ = ::dcool::core::nullPointer;

		public: constexpr IntrusiveWeak() noexcept = default;

		private: constexpr explicit IntrusiveWeak(Pointer rawPointer_) noexcept: m_pointer_(rawPointer_) {
			if (this->valid()) {
				this->dereferenceSelf_().recordWeakReference();
			}
		}

		public: constexpr IntrusiveWeak(Strong const& other_) noexcept: Self_(other_.rawPointer()) {
		}

		public: constexpr IntrusiveWeak(Self_ const& other_) noexcept: Self_(other_.rawPointer_()) {
		}

		public: constexpr IntrusiveWeak(Self_&& other_) noexcept: m_pointer_(other_.release_()) {
		}

		private: constexpr void destructSelf_() noexcept {
			if (this->valid()) {
				this->dereferenceSelf_().recordWeakReference();
			}
		}

		public: constexpr ~IntrusiveWeak() noexcept {
			this->destructSelf_();
		}

		public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			this->swapWith(other_);
			return *this;
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			::dcool::core::intelliswap(this->m_pointer_, other_.m_pointer_);
		}

		public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->rawPointer_() != ::dcool::core::nullPointer;
		}

		private: constexpr auto dereferenceSelf_() const noexcept -> Reference {
			return ::dcool::core::dereference(this->rawPointer());
		}

		private: constexpr auto rawPointer_() const noexcept -> Pointer {
			return this->m_pointer_;
		}

		private: constexpr auto release_() noexcept -> Pointer {
			Pointer result_ = this->pointer();
			this->m_pointer_ = ::dcool::core::nullPointer;
			return result_;
		}

		public: constexpr void reset() noexcept {
			this->destructSelf_();
			this->m_pointer_ = ::dcool::core::nullPointer;
		}

		public: constexpr auto lock() const noexcept -> Strong {
			return Strong(typename Strong::LockTag_(), this->rawPointer());
		}
	};

	namespace detail_ {
		template <
			typename StrongCountUnderlyingT_, typename WeakCountUnderlyingT_, ::dcool::core::CounterScenario counterScenarioC_
		> class SharedCounter_ {
			private: using Self_ = SharedCounter_<StrongCountUnderlyingT_, WeakCountUnderlyingT_, counterScenarioC_>;
			public: using StrongCountUnderlying = StrongCountUnderlyingT_;
			public: using WeakCountUnderlying = WeakCountUnderlyingT_;
			private: static constexpr ::dcool::core::CounterScenario counterScenario_ = counterScenarioC_;

			private: struct StrongCounterConfig_ {
				using Underlying = StrongCountUnderlying;
				static constexpr ::dcool::core::CounterScenario scenario = counterScenario_;
			};

			private: struct WeakCounterConfig_ {
				using Underlying = WeakCountUnderlying;
				static constexpr ::dcool::core::CounterScenario scenario = counterScenario_;
			};

			public: using StrongCounter = ::dcool::core::Counter<StrongCounterConfig_>;
			public: using WeakCounter = ::dcool::core::Counter<WeakCounterConfig_>;

			public: StrongCounter strong;
			public: WeakCounter weak;
		};

		template <
			typename StrongCountUnderlyingT_, ::dcool::core::CounterScenario counterScenarioC_
		> class SharedCounter_<StrongCountUnderlyingT_, void, counterScenarioC_> {
			private: using Self_ = SharedCounter_<StrongCountUnderlyingT_, void, counterScenarioC_>;
			public: using StrongCountUnderlying = StrongCountUnderlyingT_;
			public: using WeakCountUnderlying = void;
			private: static constexpr ::dcool::core::CounterScenario counterScenario_ = counterScenarioC_;

			private: struct StrongCounterConfig_ {
				using Underlying = StrongCountUnderlying;
				static constexpr ::dcool::core::CounterScenario scenario = counterScenario_;
			};

			public: using StrongCounter = ::dcool::core::Counter<StrongCounterConfig_>;

			public: StrongCounter strong;
		};

		template <
			typename StrongCountUnderlyingT_, typename WeakCountUnderlyingT_, ::dcool::core::CounterScenario counterScenarioC_
		> constexpr auto noWeakCount_(
			SharedCounter_<StrongCountUnderlyingT_, WeakCountUnderlyingT_, counterScenarioC_> const& sharedCounter_
		) noexcept {
			return sharedCounter_.weak.value() == 0;
		}

		template <typename StrongCountUnderlyingT_, ::dcool::core::CounterScenario counterScenarioC_> constexpr auto noWeakCount_(
			SharedCounter_<StrongCountUnderlyingT_, void, counterScenarioC_> const& sharedCounter_
		) noexcept {
			return true;
		}

		template <typename ConfigT_, typename ValueT_> class SharedAgentConfigAdaptor_ {
			private: using Self_ = SharedAgentConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using Dismissor = ::dcool::resource::ExtractedDismissorType<Config, ::dcool::resource::DefaultDestroyer>;
			public: static constexpr ::dcool::core::Boolean atomicallyCounted =
				::dcool::resource::detail_::extractedAtomicallyCountedValue_<Config>(false)
			;
			private: static constexpr ::dcool::core::UnsignedMaxInteger maxStrongReferenceCount_ =
				::dcool::resource::detail_::extractedMaxStrongReferenceCountValue_<Config>(
					static_cast<::dcool::core::UnsignedMaxInteger>(::std::numeric_limits<::dcool::core::Length>::max())
				)
			;
			private: static constexpr ::dcool::core::UnsignedMaxInteger maxWeakReferenceCount_ =
				::dcool::resource::detail_::extractedMaxWeakReferenceCountValue_<Config>(
					static_cast<::dcool::core::UnsignedMaxInteger>(0)
				)
			;

			public: using StrongCountUnderlying = ::dcool::core::IntegerType<maxStrongReferenceCount_>;
			public: using WeakCountUnderlying =
				::std::conditional<(maxWeakReferenceCount_ > 0), ::dcool::core::IntegerType<maxWeakReferenceCount_>, void
			>;
			private: static constexpr ::dcool::core::CounterScenario counterScenario_ = (
				atomicallyCounted ? ::dcool::core::CounterScenario::synchronized : ::dcool::core::CounterScenario::logicDependent
			);

			public: using Counters = ::dcool::resource::detail_::SharedCounter_<
				StrongCountUnderlying, WeakCountUnderlying, counterScenario_
			>;

			public: struct Engine {
				[[no_unique_address]] Dismissor dismissor;
			};
		};
	}

	template <typename T_, typename ValueT_> concept SharedAgentConfig = ::dcool::resource::DismissorFor<
		typename ::dcool::resource::detail_::SharedAgentConfigAdaptor_<T_, ValueT_>::Dismissor, ValueT_
	>;

	template <
		typename ConfigT_, typename ValueT_
	> requires ::dcool::resource::SharedAgentConfig<ConfigT_, ValueT_> using SharedAgentConfigAdaptor =
		::dcool::resource::detail_::SharedAgentConfigAdaptor_<ConfigT_, ValueT_>
	;

	template <
		typename ValueT_, ::dcool::resource::SharedAgentConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> class SharedAgentChassis {
		private: using Self_ = SharedAgentChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::resource::SharedAgentConfigAdaptor<Config, Value>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Dismissor = ConfigAdaptor_::Dismissor;
		public: using Counters = ConfigAdaptor_::Counters;
		public: friend Dismissor;

		private: Value* m_value_;
		private: Counters m_counters_;

		public: constexpr SharedAgentChassis() noexcept = default;
		public: SharedAgentChassis(Self_ const&) = delete;
		public: SharedAgentChassis(Self_&&) = delete;
		public: constexpr ~SharedAgentChassis() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		public: constexpr void initialize(Engine& engine_, Value& value_) noexcept {
			this->m_value_ = ::dcool::core::addressOf(value_);
		}

		private: constexpr void uninitialize_(ConfigAdaptor_::Dismissor& dismissor_) noexcept {
			dismissor_(this->value());
		}

		private: constexpr void uninitialize_(Engine& engine_) noexcept {
			this->uninitialize_(engine_.dismissor);
		}

		public: constexpr auto counters(Engine& engine_) const noexcept -> Counters const& {
			return this->m_counters_;
		}

		private: constexpr auto counters(Engine& engine_) noexcept -> Counters& {
			return this->m_counters_;
		}

		public: constexpr auto value(Engine& engine_) const noexcept -> Value& {
			return *(this->m_value_);
		}

		public: constexpr void recordStrongReference(Engine& engine_) noexcept {
			this->counters(engine_).strong.increment();
		}

		public: constexpr auto unrecordStrongReferenceToDestroy(Engine& engine_) noexcept -> ::dcool::core::Boolean {
			if (this->counters(engine_).strong.decrement() == 0) {
				this->chassis().uninitialize_(this->engine());
				if (::dcool::resource::detail_::noWeakCount_(this->counters(engine_))) {
					return true;
				}
			}
			return false;
		}

		public: constexpr auto strongReferenceCount(Engine& engine_) noexcept -> ConfigAdaptor_::StrongCountUnderlying {
			return this->counters(engine_).strong.value();
		}

		public: constexpr auto expiredForStrongReference(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->strongReferenceCount(engine_) == 0;
		}

		public: constexpr auto lockForStrongReference(Engine& engine_) noexcept -> ::dcool::core::Boolean {
			auto predicator_ = [](auto previousCount_) {
				return previousCount_ > 0;
			};
			return this->counters(engine_).strong.tryIncrementIf(predicator_);
		}

		public: constexpr void recordWeakReference(Engine& engine_) noexcept {
		 this->counters(engine_).weak.increment();
		}

		public: constexpr void unrecordWeakReferenceToDestroy(Engine& engine_) noexcept {
			return this->counters(engine_).weak.decrement() == 0 && this->expiredForStrongReference();
		}
	};

	template <
		typename ValueT_, ::dcool::resource::SharedAgentConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> class SharedAgent {
		public: using Self_ = SharedAgent<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::resource::SharedAgentConfigAdaptor<Config, Value>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Chassis = ::dcool::resource::SharedAgentChassis<Value, Config>;
		public: using Counters = ConfigAdaptor_::Counters;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standard_layout_breaker_;

		private: constexpr SharedAgent(Value& value_) {
			this->m_chassis_.initialize(this->engine(), value_);
		}

		private: template <
			::dcool::core::FormOfSame<Engine> EngineT__
		> constexpr SharedAgent(Value& value_, EngineT__&& engine_): m_engine_(::dcool::core::forward<EngineT__>(engine_)) {
			this->m_chassis_.initialize(this->engine(), value_);
		}

		public: SharedAgent(Self_ const&) = delete;
		public: constexpr SharedAgent(Self_&&) = delete;
		private: constexpr ~SharedAgent() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: constexpr auto operator =(Self_&&) -> Self_& = delete;

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto counters() const noexcept -> Counters const& {
			return this->m_counters_;
		}

		public: constexpr auto engine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto value() const noexcept -> Value& {
			return this->chassis().value(this->engine());
		}

		private: constexpr void destroySelf_() noexcept {
			this->engine().dismissor(*this);
		}

		public: constexpr void recordStrongReference() noexcept {
			this->chassis().recordStrongReference(this->engine());
		}

		public: constexpr void unrecordStrongReference() noexcept {
			if (this->chassis().unrecordStrongReferenceToDestroy(this->engine())) {
				this->destroySelf_();
			}
		}

		public: constexpr auto strongReferenceCount() noexcept -> ConfigAdaptor_::StrongCountUnderlying {
			return this->chassis().strongReferenceCount(this->engine());
		}

		public: constexpr auto expiredForStrongReference() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().expiredForStrongReference(this->engine());
		}

		public: constexpr auto lockForStrongReference() noexcept -> ::dcool::core::Boolean {
			return this->chassis().lockForStrongReference(this->engine());
		}

		public: constexpr void recordWeakReference() noexcept {
			this->chassis().recordWeakReference(this->engine());
		}

		public: constexpr void unrecordWeakReference() noexcept {
			if (this->chassis().unrecordWeakReferenceToDestroy(this->engine())) {
				this->destroySelf_();
			}
		}
	};

	template <
		typename ValueT_, ::dcool::resource::SharedAgentConfig<ValueT_> AgentConfigT_ = ::dcool::core::Empty<>
	> struct SharedStrongPointer {
		private: using Self_ = SharedStrongPointer<ValueT_, AgentConfigT_>;
		public: using Value = ValueT_;
		public: using AgentConfig = AgentConfigT_;

		public: using Agent = ::dcool::resource::SharedAgent<Value, AgentConfig>;
		public: using Intrusive = ::dcool::resource::IntrusiveStrong<Agent>;

		public: template <typename ValueT__, typename ValueDismissorT__, typename AgentPoolT__> friend constexpr auto wrapToShare(
			ValueT__& value_, ValueDismissorT__ valueDismissor_, AgentPoolT__ agentPool_
		);

		private: Intrusive m_intrusive_;

		public: constexpr SharedStrongPointer() noexcept = default;
		public: constexpr SharedStrongPointer(Self_ const&) noexcept = default;
		public: constexpr SharedStrongPointer(Self_&&) = default;

		public: constexpr SharedStrongPointer(::dcool::core::NullPointer) noexcept: Self_() {
		}

		private: constexpr SharedStrongPointer(Agent& agent_) noexcept: m_intrusive_(agent_) {
		}

		public: constexpr ~SharedStrongPointer() noexcept = default;
		public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
		public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

		public: constexpr auto operator =(::dcool::core::NullPointer) noexcept -> Self_& {
			this->m_intrusive_ = ::dcool::core::nullPointer;
			return *this;
		}

		public: constexpr auto intrusive() const noexcept -> Intrusive const& {
			return this->m_intrusive_;
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Value& {
			return this->intrusive().dereferenceSelf().value();
		}

		public: constexpr auto operator *() const noexcept -> Value& {
			return this->dereferenceSelf();
		}

		public: constexpr auto rawPointer() const noexcept -> Value* {
			return ::dcool::core::addressOf(this->dereferenceSelf());
		}

		public: constexpr auto operator ->() const noexcept -> Value* {
			return this->rawPointer();
		}

		public: constexpr void reset() noexcept {
			this->m_intrusive_.reset();
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	namespace detail_ {
		template <typename ValueT_, typename ValueDismissorT_, typename AgentDismissorT_> class SharedAgentMixedDismissor_ {
			public: ValueDismissorT_ valueDismissor;
			public: AgentDismissorT_ agentDismissor;

			public: template <typename ConfigT__> constexpr void operator ()(
				::dcool::resource::SharedAgent<ValueT_, ConfigT__>& toDismiss_
			) noexcept {
				this->agentDismissor(toDismiss_);
			}

			public: constexpr void operator ()(ValueT_& toDismiss_) noexcept {
				this->valueDismissor(toDismiss_);
			}
		};
	}

	namespace detail_ {
		template <typename DismissorT_> struct AgentConfigForWrapToShare_ {
			using Dismissor = DismissorT_;
		};
	}

	template <typename ValueT_, typename ValueDismissorT_, typename AgentPoolT_> constexpr auto wrapToShare(
		ValueT_& value_, ValueDismissorT_ valueDismissor_, AgentPoolT_ agentPool_
	) {
		using Dismissor_ = ::dcool::resource::detail_::SharedAgentMixedDismissor_<
			ValueT_, ValueDismissorT_, ::dcool::resource::PoolDestroyer<AgentPoolT_>
		>;
		using AgentConfig_ = ::dcool::resource::detail_::AgentConfigForWrapToShare_<Dismissor_>;
		using ConfigAdaptor_ = ::dcool::resource::SharedAgentConfigAdaptor<AgentConfig_, ValueT_>;
		using Engine_ = ConfigAdaptor_::Engine;
		auto agentPointer_ = ::dcool::resource::createPointerByPool<::dcool::resource::SharedAgent<ValueT_, AgentConfig_>>(
			agentPool_, value_, Engine_{
				.dismissor = Dismissor_{
					.valueDismissor = valueDismissor_,
					.agentDismissor = ::dcool::resource::PoolDestroyer<AgentPoolT_>{ .pool = agentPool_ }
				}
			}
		);
		return ::dcool::resource::SharedStrongPointer<ValueT_, AgentConfig_>(*agentPointer_);
	}
}

#endif
