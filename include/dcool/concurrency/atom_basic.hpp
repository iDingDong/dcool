#ifndef DCOOL_CONCURRENCY_ATOM_BASIC_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_ATOM_BASIC_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <atomic>
#	include <thread>

namespace dcool::concurrency {
	namespace detail_ {
		constexpr auto compareRelaxedWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::relaxed:
				return ::dcool::core::PartialOrdering::equivalent;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::less;
		}

		constexpr auto compareConsumeWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::release:
				return ::dcool::core::PartialOrdering::unordered;
				case ::std::memory_order::consume:
				return ::dcool::core::PartialOrdering::equivalent;
				case ::std::memory_order::relaxed:
				return ::dcool::core::PartialOrdering::greater;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::less;
		}

		constexpr auto compareAcquireWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::release:
				return ::dcool::core::PartialOrdering::unordered;
				case ::std::memory_order::acquire:
				return ::dcool::core::PartialOrdering::equivalent;
				case ::std::memory_order::consume:
				case ::std::memory_order::relaxed:
				return ::dcool::core::PartialOrdering::greater;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::less;
		}

		constexpr auto compareReleaseWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::acquire:
				case ::std::memory_order::consume:
				return ::dcool::core::PartialOrdering::unordered;
				case ::std::memory_order::release:
				return ::dcool::core::PartialOrdering::equivalent;
				case ::std::memory_order::relaxed:
				return ::dcool::core::PartialOrdering::greater;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::less;
		}

		constexpr auto compareAcquireReleaseWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::acq_rel:
				return ::dcool::core::PartialOrdering::equivalent;
				case ::std::memory_order::seq_cst:
				return ::dcool::core::PartialOrdering::less;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::greater;
		}

		constexpr auto compareSequentiallyConsistentWith_(::std::memory_order right_) noexcept -> ::dcool::core::PartialOrdering {
			switch (right_) {
				case ::std::memory_order::seq_cst:
				return ::dcool::core::PartialOrdering::equivalent;
				default:
				break;
			}
			return ::dcool::core::PartialOrdering::greater;
		}
	}

	constexpr auto compareStrength(
		::std::memory_order left_, ::std::memory_order right_
	) noexcept -> ::dcool::core::PartialOrdering {
		switch (left_) {
			case ::std::memory_order::relaxed:
			return ::dcool::concurrency::detail_::compareRelaxedWith_(right_);
			case ::std::memory_order::consume:
			return ::dcool::concurrency::detail_::compareConsumeWith_(right_);
			case ::std::memory_order::acquire:
			return ::dcool::concurrency::detail_::compareAcquireWith_(right_);
			case ::std::memory_order::release:
			return ::dcool::concurrency::detail_::compareReleaseWith_(right_);
			case ::std::memory_order::acq_rel:
			return ::dcool::concurrency::detail_::compareAcquireReleaseWith_(right_);
			default:
			break;
		}
		return ::dcool::concurrency::detail_::compareSequentiallyConsistentWith_(right_);
	}

	constexpr auto validLoadOrder(::std::memory_order order_) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::oneOf(
			order_,
			::std::memory_order::relaxed,
			::std::memory_order::consume,
			::std::memory_order::acquire,
			::std::memory_order::seq_cst
		);
	}

	constexpr auto validStoreOrder(::std::memory_order order_) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::oneOf(
			order_,
			::std::memory_order::relaxed,
			::std::memory_order::release,
			::std::memory_order::seq_cst
		);
	}

	constexpr auto validReadConditionalWriteOrder(
		::std::memory_order readWriteOrder_, ::std::memory_order readOrder_
	) noexcept -> ::dcool::core::Boolean {
		switch (readOrder_) {
			case ::std::memory_order::relaxed:
			if (::dcool::core::oneOf(readWriteOrder_, ::std::memory_order::relaxed, ::std::memory_order::release)) {
				return true;
			}
			[[fallthrough]];
			case ::std::memory_order::consume:
			case ::std::memory_order::acquire:
			if (::dcool::core::oneOf(readWriteOrder_, ::std::memory_order::acquire, ::std::memory_order::acq_rel)) {
				return true;
			}
			[[fallthrough]];
			case ::std::memory_order::seq_cst:
			if (readWriteOrder_ == ::std::memory_order::seq_cst) {
				return true;
			}
			[[fallthrough]];
			default:
			break;
		}
		return false;
	}

	constexpr auto toReadOrder(::std::memory_order readWriteOrder_) noexcept -> ::std::memory_order {
		switch (readWriteOrder_) {
			case ::std::memory_order::acq_rel:
			return ::std::memory_order::acquire;
			case ::std::memory_order::release:
			return ::std::memory_order::relaxed;
			default:
			break;
		}
		return readWriteOrder_;
	}

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
		ValueT_& object_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		DCOOL_CORE_ASSERT(::dcool::concurrency::validLoadOrder(order_));
		return ::std::atomic_ref<ValueT_>(object_).load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyLoad(
		::std::atomic<ValueT_> const& atom_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		DCOOL_CORE_ASSERT(::dcool::concurrency::validLoadOrder(order_));
		return atom_.load(order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyLoad(
		::std::atomic<ValueT_>& atom_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) -> ValueT_ {
		return ::dcool::concurrency::atomicallyLoad(::dcool::core::constantize(atom_), order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		DCOOL_CORE_ASSERT(::dcool::concurrency::validStoreOrder(order_));
		::std::atomic_ref<ValueT_>(object_).store(newValue_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyStore(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& newValue_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) {
		DCOOL_CORE_ASSERT(::dcool::concurrency::validStoreOrder(order_));
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
		DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(successOrder_, failureOrder_));
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
		DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(successOrder_, failureOrder_));
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
		DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(successOrder_, failureOrder_));
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
		DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(successOrder_, failureOrder_));
		return atom_.compare_exchange_strong(expected_, newValue_, successOrder_, failureOrder_);
	}

	namespace detail_ {
		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyFetchTransform_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			using Value_ = StandardAtomT_::value_type;
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

		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyTransformFetch_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(transformOrder_, loadOrder_));
			using Value_ = StandardAtomT_::value_type;
			Value_ old_ = atom_.load(loadOrder_);
			Value_ new_;
			for (; ; ) {
				new_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(old_));
				if (atom_.compare_exchange_weak(old_, new_, transformOrder_, loadOrder_)) {
					break;
				}
			}
			return new_;
		}

		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyFetchTransformOrLoad_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(transformOrder_, loadOrder_));
			using Value_ = StandardAtomT_::value_type;
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

		template <typename StandardAtomT_, typename TransformerT_> auto atomicallyTransformFetchOrLoad_(
			StandardAtomT_& atom_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(transformOrder_, loadOrder_));
			using Value_ = StandardAtomT_::value_type;
			Value_ old_ = atom_.load(loadOrder_);
			for (; ; ) {
				auto newValue_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(old_));
				if (!(newValue_.valid())) {
					break;
				}
				if (atom_.compare_exchange_weak(old_, newValue_.access(), transformOrder_, loadOrder_)) {
					return newValue_.access();
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
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
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
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetch(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyTransformFetch_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetch(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyTransformFetch(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetch(
		::std::atomic<ValueT_>& atom_,
		TransformerT_&& transformer_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyTransformFetch_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetch(
		::std::atomic<ValueT_>& atom_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyTransformFetch(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
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
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
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
		::std::atomic<ValueT_>& atom_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchTransformOrLoad(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetchOrLoad(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyTransformFetchOrLoad_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetchOrLoad(
		ValueT_& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyTransformFetchOrLoad(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetchOrLoad(
		::std::atomic<ValueT_>& atom_,
		TransformerT_&& transformer_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyTransformFetchOrLoad_(
			atom_, ::dcool::core::forward<TransformerT_>(transformer_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TransformerT_> auto atomicallyTransformFetchOrLoad(
		::std::atomic<ValueT_>& object_, TransformerT_&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyTransformFetchOrLoad(
			object_, ::dcool::core::forward<TransformerT_>(transformer_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	namespace detail_ {
		template <typename StandardAtomT_> void atomicallyWait_(
			StandardAtomT_ const& atom_,
			typename StandardAtomT_::value_type const& old_,
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept {
#	if DCOOL_CPP_ATOMIC_WAIT_VERSION >= DCOOL_CPP_ATOMIC_WAIT_VERSION_FOR_P1135R6
			atom_.wait(old_, order_);
#	else
			while (::dcool::core::intelliHasEqualValueRepresentation(atom_.load(order_), old_)) {
				::std::this_thread::yield();
			}
#	endif
		}

		template <typename StandardAtomT_, typename TimePointT_> auto atomicallyWaitUntil_(
			StandardAtomT_ const& atom_,
			typename StandardAtomT_::value_type const& old_,
			TimePointT_ const& deadline_,
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> ::dcool::core::Boolean {
			while (::dcool::core::intelliHasEqualValueRepresentation(atom_.load(order_), old_)) {
				if (TimePointT_::clock::now() > deadline_) {
					return false;
				}
				::std::this_thread::yield();
			}
			return true;
		}

		template <typename StandardAtomT_> auto atomicallyWaitFetchWeak_(
			StandardAtomT_ const& atom_, typename StandardAtomT_::value_type const& old_, ::std::memory_order order_
		) noexcept -> typename StandardAtomT_::value_type {
#	if DCOOL_CPP_ATOMIC_WAIT_VERSION >= DCOOL_CPP_ATOMIC_WAIT_VERSION_FOR_P1135R6
			atom_.wait(old_, order_);
			return atom_.load(order_);
#	else
			return atom_.load(order_);
#	endif
		}

		template <typename StandardAtomT_> auto atomicallyWaitFetchStrong_(
			StandardAtomT_ const& atom_, typename StandardAtomT_::value_type const& old_, ::std::memory_order order_
		) noexcept -> typename StandardAtomT_::value_type {
			typename StandardAtomT_::value_type result_;
			for (; ; ) {
				result_ = ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, old_, order_);
				if (!(::dcool::core::intelliHasEqualValueRepresentation(result_, old_))) {
					break;
				}
				::std::this_thread::yield();
			}
			return result_;
		}

		template <typename StandardAtomT_, typename PredicateT_> auto atomicallyWaitPredicateFetch_(
			StandardAtomT_ const& atom_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> StandardAtomT_::value_type {
			typename StandardAtomT_::value_type value_ = atom_.load(order_);
			for (; ; ) {
				if (::dcool::core::invoke(predicate_, ::dcool::core::constantize(value_))) {
					break;
				}
				::std::this_thread::yield();
				value_ = ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, value_, order_);
			}
			return value_;
		}

		template <typename StandardAtomT_> void atomicallyWaitEquality_(
			StandardAtomT_ const& atom_,
			typename StandardAtomT_::value_type const& expected_,
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept {
			::dcool::concurrency::detail_::atomicallyWaitPredicateFetch_(
				atom_,
				[&expected_](typename StandardAtomT_::value_type const& value_) noexcept -> ::dcool::core::Boolean {
					return ::dcool::core::intelliHasEqualValueRepresentation(value_, expected_);
				},
				order_
			);
		}

		template <typename StandardAtomT_> void atomicallyNotifyOne_(StandardAtomT_& atom_) noexcept {
#	if DCOOL_CPP_ATOMIC_WAIT_VERSION >= DCOOL_CPP_ATOMIC_WAIT_VERSION_FOR_P1135R6
			atom_.notify_one();
#	endif
		}

		template <typename StandardAtomT_> void atomicallyNotifyAll_(StandardAtomT_& atom_) noexcept {
#	if DCOOL_CPP_ATOMIC_WAIT_VERSION >= DCOOL_CPP_ATOMIC_WAIT_VERSION_FOR_P1135R6
			atom_.notify_all();
#	endif
		}
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TimePointT_> void atomicallyWaitUntil(
		ValueT_& object_,
		ValueT_ const& old_,
		TimePointT_ const& deadline_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyWaitUntil_(atom_, old_, deadline_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TimePointT_> void atomicallyWaitUntil(
		::std::atomic<ValueT_> const& atom_,
		ValueT_ const& old_,
		TimePointT_ const& deadline_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::dcool::concurrency::detail_::atomicallyWaitUntil_(atom_, old_, deadline_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWait(
		ValueT_& object_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyWait_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWait(
		::std::atomic<ValueT_> const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::dcool::concurrency::detail_::atomicallyWait_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitFetchWeak(
		ValueT_& object_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitFetchWeak(
		::std::atomic<ValueT_> const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitFetchStrong(
		ValueT_& object_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyWaitFetchStrong_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> auto atomicallyWaitFetchStrong(
		::std::atomic<ValueT_> const& atom_, ValueT_ const& old_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyWaitFetchStrong_(atom_, old_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename PredicateT_> auto atomicallyWaitPredicateFetch(
		ValueT_& object_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyWaitPredicateFetch_(
			atom_, ::dcool::core::forward<PredicateT_>(predicate_), order_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename PredicateT_> auto atomicallyWaitPredicateFetch(
		::std::atomic<ValueT_> const& atom_, PredicateT_&& predicate_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyWaitPredicateFetch_(
			atom_, ::dcool::core::forward<PredicateT_>(predicate_), order_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWaitEquality(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& expected_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::std::atomic_ref<ValueT_> atom_(object_);
		::dcool::concurrency::detail_::atomicallyWaitEquality_(atom_, expected_, order_);
	}

	template <::dcool::core::TriviallyCopyable ValueT_> void atomicallyWaitEquality(
		::std::atomic<ValueT_> const& atom_,
		::dcool::core::UndeducedType<ValueT_> const& expected_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept {
		::dcool::concurrency::detail_::atomicallyWaitEquality_(atom_, expected_, order_);
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

	namespace detail_ {
		template <typename StandardAtomT_, typename TaskT_> auto atomicallyHintedFetchExecute_(
			StandardAtomT_& atom_,
			typename StandardAtomT_::value_type const& hint_,
			TaskT_&& task_,
			::std::memory_order transformOrder_,
			::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(transformOrder_, loadOrder_));
			using Value_ = StandardAtomT_::value_type;
			Value_ old_ = hint_;
			for (; ; ) {
				auto taskResult_ = ::dcool::core::invoke(task_, ::dcool::core::constantize(old_));
				if (taskResult_.aborted()) {
					break;
				}
				if (taskResult_.done()) {
					if (atom_.compare_exchange_weak(old_, taskResult_.accessValue(), transformOrder_, loadOrder_)) {
						break;
					}
				} else {
					::std::this_thread::yield();
					if (taskResult_.delayedRetryRequested()) {
						old_ = ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, old_, loadOrder_);
					} else {
						old_ = atom_.load(loadOrder_);
					}
				}
			}
			return old_;
		}

		template <typename StandardAtomT_, typename TaskT_> auto atomicallyHintedExecuteFetch_(
			StandardAtomT_& atom_,
			typename StandardAtomT_::value_type const& hint_,
			TaskT_&& task_,
			::std::memory_order transformOrder_,
			::std::memory_order loadOrder_
		) noexcept -> StandardAtomT_::value_type {
			DCOOL_CORE_ASSERT(::dcool::concurrency::validReadConditionalWriteOrder(transformOrder_, loadOrder_));
			using Value_ = StandardAtomT_::value_type;
			Value_ old_ = hint_;
			for (; ; ) {
				auto taskResult_ = ::dcool::core::invoke(task_, ::dcool::core::constantize(old_));
				if (taskResult_.aborted()) {
					break;
				}
				if (taskResult_.done()) {
					if (atom_.compare_exchange_weak(old_, taskResult_.accessValue(), transformOrder_, loadOrder_)) {
						return taskResult_.accessValue();
					}
				} else {
					::std::this_thread::yield();
					if (taskResult_.delayedRetryRequested()) {
						old_ = ::dcool::concurrency::detail_::atomicallyWaitFetchWeak_(atom_, old_, loadOrder_);
					} else {
						old_ = atom_.load(loadOrder_);
					}
				}
			}
			return old_;
		}
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedFetchExecute(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyHintedFetchExecute_(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedFetchExecute(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedFetchExecute(
			object_, hint_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedFetchExecute(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyHintedFetchExecute_(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedFetchExecute(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedFetchExecute(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyFetchExecute(
		ValueT_& object_, TaskT_&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchExecute(
			object_,
			::dcool::concurrency::atomicallyLoad(object_, loadOrder_),
			::dcool::core::forward<TaskT_>(task_),
			transformOrder_,
			loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyFetchExecute(
		ValueT_& object_, TaskT_&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchExecute(
			object_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyFetchExecute(
		::std::atomic<ValueT_>& atom_, TaskT_&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedFetchExecute(
			atom_,
			::dcool::concurrency::atomicallyLoad(atom_, loadOrder_),
			::dcool::core::forward<TaskT_>(task_),
			transformOrder_,
			loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyFetchExecute(
		::std::atomic<ValueT_>& atom_, TaskT_&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyFetchExecute(
			atom_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedExecuteFetch(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		::std::atomic_ref<ValueT_> atom_(object_);
		return ::dcool::concurrency::detail_::atomicallyHintedExecuteFetch_(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedExecuteFetch(
		ValueT_& object_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedExecuteFetch(
			object_, hint_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedExecuteFetch(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order transformOrder_,
		::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::detail_::atomicallyHintedExecuteFetch_(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), transformOrder_, loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyHintedExecuteFetch(
		::std::atomic<ValueT_>& atom_,
		::dcool::core::UndeducedType<ValueT_> const& hint_,
		TaskT_&& task_,
		::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedExecuteFetch(
			atom_, hint_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyExecuteFetch(
		ValueT_& object_, TaskT_&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyExecuteFetch(
			object_,
			::dcool::concurrency::atomicallyLoad(object_, loadOrder_),
			::dcool::core::forward<TaskT_>(task_),
			transformOrder_,
			loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyExecuteFetch(
		ValueT_& object_, TaskT_&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyExecuteFetch(
			object_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyExecuteFetch(
		::std::atomic<ValueT_>& atom_, TaskT_&& task_, ::std::memory_order transformOrder_, ::std::memory_order loadOrder_
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyHintedExecuteFetch(
			atom_,
			::dcool::concurrency::atomicallyLoad(atom_, loadOrder_),
			::dcool::core::forward<TaskT_>(task_),
			transformOrder_,
			loadOrder_
		);
	}

	template <::dcool::core::TriviallyCopyable ValueT_, typename TaskT_> auto atomicallyExecuteFetch(
		::std::atomic<ValueT_>& atom_, TaskT_&& task_, ::std::memory_order order_ = ::std::memory_order::seq_cst
	) noexcept -> ValueT_ {
		return ::dcool::concurrency::atomicallyExecuteFetch(
			atom_, ::dcool::core::forward<TaskT_>(task_), order_, ::dcool::concurrency::toReadOrder(order_)
		);
	}
}

#	define DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Operation_, standardOperation_) \
		namespace dcool::concurrency { \
			namespace detail_ { \
				template <typename StandardAtomT_, typename OperandT_> auto atomicallyFetch##Operation_##_( \
					StandardAtomT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ \
				) noexcept -> StandardAtomT_::value_type { \
					using Value_ = StandardAtomT_::value_type; \
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
				\
				template <typename StandardAtomT_, typename OperandT_> auto atomically##Operation_##Fetch_( \
					StandardAtomT_& atom_, OperandT_ const& operand_, ::std::memory_order order_ \
				) noexcept -> StandardAtomT_::value_type { \
					using Value_ = StandardAtomT_::value_type; \
					if constexpr ( \
						requires (Value_ result_) { \
							result_ = atom_.fetch_##standardOperation_(operand_, order_) + operand_; \
						} \
					) { \
						return atom_.fetch_##standardOperation_(operand_, order_) + operand_; \
					} \
					return ::dcool::concurrency::detail_::atomicallyTransformFetch_( \
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
			\
			template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomically##Operation_##Fetch( \
				ValueT_& object_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
			) -> ValueT_ { \
				::std::atomic_ref<ValueT_> atom_(object_); \
				return ::dcool::concurrency::detail_::atomically##Operation_##Fetch_(atom_, operand_, order_); \
			} \
			\
			template <::dcool::core::TriviallyCopyable ValueT_, typename OperandT_> auto atomically##Operation_##Fetch( \
				::std::atomic<ValueT_>& atom_, OperandT_ const& operand_, ::std::memory_order order_ = ::std::memory_order::seq_cst \
			) -> ValueT_ { \
				return ::dcool::concurrency::detail_::atomically##Operation_##Fetch_(atom_, operand_, order_); \
			} \
		}

DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Add, add)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(Subtract, sub)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseAnd, and)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseOr, or)
DCOOL_CONCURRENCY_DEFINE_ATOMIC_ARITHMETIC_OPERATION_(BitwiseExclusiveOr, xor)

#endif
