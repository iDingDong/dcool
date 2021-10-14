#ifndef DCOOL_CONCURRENCY_ATOM_BASIC_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_ATOM_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <atomic>
#	include <thread>

namespace dcool::concurrency {
	namespace detail_ {
		constexpr auto intepretStandardLockFree(int value_) noexcept -> ::dcool::core::Triboolean {
			switch (value_) {
				case 0:
				return ::dcool::core::determinateFalse;
				case 2:
				return ::dcool::core::determinateTrue;
				default:
				break;
			}
			return ::dcool::core::indeterminate;
		}
	}

	template <typename T_> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic =
		::std::atomic<T_>::is_always_lock_free() ? ::dcool::core::determinateTrue : ::dcool::core::indeterminate
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<::dcool::core::Boolean> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_BOOL_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<unsigned char> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<signed char> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<char> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<char8_t> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR8_T_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<char16_t> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR16_T_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<char32_t> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_CHAR32_T_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<wchar_t> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_WCHAR_T_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<unsigned short> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_SHORT_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<signed short> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_SHORT_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<unsigned int> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_INT_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<signed int> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_INT_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<unsigned long> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_LONG_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<signed long> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_LONG_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<unsigned long long> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_LLONG_LOCK_FREE)
	;

	template <> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<signed long long> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_LLONG_LOCK_FREE)
	;

	template <typename ValueT_> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomic<ValueT_*> =
		::dcool::concurrency::detail_::intepretStandardLockFree(ATOMIC_POINTER_LOCK_FREE)
	;

	template <typename T_> constexpr ::dcool::core::Triboolean lockFreeInStandardAtomicRef =
		::std::atomic_ref<T_>::is_always_lock_free() ? ::dcool::core::determinateTrue : ::dcool::core::indeterminate
	;

	template <::dcool::core::TriviallyCopyable ValueT_> constexpr ::dcool::core::Alignment requiredAtomAlignment =
		::std::atomic_ref<ValueT_>::required_alignment
	;

	template <::dcool::core::TriviallyCopyable ValueT_> auto lockFreeAtomicityOnExecution(
		ValueT_ const& object_
	) -> ::dcool::core::Boolean {
		return ::std::atomic_ref<ValueT_>(object_).is_lock_free();
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto lockFreeAtomicityOnExecution(
		::std::atomic<ValueT_> const& atom_
	) -> ::dcool::core::Boolean {
		return atom_.is_lock_free();
	}

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

	namespace detail_ {
		template <typename StandardAtomT_, ::dcool::core::TriviallyCopyable ValueT_> void atomicallyWait_(
			StandardAtomT_ const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept {
#	if DCOOL_CPP_P1135R6_ENABLED
			atom_.wait(old_, order_);
#	else
			while (::dcool::core::hasEqualValueRepresentation(atom_.load(order_), old_)) {
				::std::this_thread::yield();
			}
#	endif
		}

		template <typename StandardAtomT_, ::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitNew_(
			StandardAtomT_ const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> ValueT_ {
			ValueT_ result_;
			for (; ; ) {
#	if DCOOL_CPP_P1135R6_ENABLED
				atom_.wait(old_, order_);
#	endif
				result_ = atom_.load(order_);
				if (!(::dcool::core::hasEqualValueRepresentation(atom_.load(order_), old_))) {
					break;
				}
				::std::this_thread::yield();
			}
			return result_;
		}

		template <typename StandardAtomT_, typename PredicateT_> void atomicallyWaitPredicate_(
			StandardAtomT_ const& atom_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept {
			using Value_ = StandardAtomT_::value_type;
			Value_ value_ = atom_.load(order_);
			for (; ; ) {
				if (::dcool::core::invoke(predicate_, ::dcool::core::constantize(value_))) {
					break;
				}
				::std::this_thread::yield();
				value_ = ::dcool::concurrency::detail_::atomicallyWaitNew_(atom_, value_, order_);
			}
		}

		template <typename StandardAtomT_> void atomicallyNotifyOne_(StandardAtomT_& object_) noexcept {
#	if DCOOL_CPP_P1135R6_ENABLED
			atom_.notify_one();
#	endif
		}

		template <typename StandardAtomT_> void atomicallyNotifyAll_(StandardAtomT_& object_) noexcept {
#	if DCOOL_CPP_P1135R6_ENABLED
			atom_.notify_all();
#	endif
		}
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWait(
		ValueT_ const& object_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyWait_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWait(
		::std::atomic<ValueT_> const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::dcool::concurrency::detail_::atomicallyWait_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitNew(
		ValueT_ const& object_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyWaitNew_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitNew(
		::std::atomic<ValueT_> const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyWaitNew_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename PredicateT_> void atomicallyWaitPredicate(
		ValueT_ const& object_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyWaitPredicate_(atom_, ::dcool::core::forward<PredicateT_>(predicate_), order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename PredicateT_> void atomicallyWaitPredicate(
		::std::atomic<ValueT_> const& atom_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::dcool::concurrency::detail_::atomicallyWaitPredicate_(atom_, ::dcool::core::forward<PredicateT_>(predicate_), order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyNotifyOne(ValueT_& object_) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyNotifyOne_(atom_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyNotifyOne(::std::atomic<ValueT_>& atom_) noexcept {
		::dcool::concurrency::detail_::atomicallyNotifyOne_(atom_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyNotifyAll(ValueT_& object_) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyNotifyAll_(atom_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyNotifyAll(::std::atomic<ValueT_>& atom_) noexcept {
		::dcool::concurrency::detail_::atomicallyNotifyAll_(atom_);
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

#endif
