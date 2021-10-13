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
		ValueT_ const& object_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(object_).load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyLoad(
		::std::atomic<ValueT_> const& atom_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		::std::atomic_ref<ValueT_>(object_).store(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		atom_.store(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyExchange(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::std::atomic_ref<ValueT_>(object_).exchange(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyExchange(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return atom_.exchange(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(object_).compare_exchange_weak(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeWeak(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(object_).compare_exchange_weak(expected_, newValue_, successOrder_, failureOrder_);
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
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(object_).compare_exchange_strong(expected_, newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyCompareExchangeStrong(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_>& expected_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order successOrder_,
		::std::memory_order failureOrder_
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(object_).compare_exchange_strong(expected_, newValue_, successOrder_, failureOrder_);
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

	namespace detail_ {
		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyFetchTransform_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type {
			using Value_ = ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type;
			Value_ old_ = atom_.load(loadOrder_);
			while (
				!(
					atom_.compare_exchange_weak(
						old_, ::dcool::core::invoke(transformer_, ::dcool::core::constantize(old_)), transformOrder_, loadOrder_
					)
				)
			) {
			}
			return old_;
		}

		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type {
			using Value_ = ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type;
			Value_ old_ = atom_.load(loadOrder_);
			for (; ; ) {
				auto newValue_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(old_));
				if (!(newValue_.valid())) {
					break;
				}
				if (atom_.compare_exchange_weak(old_, newValue_.access(), transformOrder_, loadOrder_)) {
					break;
				}
			}
			return old_;
		}
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransform(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyFetchTransform_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransform(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchTransform(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, order_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransform(
		::std::atomic<ValueT_>& atom_,
		TransformerT_&& transformer_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyFetchTransform_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransform(
		::std::atomic<ValueT_>& atom_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchTransform(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), order_, order_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyFetchTransformOrLoad_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchTransformOrLoad(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, order_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad(
		::std::atomic<ValueT_>& atom_,
		TransformerT_&& transformer_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyFetchTransformOrLoad_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad(
		::std::atomic<ValueT_>& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchTransformOrLoad(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, order_
		);
	}
}

#	define DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Operation_, standardOperation_) \
		namespace dcool::concurrency { \
			namespace detail_ { \
				template <typename StandardAtomT_, typename OperandT_> auto atomicallyFetch##Operation_##_( \
					StandardAtomT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ \
				) noexcept -> ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type { \
					using Value_ = ::dcool::core::QualifiedReferenceRemovedType<StandardAtomT_>::value_type; \
					if constexpr ( \
						requires (Value_ result_) { \
							result_ = atom_.fetch_##standardOperation_(operand_, order_); \
						} \
					) { \
						return atom_.fetch_##standardOperation_(operand_, order_); \
					} \
					return ::dcool::concurrency::detail_::atomicallyFetchTransform_( \
						atom_, \
						::dcool::core::Operation_##Transformer<OperandT_ const&> { \
							.operand = operand_ \
						}, \
						order_, \
						::std::memory_order::relaxed \
					); \
				} \
			} \
			\
			template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetch##Operation_( \
				ValueT_& object_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
			) -> ValueT_ { \
				::std::atomic_ref<ValueT_> atom_(object_); \
				return ::dcool::concurrency::detail_::atomicallyFetch##Operation_##_(atom_, operand_, order_); \
			} \
			\
			template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomicallyFetch##Operation_( \
				::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
			) -> ValueT_ { \
				return ::dcool::concurrency::detail_::atomicallyFetch##Operation_##_(atom_, operand_, order_); \
			} \
		}

DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Add, add)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Subtract, sub)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseAnd, and)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseOr, or)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseExclusiveOr, xor)

namespace dcool::concurrency {
	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class AtomConfigAdaptor_ {
			private: using Self_ = AtomConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;
			static_assert(::dcool::core::TriviallyCopyable<Value>);

			public: static constexpr ::dcool::core::Triboolean atomic =
				::dcool::concurrency::detail_::extractedAtomicForAtomValue_<Config>(::dcool::core::determinateTrue)
			;
			public: static constexpr ::dcool::core::Length stampWidth =
				::dcool::concurrency::detail_::extractedStampWidthForAtomValue_<Config>(::dcool::core::UnsignedMaxInteger(0))
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
		public: static constexpr ::dcool::core::Boolean stamped = stampWidth > 0;
		private: static constexpr ::dcool::core::Boolean supportAtomicOperation_ = !(atomic.isDeterminateFalse());
		private: static constexpr ::dcool::core::Boolean useAtomicRef_ = !(atomic.isDeterminate());

		public: using Stamp = ::dcool::core::UnsignedInteger<stampWidth>;

		public: struct StampedValue {
			[[no_unique_address]] ValueT_ value;
			[[no_unique_address]] Stamp stamp;
		};

		private: using UnderlyingValue_ = ::dcool::core::ConditionalType<stamped, StampedValue, Value>;

		private: using Underlying_ = ::dcool::core::ConditionalType<
			atomic.isDeterminateTrue(), ::std::atomic<UnderlyingValue_>, UnderlyingValue_
		>;

		private: static constexpr ::dcool::core::Alignment underlyingAlignment_ =
			useAtomicRef_ ? ::dcool::concurrency::requiredAtomAlignment<Underlying_> : alignof(Underlying_)
		;

		private: alignas(underlyingAlignment_) Underlying_ m_underlying_;

		public: constexpr Atom() noexcept: Atom(Value()) {
		}

		public: Atom(Self_ const&) = delete;
		public: Atom(Self_&&) = delete;

		public: constexpr Atom(Value desired_) noexcept: m_underlying_(toUnderlying_(desired_)) {
		}

		public: auto operator =(Self_ const&)& -> Self_& = delete;

		public: constexpr auto operator =(Value const& desired_)& noexcept -> Self_& {
			this->store(desired_);
			return *this;
		}

		private: static constexpr auto toUnderlying_(Value const& value_) noexcept -> UnderlyingValue_ {
			UnderlyingValue_ result_;
			if constexpr (stamped) {
				result_.value = value_;
				result_.stamp = Stamp(0);
			} else {
				result_ = value_;
			}
			return result_;
		}

		private: static constexpr auto sameValueRepresentation_(
			UnderlyingValue_ const& left_, UnderlyingValue_ const& right_
		) noexcept -> ::dcool::core::Boolean {
			if constexpr (stamped) {
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

		private: constexpr void sequencedStoreUnderlying_(UnderlyingValue_ const& desired_)& noexcept {
			if constexpr (atomic.isDeterminateTrue()) {
				this->atomicallyStoreUnderlying_(desired_, ::std::memory_order::relaxed);
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
			return this->atomicallyFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		private: template <typename TransformerT__> constexpr auto atomicallyFetchTransformUnderlying_(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) {
			return ::dcool::concurrency::atomicallyFetchTransform(
				this->m_underlying_, ::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
			);
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
			this->m_underlying_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
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
				auto newValue_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(old_));
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
			UnderlyingValue_ result_ = this->sequencedLoadUnderlying_();
			auto new_ = ::dcool::core::invoke(
				::dcool::core::forward<TransformerT__>(transformer_), ::dcool::core::constantize(result_)
			);
			if (new_.valid()) {
				this->sequencedStoreUnderlying_(new_.access());
			}
			return result_;
		}

		// Load operations
		public: constexpr auto atomicallyLoadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyLoadUnderlying_(order_);
		}

		public: constexpr auto sequencedLoadStamped() const noexcept -> StampedValue requires (stamped) {
			return this->sequencedLoadUnderlying_();
		}

		public: constexpr auto loadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue requires (stamped) {
			StampedValue result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyLoadStamped(order_);
			} else {
				result_ = this->sequencedLoadStamped();
			}
			return result_;
		}

		public: constexpr auto atomicallyLoad(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyLoadStamped(order_).value;
			} else {
				result_ = this->atomicallyLoadUnderlying_(order_);
			}
			return result_;
		}

		public: constexpr auto sequencedLoad() const noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedLoadStamped().value;
			} else {
				return this->sequencedLoadUnderlying_();
			}
			return result_;
		}

		public: constexpr auto load(::std::memory_order order_ = ::std::memory_order::seq_cst) const noexcept -> Value {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyLoad(order_);
			} else {
				result_ = this->sequencedLoad();
			}
			return result_;
		}

		// Store operations
		public: constexpr void atomicallyStampedStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_ && stamped) {
			static_cast<void>(this->atomicallyStampedExchangeStamped(desired_, order_));
		}

		public: constexpr void sequencedStampedStore(Value const& desired_)& noexcept requires (stamped) {
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = this->sequencedLoadStamped().stamp + 1
				}
			);
		}

		public: constexpr void stampedStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (stamped) {
			if constexpr (supportAtomicOperation_) {
				this->atomicallyStampedStore(desired_, order_);
			} else {
				this->sequencedStampedStore(desired_);
			}
		}

		public: constexpr void atomicallyStore(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept requires (supportAtomicOperation_) {
			if constexpr (stamped) {
				this->atomicallyStampedStore(desired_, order_);
			} else {
				this->atomicallyStoreUnderlying_(desired_, order_);
			}
		}

		public: constexpr void sequencedStore(Value const& desired_)& noexcept {
			if constexpr (stamped) {
				this->sequencedStampedStore(desired_);
			} else {
				this->sequencedStoreUnderlying_(desired_);
			}
		}

		public: constexpr void store(Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst)& noexcept {
			if constexpr (supportAtomicOperation_) {
				this->atomicallyStore(desired_, order_);
			} else {
				this->sequencedStore(desired_);
			}
		}

		// Exchange operations
		public: constexpr auto atomicallyStampedExchangeStamped(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
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
		)& noexcept -> StampedValue requires (stamped) {
			StampedValue old_ = this->sequencedLoadStamped();
			this->sequencedStoreUnderlying_(
				StampedValue {
					.value = desired_,
					.stamp = old_.stamp + 1
				}
			);
			return old_;
		}

		public: constexpr auto stampedExchangeStamped(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			StampedValue result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedExchangeStamped(desired_, order_);
			} else {
				result_ = this->sequencedStampedExchangeStamped(desired_);
			}
			return result_;
		}

		public: constexpr auto atomicallyStampedExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedExchangeStamped(desired_, order_).value;
		}

		public: constexpr auto sequencedStampedExchange(Value const& desired_)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedExchangeStamped(desired_).value;
		}

		public: constexpr auto stampedExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedExchange(desired_, order_);
			} else {
				result_ = this->sequencedStampedExchange(desired_);
			}
			return result_;
		}

		public: constexpr auto atomicallyExchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedExchange(desired_, order_);
			} else {
				result_ = this->atomicallyExchangeUnderlying_(desired_, order_);
			}
			return result_;
		}

		public: constexpr auto sequencedExchange(Value const& desired_)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedExchange(desired_);
			} else {
				result_ = this->sequencedExchangeUnderlying_(desired_);
			}
			return result_;
		}

		public: constexpr auto exchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyExchange(desired_, order_);
			} else {
				result_ = this->sequencedExchange(desired_);
			}
			return result_;
		}

		// Weak CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStampedWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingWeak_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStampedStrong(expected_, desired_);
		}

		public: constexpr auto stampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStampedWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto stampedCompareExchangeStampedWeak(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedCompareExchangeStampedWeak(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedStampedCompareExchangeStampedWeak(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->atomicallyStampedCompareExchangeStrong(successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeWeak(
			Value& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			// Currently we do not have a determinately more efficient implementation with spuriously failure.
			return this->sequencedStampedCompareExchangeStrong(expected_, desired_);
		}

		public: constexpr auto stampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto stampedCompareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedStampedCompareExchangeWeak(expected_, desired_);
			}
			return result_;
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
			if constexpr (stamped) {
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
			if constexpr (stamped) {
				result_ = this->sequencedStampedCompareExchangeWeak(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingWeak_(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto compareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, order_, order_);
		}

		public: constexpr auto compareExchangeWeak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyCompareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedCompareExchangeWeak(expected_, desired_);
			}
			return result_;
		}

		// Strong CAS operations
		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStampedStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			StampedValue new_ = {
				.value = desired_,
				.stamp = expected_.stamp + 1
			};
			return this->atomicallyCompareExchangeUnderlyingStrong_(expected_, new_, successOrder_, failureOrder_);
		}

		public: constexpr auto sequencedStampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
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

		public: constexpr auto stampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStampedStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto stampedCompareExchangeStampedStrong(
			StampedValue& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedCompareExchangeStampedStrong(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedStampedCompareExchangeStampedStrong(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedCompareExchangeStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto atomicallyStampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (supportAtomicOperation_ && stamped) {
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
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
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

		public: constexpr auto stampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			return this->stampedCompareExchangeStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto stampedCompareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean requires (stamped) {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedStampedCompareExchangeStrong(expected_, desired_);
			}
			return result_;
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
			if constexpr (stamped) {
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
			if constexpr (stamped) {
				result_ = this->sequencedStampedCompareExchangeStrong(expected_, desired_);
			} else {
				result_ = this->sequencedCompareExchangeUnderlyingStrong_(expected_, desired_);
			}
			return result_;
		}

		public: constexpr auto compareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, order_, order_);
		}

		public: constexpr auto compareExchangeStrong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			::dcool::core::Boolean result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyCompareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
			} else {
				result_ = this->sequencedCompareExchangeStrong(expected_, desired_);
			}
			return result_;
		}

		// Fetch transform operations
		private: template <typename TransformerT__> struct StampedTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> StampedValue {
				return StampedValue {
					.value = ::dcool::core::invoke(*transformer_, ::dcool::core::constantize(old_.value)),
					.stamp = old_.stamp + 1
				};
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> StampedValue {
				return StampedValue {
					.value = ::dcool::core::invoke(
						::dcool::core::forward<TransformerT__>(*transformer_), ::dcool::core::constantize(old_.value)
					),
					.stamp = old_.stamp + 1
				};
			}
		};

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyFetchTransformUnderlying_(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedFetchTransformUnderlying_(
				StampedTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			StampedValue result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedFetchTransformStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformStamped(
				::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedFetchTransformStamped(::dcool::core::forward<TransformerT__>(transformer_)).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_) {
			return this->atomicallyFetchTransform(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyFetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_) {
			Value result_;
			if constexpr (stamped) {
				result_ = this->atomicallyStampedFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->atomicallyFetchTransformUnderlying_(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto sequencedFetchTransform(
			TransformerT__&& transformer_
		)& noexcept -> Value {
			Value result_;
			if constexpr (stamped) {
				result_ = this->sequencedStampedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto fetchTransform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->fetchTransform(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto fetchTransform(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyFetchTransform(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedFetchTransform(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		// Fetch transform or load operations
		private: template <typename TransformerT__> struct StampedConditionalTransformer_ {
			::dcool::core::ReferenceRemovedType<TransformerT__>* transformer_;

			constexpr auto operator ()(StampedValue old_)& noexcept -> StampedValue {
				auto newValue_ = ::dcool::core::invoke(*transformer_, ::dcool::core::constantize(old_.value));
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return StampedValue {
					.value = newValue_.access(),
					.stamp = old_.stamp + 1
				};
			}

			constexpr auto operator ()(StampedValue old_)&& noexcept -> StampedValue {
				auto newValue_ = ::dcool::core::invoke(
					::dcool::core::forward<TransformerT__>(*transformer_), ::dcool::core::constantize(old_.value)
				);
				if (!(newValue_.valid())) {
					return ::dcool::core::nullOptional;
				}
				return StampedValue {
					.value = newValue_.access(),
					.stamp = old_.stamp + 1
				};
			}
		};

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), order_, order_
			);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyFetchTransformOrLoadUnderlying_(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				},
				transformOrder_,
				loadOrder_
			);
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_
		)& noexcept -> StampedValue requires (stamped) {
			return this->sequencedFetchTransformOrLoadUnderlying_(
				StampedConditionalTransformer_<TransformerT__> {
					.transformer_ = ::dcool::core::addressOf(transformer_)
				}
			);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> StampedValue requires (stamped) {
			return this->stampedFetchTransformOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoadStamped(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> StampedValue requires (stamped) {
			StampedValue result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedFetchTransformOrLoadStamped(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedStampedFetchTransformOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto atomicallyStampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) {
			return this->atomicallyStampedFetchTransformOrLoadStamped(
				::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
			).value;
		}

		public: template <typename TransformerT__> constexpr auto sequencedStampedFetchTransformOrLoad(
			TransformerT__&& transformer_
		)& noexcept -> Value requires (stamped) {
			return this->sequencedStampedFetchTransformOrLoadStamped(::dcool::core::forward<TransformerT__>(transformer_)).value;
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value requires (stamped) {
			return this->stampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto stampedFetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value requires (stamped) {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyStampedFetchTransformOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedStampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
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
			if constexpr (stamped) {
				result_ = this->atomicallyStampedFetchTransformOrLoad(
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
			if constexpr (stamped) {
				result_ = this->sequencedStampedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
			} else {
				result_ = this->sequencedFetchTransformOrLoadUnderlying_(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		public: template <typename TransformerT__> constexpr auto fetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			return this->fetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_), order_, order_);
		}

		public: template <typename TransformerT__> constexpr auto fetchTransformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		)& noexcept -> Value {
			Value result_;
			if constexpr (supportAtomicOperation_) {
				result_ = this->atomicallyFetchTransformOrLoad(
					::dcool::core::forward<TransformerT__>(transformer_), transformOrder_, loadOrder_
				);
			} else {
				result_ = this->sequencedFetchTransformOrLoad(::dcool::core::forward<TransformerT__>(transformer_));
			}
			return result_;
		}

		// Arithmetic operations
#	define DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Operation_, standardOperation_) \
		private: template <typename OperandT__> constexpr auto atomicallyFetch##Operation_##Underlying_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) { \
			return ::dcool::concurrency::atomicallyFetch##Operation_(this->m_underlying_, operand_, order_); \
		} \
		\
		private: template <typename OperandT__> constexpr auto sequencedFetch##Operation_##Underlying_( \
			OperandT__ const& operand_ \
		)& noexcept -> UnderlyingValue_ requires (supportAtomicOperation_) { \
			if constexpr (atomic.isDeterminateTrue()) { \
				return this->atomicallyFetch##Operation_##Underlying_(operand_, ::std::memory_order::relaxed); \
			} \
			return this->m_underlying_ += operand_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (supportAtomicOperation_ && stamped) { \
			return this->atomicallyStampedFetchTransformStamped( \
				::dcool::core::Operation_##Transformer<OperandT__ const&> { \
					.operand = operand_ \
				}, \
				order_, \
				::std::memory_order::relaxed \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_ \
		)& noexcept -> StampedValue requires (stamped) { \
			return this->sequencedStampedFetchTransformStamped( \
				::dcool::core::Operation_##Transformer<OperandT__ const&> { \
					.operand = operand_ \
				} \
			); \
		} \
		\
		public: template <typename OperandT__> constexpr auto stampedFetch##Operation_##Stamped( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> StampedValue requires (stamped) { \
			StampedValue result_; \
			if constexpr (supportAtomicOperation_) { \
				result_ = this->atomicallyStampedFetch##Operation_##Stamped(operand_, order_); \
			} else { \
				result_ = this->sequencedStampedFetch##Operation_##Stamped(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyStampedFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_ && stamped) { \
			return this->atomicallyStampedFetch##Operation_##Stamped(operand_, order_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedStampedFetch##Operation_( \
			OperandT__ const& operand_ \
		)& noexcept -> Value requires (stamped) { \
			return this->sequencedStampedFetch##Operation_##Stamped(operand_).value; \
		} \
		\
		public: template <typename OperandT__> constexpr auto stampedFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (stamped) { \
			Value result_; \
			if constexpr (supportAtomicOperation_) { \
				result_ = this->atomicallyStampedFetch##Operation_(operand_, order_); \
			} else { \
				result_ = this->sequencedStampedFetch##Operation_(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto atomicallyFetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value requires (supportAtomicOperation_) { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->atomicallyStampedFetch##Operation_(operand_, order_); \
			} else { \
				result_ = this->atomicallyFetch##Operation_##Underlying_(operand_, order_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto sequencedFetch##Operation_( \
			OperandT__ const& operand_ \
		)& noexcept -> Value { \
			Value result_; \
			if constexpr (stamped) { \
				result_ = this->sequencedStampedFetch##Operation_(operand_); \
			} else { \
				result_ = this->sequencedFetch##Operation_##Underlying_(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto fetch##Operation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value { \
			Value result_; \
			if constexpr (supportAtomicOperation_) { \
				result_ = this->atomicallyFetch##Operation_(operand_, order_); \
			} else { \
				result_ = this->sequencedFetch##Operation_(operand_); \
			} \
			return result_; \
		} \
		\
		public: template <typename OperandT__> constexpr auto fetch_##standardOperation_( \
			OperandT__ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
		)& noexcept -> Value { \
			return this->fetch##Operation_(operand_, order_); \
		}

		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Add, add)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(Subtract, sub)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseAnd, and)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseOr, or)
		DCOOL_CONCURRENCY_DEFINE_ATOM_ARITHMETIC_OPERATION_(BitwiseExclusiveOr, xor)

		// For standard compatibility
		public: constexpr auto compare_exchange_weak(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, order_);
		}

		public: constexpr auto compare_exchange_weak(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeWeak(expected_, desired_, successOrder_, failureOrder_);
		}

		public: constexpr auto compare_exchange_strong(
			Value& expected_, Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, order_);
		}

		public: constexpr auto compare_exchange_strong(
			Value& expected_, Value const& desired_, ::std::memory_order successOrder_, ::std::memory_order failureOrder_
		)& noexcept -> ::dcool::core::Boolean {
			return this->compareExchangeStrong(expected_, desired_, successOrder_, failureOrder_);
		}
	};
}

#endif
