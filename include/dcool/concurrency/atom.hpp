#ifndef DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <atomic>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueAtomicForAtom_, extractedAtomicForAtomValue_, atomic
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueStampWidthForAtom_, extractedStampWidthForAtomValue_, stampWidth
)

namespace dcool::concurrency {
	template <::dcool::core::TriviallyCopyable ValueT_> constexpr ::dcool::core::Alignment requiredAtomAlignment =
		::std::atomic_ref<ValueT_>::required_alignment
	;

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyLoad(
		ValueT_& atom_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyLoad(
		::std::atomic<ValueT_>& atom_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		::std::atomic_ref<ValueT_>(atom_).store(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		::std::atomic<ValueT_>&& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		atom_.store(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyExchange(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).exchange(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyExchange(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.exchange(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(atom_).compare_exchange_weak(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(atom_).compare_exchange_weak(expected_, newValue_, successOrder_, failureOrder_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return atom_.compare_exchange_weak(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return atom_.compare_exchange_weak(expected_, newValue_, successOrder_, failureOrder_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeStrong(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(atom_).compare_exchange_strong(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeStrong(
		ValueT_& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(atom_).compare_exchange_strong(expected_, newValue_, successOrder_, failureOrder_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeStrong(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return atom_.compare_exchange_strong(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeStrong(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return atom_.compare_exchange_strong(expected_, newValue_, successOrder_, failureOrder_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchAdd(
		ValueT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).fetch_add(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchAdd(
		::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.fetch_add(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchSubtract(
		ValueT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).fetch_sub(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchSubtract(
		::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.fetch_sub(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseAnd(
		ValueT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).fetch_and(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseAnd(
		::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.fetch_and(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseOr(
		ValueT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).fetch_or(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseOr(
		::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.fetch_or(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseExclusiveOr(
		ValueT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(atom_).fetch_xor(operand_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetchBitwiseExclusiveOr(
		::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.fetch_xor(operand_, order_);
	}

	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class AtomConfigAdaptor_ {
			private: using Self_ = AtomConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;
			static_assert(::dcool::core::TriviallyCopyable<Value>);

			public: static constexpr ::dcool::core::Triboolean atomic =
				::dcool::concurrency::detail_::extractedAtomicForAtomValue_<Config>(::dcool::core::determinateTrue)
			;
			public: static constexpr ::dcool::core::Boolean stampWidth =
				atomic ?
					::dcool::concurrency::detail_::extractedStampWidthForAtomValue_<Config>(::dcool::core::UnsignedMaxInteger(0)) :
					0
			;
		};
	}

	template <typename T_, typename ValueT_> concept AtomConfig = requires {
		typename ::dcool::concurrency::detail_::AtomConfigAdaptor_<T_, ValueT_>;
	};

	template <typename ConfigT_, typename ValueT_> requires ::dcool::concurrency::AtomConfig<
		ConfigT_, ValueT_
	> using AtomConfigAdaptor = ::dcool::concurrency::detail_::AtomConfigAdaptor_<ConfigT_, ValueT_>;

	template <typename ValueT_, ::dcool::concurrency::AtomConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct Atom {
		private: using Self_ = Atom<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::AtomConfigAdaptor<Config, Value>;
		public: static constexpr ::dcool::core::Triboolean atomic = ConfigAdaptor_::atomic;
		public: static constexpr ::dcool::core::Length stampWidth = ConfigAdaptor_::stampWidth;
		private: static constexpr ::dcool::core::Boolean stamped_ = stampWidth > 0;
		private: static constexpr ::dcool::core::Boolean supportAtomicOperation_ = !(atomic.isDeterminateFalse());
		private: static constexpr ::dcool::core::Boolean useAtomicRef_ = !(atomic.isDeterminate());

		public: using Stamp = ::dcool::core::UnsignedInteger<stampWidth>;

		public: struct StampedValue {
			[[no_unique_address]] ValueT_ value;
			[[no_unique_address]] Stamp stamp;
		};

		private: using UnderlyingValue_ = ::dcool::core::ConditionalType<stamped_, StampedValue, Value>;

		private: using Underlying_ = ::dcool::core::ConditionalType<
			atomic.isDeterminateTrue(), ::std::atomic<UnderlyingValue_>, UnderlyingValue_
		>;

		private: static constexpr ::dcool::core::Alignment underlyingAlignment_ =
			useAtomicRef_ ? ::dcool::concurrency::requiredAtomAlignment<Underlying_> : alignof(Underlying_)
		;

		private: alignas(underlyingAlignment_) Underlying_ m_underlying_;

		public: constexpr Atom() noexcept = default;
		public: Atom(Self_ const&) = delete;

		public: constexpr Atom(Value desired_) noexcept: m_underlying_ {
			.value = desired_,
			.stamp = Stamp(0)
		} {
		}

		public: auto operator =(Self_ const&)& -> Self_& = delete;

		public: constexpr auto operator =(Value const& desired_)& noexcept -> Self_& {
			this->store(desired_);
			return *this;
		}

		private: static constexpr auto sameValueRepresentation_(
			UnderlyingValue_ const& left_, UnderlyingValue_ const& right_
		) noexcept -> ::dcool::core::Boolean {
			if constexpr (stamped_) {
				return left_.stamp == right_.stamp && ::dcool::core::hasEqualValueRepresentation(left_.value, right_.value);
			}
			return ::dcool::core::hasEqualValueRepresentation(left_, right_);
		}

		// Underlying operations
		private: constexpr auto atomicallyLoadUnderlying_(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyLoad(this->m_underlying_, order_);
		}

		private: constexpr auto sequencedLoadUnderlying_() const noexcept -> UnderlyingValue_ {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyLoadUnderlying_(::std::memory_order::relaxed);
			}
			return this->m_underlying_;
		}

		private: constexpr void atomicallyStoreUnderlying_(
			UnderlyingValue_ const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_) {
			::dcool::concurrency::atomicallyStore(this->m_underlying_, desired_, order_);
		}

		private: constexpr void sequencedStoreUnderlying_(UnderlyingValue_ const& desired_) const noexcept {
			if constexpr (atomic.isDeterminateTrue()) {
				this->sequencedStoreUnderlying_(desired_, ::std::memory_order::relaxed);
			} else {
				this->m_underlying_ = desired_;
			}
		}

		private: constexpr auto atomicallyExchangeUnderlying_(
			UnderlyingValue_ const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyExchange(this->m_underlying_, desired_, order_);
		}

		private: constexpr auto sequencedExchangeUnderlying_(UnderlyingValue_ const& desired_) const noexcept -> UnderlyingValue_ {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyExchangeUnderlying_(desired_, ::std::memory_order::relaxed);
			}
			UnderlyingValue_ result_ = this->m_underlying_;
			this->m_underlying_ = desired_;
			return result_;
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingWeak_(
			UnderlyingValue_& expected_,
			UnderlyingValue_ const& desired_,
			::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyCompareExchangeWeak(this->m_underlying_, expected_, desired_, order_);
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingWeak_(
			UnderlyingValue_& expected_,
			UnderlyingValue_ const& desired_,
			::std::memory_order successOrder_,
			::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyCompareExchangeWeak(
				this->m_underlying_, expected_, desired_, successOrder_, failureOrder_
			);
		}

		private: constexpr auto sequencedCompareExchangeUnderlyingWeak_(
			UnderlyingValue_& expected_, UnderlyingValue_ const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (atomic.isDeterminateTrue()) {
				result_ = ::dcool::concurrency::atomicallyCompareExchangeWeak(
					this->m_underlying_, expected_, ::std::memory_order::relaxed
				);
			} else {
				// Non-atomic operations will not fail spuriously.
				result_ = this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
			}
			return result_;
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingStrong_(
			UnderlyingValue_& expected_,
			UnderlyingValue_ const& desired_,
			::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyExchangeUnderlying_(desired_, ::std::memory_order::relaxed);
			}
			return ::dcool::concurrency::atomicallyCompareExchangeStrong(this->m_underlying_, expected_, desired_, order_);
		}

		private: constexpr auto atomicallyCompareExchangeUnderlyingStrong_(
			UnderlyingValue_& expected_,
			UnderlyingValue_ const& desired_,
			::std::memory_order successOrder_,
			::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyCompareExchangeStrong(
				this->m_underlying_, expected_, desired_, successOrder_, failureOrder_
			);
		}

		private: constexpr auto sequencedCompareExchangeUnderlyingStrong_(
			UnderlyingValue_& expected_, UnderlyingValue_ const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (atomic.isDeterminateTrue()) {
				result_ = ::dcool::concurrency::atomicallyCompareExchangeStrong(
					this->m_underlying_, expected_, ::std::memory_order::relaxed
				);
			} else {
				if (!(sameValueRepresentation_(this->m_underlying_, expected_))) {
					expected_ = this->m_underlying_;
					return false;
				}
				this->m_underlying_ = desired_;
				result_ = true;
			}
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			UnderlyingValue_ old_ = this->atomicallyLoadUnderlying_(::std::memory_order::relaxed);
			while (
				!(
					this->atomicallyCompareExchangeUnderlyingWeak_(
						old_, ::dcool::core::invoke(transformer_, old_), order_, ::std::memory_order::relaxed
					)
				)
			) {
			}
			return old_;
		}

		private: template <typename TransformerT__> constexpr auto sequencedFetchTransformUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue_ {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchTransformUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), ::std::memory_order::relaxed
				);
			}
			UnderlyingValue_ result_ = this->m_underlying_;
			this->m_underlying_ = ::dcool::core::invoke(transformer_, result_);
			return result_;
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoadUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return this->atomicallyFetchTransformOrLoadUnderlying_(
				::dcool::core::forward<TransformerT__>(transformer_), order_, order_
			);
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoadUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			UnderlyingValue_ old_ = this->atomicallyLoadUnderlying_(loadOrder_);
			for (; ; ) {
				auto newValue_ = ::dcool::core::invoke(transformer_, old_);
				if (newValue_.valid()) {
					break;
				}
				if (this->atomicallyCompareExchangeUnderlyingWeak_(old_, newValue_.access(), transformOrder_, loadOrder_)) {
					break;
				}
			}
			return old_;
		}

		private: template <typename TransformerT__> constexpr auto sequencedFetchTransformOrLoadUnderlying_(
			TransformerT__&& transformer_
		)& noexcept -> UnderlyingValue_ {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchTransformOrLoadUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), ::std::memory_order::relaxed
				);
			}
			UnderlyingValue_ result_ = this->m_underlying_;
			auto new_ = ::dcool::core::invoke(transformer_, result_);
			if (new_.valid()) {
				this->m_underlying_ = new_.access();
			}
			return result_;
		}

		private: template <typename OperandT__> constexpr auto atomicallyFetchAddUnderlying_(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchAdd(this->m_underlying_, operand_, order_);
		}

		private: template <typename OperandT__> constexpr auto sequencedFetchAddUnderlying_(
			OperandT__ const& operand_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchAddUnderlying_(operand_, ::std::memory_order::relaxed);
			}
			return this->m_underlying_ += operand_;
		}

		private: template <typename OperandT__> constexpr auto atomicallyFetchSubtractUnderlying_(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchSubtract(this->m_underlying_, operand_, order_);
		}

		private: template <typename OperandT__> constexpr auto sequencedFetchSubtractUnderlying_(
			OperandT__ const& operand_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchSubtractUnderlying_(operand_, ::std::memory_order::relaxed);
			}
			return this->m_underlying_ -= operand_;
		}

		private: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseAndUnderlying_(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchBitwiseAnd(this->m_underlying_, operand_, order_);
		}

		private: template <typename OperandT__> constexpr auto sequencedFetchBitwiseAndUnderlying_(
			OperandT__ const& operand_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchBitwiseAndUnderlying_(operand_, ::std::memory_order::relaxed);
			}
			return this->m_underlying_ &= operand_;
		}

		private: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseOrUnderlying_(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchBitwiseOr(this->m_underlying_, operand_, order_);
		}

		private: template <typename OperandT__> constexpr auto sequencedFetchBitwiseOrUnderlying_(
			OperandT__ const& operand_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchBitwiseOrUnderlying_(operand_, ::std::memory_order::relaxed);
			}
			return this->m_underlying_ |= operand_;
		}

		private: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseExclusiveOrUnderlying_(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchBitwiseExclusiveOr(this->m_underlying_, operand_, order_);
		}

		private: template <typename OperandT__> constexpr auto sequencedFetchBitwiseExclusiveOrUnderlying_(
			OperandT__ const& operand_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			if constexpr (atomic.isDeterminateTrue()) {
				return this->atomicallyFetchBitwiseExclusiveOrUnderlying_(operand_, ::std::memory_order::relaxed);
			}
			return this->m_underlying_ ^= operand_;
		}

		// Load operations
		public: constexpr auto atomicallyLoadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyLoadUnderlying_(order_);
		}

		public: constexpr auto sequencedLoadStamped() const noexcept -> StampedValue requires (stamped_) {
			return this->sequencedLoadUnderlying_();
		}

		public: constexpr auto atomicallyLoad(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyLoadStamped(order_).value;
			} else {
				result_ = this->atomicallyLoadUnderlying_(order_);
			}
			return result_;
		}

		public: constexpr auto sequencedLoad() const noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedLoadStamped().value;
			} else {
				return this->sequencedLoadUnderlying_();
			}
			return result_;
		}

		// Store operations
		public: constexpr void atomicallyStampedStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_ && stamped_) {
			static_cast<void>(this->atomicallyStampedExchangeStamped(desired_, order_));
		}

		public: constexpr void sequencedStampedStore(Value const& desired_)& noexcept requires (stamped_) {
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = this->sequencedLoadStamped().stamp + 1
				}
			);
		}

		public: constexpr void atomicallyStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_) {
			if constexpr (stamped_) {
				this->atomicallyStampedStore(desired_, order_);
			} else {
				this->atomicallyStoreUnderlying_(desired_, order_);
			}
		}

		public: constexpr void sequencedStore(Value const& desired_)& noexcept {
			if constexpr (stamped_) {
				this->sequencedStampedStore(desired_);
			} else {
				this->sequencedStoreUnderlying_(desired_);
			}
		}

		// Exchange operations
		public: constexpr auto atomicallyStampedExchangeStamped(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			StampedValue old_ = this->atomicallyLoadStamped(::std::memory_order::relaxed);
			StampedValue new_ {
				.value = desired_,
				.stamp = old_.stamp + 1
			};
			while (!(this->atomicallyCompareExchangeUnderlyingWeak_(old_, new_, order_, ::std::memory_order::relaxed))) {
				new_.stamp = old_.stamp + 1;
			}
			return old_;
		}

		public: constexpr auto sequencedStampedExchangeStamped(
			Value const& desired_
		)& noexcept -> StampedValue requires (stamped_) {
			StampedValue old_ = this->sequencedLoadStamped();
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return old_;
		}

		public: constexpr auto atomicallyStampedExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedExchangeStamped(desired_, order_).value;
		}

		public: constexpr auto sequencedStampedExchange(Value const& desired_)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedExchangeStamped(desired_).value;
		}

		public: constexpr auto atomicallyExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedExchange(desired_, order_);
			} else {
				result_ = this->atomicallyExchangeUnderlying_(desired_, order_);
			}
			return result_;
		}

		public: constexpr auto sequencedExchange(Value const& desired_)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedExchange(desired_);
			} else {
				result_ = this->sequencedExchangeUnderlying_(desired_);
			}
			return result_;
		}

		// Weak CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedCompareExchangeStampedWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingWeak_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped_) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStampedStrong(expected_, desired_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedCompareExchangeWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->atomicallyStampedCompareExchangeStrong(successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped_) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStrong(expected_, desired_);
		}

		public: constexpr auto atomicallyCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return this->atomicallyCompareExchangeWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			::dcool::core::Boolean result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->atomicallyCompareExchangeUnderlyingWeak_(expected_, desired_, successOrder_, failureOrder_);
			}
			return result_;
		}

		public: constexpr auto sequencedCompareExchangeWeak(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedCompareExchangeWeak(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingWeak_(expected_, desired_);
			}
			return result_;
		}

		// Strong CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedCompareExchangeStampedStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingStrong_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped_) {
			StampedValue old_ = this->sequencedLoadStamped();
			if (!(sameValueRepresentation_(expected_, old_))) {
				expected_ = old_;
				return false;
			}
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return true;
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedCompareExchangeStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped_) {
			StampedValue old_ = {
				.value = expected_,
				.stamp = 0
			};
			for (; ; ) {
				if (this->atomicallyStampedCompareExchangeStampedWeak(old_, desired_, successOrder_, failureOrder_)) {
					break;
				}
				if (!(::dcool::core::hasEqualValueRepresentation(old_.value, expected_))) {
					expected_ = old_.value;
					return false;
				}
			}
			return true;
		}

		public: constexpr auto sequencedStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped_) {
			StampedValue old_ = this->sequencedLoadStamped();
			if (!(::dcool::core::hasEqualValueRepresentation(expected_, old_.value_))) {
				expected_ = old_.value_;
				return false;
			}
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return true;
		}

		public: constexpr auto atomicallyCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			return this->atomicallyCompareExchangeStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_) {
			::dcool::core::Boolean result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->atomicallyCompareExchangeUnderlyingStrong_(expected_, desired_, successOrder_, failureOrder_);
			}
			return result_;
		}

		public: constexpr auto sequencedCompareExchangeStrong(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedCompareExchangeStrong(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
			}
			return result_;
		}

		// Fetch transform operations
		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyFetchTransformUnderlying_(
				[&transformer_](StampedValue old_) noexcept -> StampedValue {
					return StampedValue {
						.value = ::dcool::core::invoke(transformer_, old_.value),
						.stamp = old_.stamp
					};
				},
				order_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped_) {
			StampedValue old_ = this->sequencedLoadStamped();
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = ::dcool::core::invoke(transformer_, old_.value),
					.stamp = old_.stamp + 1
				}
			);
			return old_;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_), order_).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_)).value;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_), order_);
			} else {
				result_ = this->atomicallyFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_), order_);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		// Fetch transform or load operations
		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, order_
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyFetchTransformOrLoadUnderlying_(
				[&transformer_](StampedValue old_) noexcept -> ::dcool::core::Optional<StampedValue> {
					auto newValue_ = ::dcool::core::invoke(transformer_, old_.value);
					if (!(newValue_.valid())) {
						return ::dcool::core::nullOptional;
					}
					return StampedValue {
						.value = ::dcool::core::invoke(transformer_, newValue_.access()),
						.stamp = old_.stamp + 1
					};
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped_) {
			StampedValue old_ = this->sequencedLoadStamped();
			auto newValue_ = ::dcool::core::invoke(transformer_, old_.value);
			if (!(newValue_.valid())) {
				return old_;
			}
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = newValue_.access(),
					.stamp = old_.stamp + 1
				}
			);
			return old_;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformOrLoadTransform(
				::dcool::core::forward<TransformerT__>(transformer_), order_, order_
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoadTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchTransformOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_)).value;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchTransformOrLoadTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyFetchTransformOrLoadUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedFetchTransformOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchTransformOrLoadTransform(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		// Arithmetic operations
		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchAddStamped(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformStamped(
				::dcool::core::AddTransformer {
					.operand = operand_
				},
				order_
			);
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchAddStamped(
			OperandT__ const& operand_
		)& noexcept -> StampedValue requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(
				::dcool::core::AddTransformer {
					.operand = operand_
				}
			);
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchAdd(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchAddStamped(operand_, order_).value;
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchAdd(
			OperandT__ const& operand_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchAddStamped(operand_).value;
		}

		public: template <typename OperandT__> constexpr auto atomicallyFetchAdd(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchAdd(operand_, order_);
			} else {
				result_ = this->atomicallyFetchAddUnderlying_(operand_, order_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto sequencedFetchAdd(OperandT__ const& operand_)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchAdd(operand_);
			} else {
				result_ = this->sequencedFetchAddUnderlying_(operand_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchSubtractStamped(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformStamped(
				::dcool::core::SubtractTransformer {
					.operand = operand_
				},
				order_
			);
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchSubtractStamped(
			OperandT__ const& operand_
		)& noexcept -> StampedValue requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(
				::dcool::core::SubtractTransformer {
					.operand = operand_
				}
			);
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchSubtract(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchSubtractStamped(operand_, order_).value;
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchSubtract(
			OperandT__ const& operand_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchSubtractStamped(operand_).value;
		}

		public: template <typename OperandT__> constexpr auto atomicallyFetchSubtract(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchSubtract(operand_, order_);
			} else {
				result_ = this->atomicallyFetchSubtractUnderlying_(operand_, order_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto sequencedFetchSubtract(
			OperandT__ const& operand_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchSubtract(operand_);
			} else {
				result_ = this->sequencedFetchSubtractUnderlying_(operand_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseAndStamped(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchTransformStamped(
				::dcool::core::BitwiseAndTransformer {
					.operand = operand_
				},
				order_
			);
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseAndStamped(
			OperandT__ const& operand_
		)& noexcept -> StampedValue requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(
				::dcool::core::BitwiseAndTransformer {
					.operand = operand_
				}
			);
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseAnd(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchBitwiseAndStamped(operand_, order_).value;
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseAnd(
			OperandT__ const& operand_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchBitwiseAndStamped(operand_).value;
		}

		public: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseAnd(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchBitwiseAnd(operand_, order_);
			} else {
				result_ = this->atomicallyFetchBitwiseAndUnderlying_(operand_, order_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto sequencedFetchBitwiseAnd(
			OperandT__ const& operand_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchBitwiseAnd(operand_);
			} else {
				result_ = this->sequencedFetchBitwiseAndUnderlying_(operand_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseOrStamped(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchBitwiseOrStamped(
				::dcool::core::BitwiseOrTransformer {
					.operand = operand_
				},
				order_
			);
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseOrStamped(
			OperandT__ const& operand_
		)& noexcept -> StampedValue requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(
				::dcool::core::BitwiseOrTransformer {
					.operand = operand_
				}
			);
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseOr(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchBitwiseOrStamped(operand_, order_).value;
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseOr(
			OperandT__ const& operand_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchBitwiseOrStamped(operand_).value;
		}

		public: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseOr(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchBitwiseOr(operand_, order_);
			} else {
				result_ = this->atomicallyFetchBitwiseOrUnderlying_(operand_, order_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto sequencedFetchBitwiseOr(
			OperandT__ const& operand_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchBitwiseOr(operand_);
			} else {
				result_ = this->sequencedFetchBitwiseOrUnderlying_(operand_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseExclusiveOrStamped(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchBitwiseExclusiveOrStamped(
				::dcool::core::BitwiseOrTransformer {
					.operand = operand_
				},
				order_
			);
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseExclusiveOrStamped(
			OperandT__ const& operand_
		)& noexcept -> StampedValue requires (stamped_) {
			return this->sequencedStampedFetchTransformStamped(
				::dcool::core::BitwiseExclusiveOrTransformer {
					.operand = operand_
				}
			);
		}

		public: template <typename OperandT__> constexpr auto atomicallyStampedFetchBitwiseExclusiveOr(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped_) {
			return this->atomicallyStampedFetchBitwiseExclusiveOrStamped(operand_, order_).value;
		}

		public: template <typename OperandT__> constexpr auto sequencedStampedFetchBitwiseExclusiveOr(
			OperandT__ const& operand_
		)& noexcept -> Value requires (stamped_) {
			return this->sequencedStampedFetchBitwiseExclusiveOrStamped(operand_).value;
		}

		public: template <typename OperandT__> constexpr auto atomicallyFetchBitwiseExclusiveOr(
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->atomicallyStampedFetchBitwiseExclusiveOr(operand_, order_);
			} else {
				result_ = this->atomicallyFetchBitwiseExclusiveOrUnderlying_(operand_, order_);
			}
			return result_;
		}

		public: template <typename OperandT__> constexpr auto sequencedFetchBitwiseExclusiveOr(
			OperandT__ const& operand_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped_) {
				result_ = this->sequencedStampedFetchBitwiseExclusiveOr(operand_);
			} else {
				result_ = this->sequencedFetchBitwiseExclusiveOrUnderlying_(operand_);
			}
			return result_;
		}
	};
}

#endif
