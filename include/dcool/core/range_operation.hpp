#ifndef DCOOL_CORE_RANGE_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RANGE_OPERATION_HPP_INCLUDED_

#	include <dcool/core/compare.hpp>
#	include <dcool/core/contamination.hpp>
#	include <dcool/core/debug.hpp>
#	include <dcool/core/destruct.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/optional.hpp>
#	include <dcool/core/range.hpp>

#	include <algorithm>

namespace dcool::core {
	template <
		::dcool::core::ForwardIterator IteratorT_
	> constexpr void batchDestruct(IteratorT_ begin_, IteratorT_ end_) noexcept {
		using Value_ = ::dcool::core::IteratorValueType<IteratorT_>;
		if constexpr (!::dcool::core::isTriviallyDestructible<Value_>) {
			while (begin_ != end_) {
				::dcool::core::destruct(::dcool::core::dereference(begin_));
				++begin_;
			}
		}
	}

	template <
		::dcool::core::ForwardIterator IteratorT_
	> constexpr void batchDestructN(IteratorT_ begin_, ::dcool::core::IteratorDifferenceType<IteratorT_> count_) noexcept {
		DCOOL_CORE_ASSERT(count_ >= 0);
		using Value_ = ::dcool::core::IteratorValueType<IteratorT_>;
		if constexpr (!::dcool::core::isTriviallyDestructible<Value_>) {
			while (count_ > 0) {
				::dcool::core::destruct(::dcool::core::dereference(begin_));
				++begin_;
				--count_;
			}
		}
	}

	template <::dcool::core::ForwardIterator IteratorT_> constexpr void batchDefaultInitialize(
		IteratorT_ begin_, IteratorT_ end_
	) noexcept(noexcept(new (::dcool::core::rawPointerOf(begin_)) ::dcool::core::IteratorValueType<IteratorT_>)) {
		for (IteratorT_ current_ = begin_; current_ < end_; ++current_) {
			if constexpr (noexcept(new (::dcool::core::rawPointerOf(current_)) ::dcool::core::IteratorValueType<IteratorT_>)) {
				new (::dcool::core::rawPointerOf(current_)) ::dcool::core::IteratorValueType<IteratorT_>;
			} else {
				try {
					new (::dcool::core::rawPointerOf(current_)) ::dcool::core::IteratorValueType<IteratorT_>;
				} catch (...) {
					::dcool::core::batchDestruct(begin_, current_);
					throw;
				}
			}
		}
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,typename ValueT_,::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr void batchFill(ValueT_&& value_, DestinationIteratorT_ begin_, DestinationIteratorT_ end_) {
		if constexpr (::dcool::core::atAnyCost(strategyC_) && !::dcool::core::ConstReference<ValueT_&&>) {
			::dcool::core::batchFill<strategyC_>(::dcool::core::constantize(value_), begin_, end_);
		} else {
			while (begin_ != end_) {
				*begin_ = value_;
				++begin_;
			}
		}
	}

	template <typename ValueT_, ::dcool::core::ForwardIterator DestinationIteratorT_> constexpr void batchFill(
		ValueT_&& value_, DestinationIteratorT_ begin_, DestinationIteratorT_ end_
	) {
		::dcool::core::batchFill<::dcool::core::defaultExceptionSafetyStrategy>(
			::dcool::core::forward<ValueT_>(value_), begin_, end_
		);
	}

	template <::dcool::core::ForwardIterator IteratorT_> struct OverlappedResult {
		::dcool::core::Optional<::dcool::core::IteratorDifferenceType<IteratorT_>> countBeforeOverlap;
		IteratorT_ destinationEnd;
	};

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopy(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (
			::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>
		) {
			if constexpr (
				::dcool::core::ContiguousIterator<SourceIteratorT_> && ::dcool::core::ContiguousIterator<DestinationIteratorT_>
			) {
				auto length_ = end_ - begin_;
				::std::copy_n(::dcool::core::rawPointerToLivingOf(begin_), length_, ::dcool::core::rawPointerOf(destination_));
				DestinationIteratorT_ destinationEnd_ = destination_ + length_;
				return destinationEnd_;
			}
			if constexpr (
				::dcool::core::ReversedContiguousIterator<SourceIteratorT_> &&
				::dcool::core::ReversedContiguousIterator<DestinationIteratorT_>
			) {
				auto length_ = end_ - begin_;
				if (length_ <= 0) {
					return destination_;
				}
				DestinationIteratorT_ destinationEnd_ = destination_ + length_;
				::std::copy_n(::dcool::core::rawPointerToLivingOf(end_ - 1), length_, ::dcool::core::rawPointerOf(destinationEnd_ - 1));
				return destinationEnd_;
			}
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::copy(begin_, end_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopy(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchCopy<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyN(
		SourceIteratorT_ begin_, ::dcool::core::IteratorDifferenceType<SourceIteratorT_> count_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		DCOOL_CORE_ASSERT(count_ >= 0);
		if constexpr (::dcool::core::RandomAccessIterator<SourceIteratorT_>) {
			return ::dcool::core::batchCopy<strategyC_>(begin_, begin_ + count_, destination_);
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::copy_n(begin_, count_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyN(
		SourceIteratorT_ begin_, ::dcool::core::IteratorDifferenceType<SourceIteratorT_> count_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchCopyN<::dcool::core::defaultExceptionSafetyStrategy>(begin_, count_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyConstruct(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (
			::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_> &&
			::dcool::core::isTriviallyCopyable<SourceValue_>
		) {
			return ::dcool::core::batchCopy<strategyC_>(begin_, end_, destination_);
		}
		DestinationIteratorT_ destinationCurrent_ = destination_;
		try {
			while (begin_ != end_) {
				new (::dcool::core::rawPointerOf(destinationCurrent_)) DestinationValue_(*begin_);
				++begin_;
				++destinationCurrent_;
			}
		} catch (...) {
			::dcool::core::batchDestruct(destination_, destinationCurrent_);
			throw;
		}
		return destinationCurrent_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyConstruct(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchCopyConstruct<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyConstructN(
		SourceIteratorT_ begin_, ::dcool::core::IteratorDifferenceType<SourceIteratorT_> count_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		DCOOL_CORE_ASSERT(count_ >= 0);
		if constexpr (::dcool::core::RandomAccessIterator<SourceIteratorT_>) {
			return ::dcool::core::batchCopyConstruct<strategyC_>(begin_, begin_ + count_, destination_);
		}
		DestinationIteratorT_ destinationCurrent_ = destination_;
		try {
			while (count_ > 0) {
				using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
				new (::dcool::core::rawPointerOf(destinationCurrent_)) DestinationValue_(*begin_);
				++begin_;
				--count_;
				++destinationCurrent_;
			}
		} catch (...) {
			::dcool::core::batchDestruct(destination_, destinationCurrent_);
			throw;
		}
		return destinationCurrent_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyConstructN(
		SourceIteratorT_ begin_, ::dcool::core::IteratorDifferenceType<SourceIteratorT_> count_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchCopyConstructN<::dcool::core::defaultExceptionSafetyStrategy>(begin_, count_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (
			::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>
		) {
			if constexpr (
				::dcool::core::ContiguousIterator<SourceIteratorT_> && ::dcool::core::ContiguousIterator<DestinationIteratorT_>
			) {
				auto length_ = end_ - begin_;
				::dcool::core::Size size_ = sizeof(SourceValue_) * length_;
				::std::memmove(::dcool::core::rawPointerOf(destination_), ::dcool::core::rawPointerToLivingOf(begin_), size_);
				DestinationIteratorT_ destinationEnd_ = destination_ + length_;
				return destinationEnd_;
			}
			if constexpr (
				::dcool::core::ReversedContiguousIterator<SourceIteratorT_> &&
				::dcool::core::ReversedContiguousIterator<DestinationIteratorT_>
			) {
				auto length_ = end_ - begin_;
				if (length_ <= 0) {
					return destination_;
				}
				::dcool::core::Size size_ = sizeof(SourceValue_) * length_;
				DestinationIteratorT_ destinationEnd_ = destination_ + length_;
				::std::memmove(::dcool::core::rawPointerOf(destinationEnd_ - 1), ::dcool::core::rawPointerToLivingOf(end_ - 1), size_);
				return destinationEnd_;
			}
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::copy(begin_, end_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchCopyForward<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchCopyForwardN(
		SourceIteratorT_ begin_, ::dcool::core::IteratorDifferenceType<SourceIteratorT_> count_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		DCOOL_CORE_ASSERT(count_ >= 0);
		if constexpr (::dcool::core::RandomAccessIterator<SourceIteratorT_>) {
			return ::dcool::core::batchCopyForwardN<strategyC_>(begin_, begin_ + count_, destination_);
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::copy_n(begin_, count_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMove(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (::dcool::core::isSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>) {
			return ::dcool::core::batchCopy<strategyC_>(begin_, end_, destination_);
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::move(begin_, end_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMove(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchMove<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (
			::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>
		) {
			return ::dcool::core::batchCopyForward<strategyC_>(begin_, end_, destination_);
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::move(begin_, end_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchMoveForward<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	// Two ranges shall not overlap.
	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveConstruct(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		if constexpr (
			::dcool::core::atAnyCost(strategyC_) &&
			(
				!noexcept(
					new (::dcool::core::rawPointerOf(destination_)) ::dcool::core::IteratorValueType<DestinationIteratorT_>(
						::dcool::core::move(::dcool::core::dereference(begin_))
					)
				)
			)
		) {
			return ::dcool::core::batchCopyConstruct(begin_, end_, destination_);
		}
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (
			::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>
		) {
			return ::dcool::core::batchMove<strategyC_>(begin_, end_, destination_);
		}
		SourceIteratorT_ sourceCurrent_ = begin_;
		DestinationIteratorT_ destinationCurrent_ = destination_;
		while (sourceCurrent_ != end_) {
			try {
				new (::dcool::core::rawPointerOf(destinationCurrent_)) DestinationValue_(
					::dcool::core::move(::dcool::core::dereference(sourceCurrent_))
				);
			} catch (...) {
				if constexpr (::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_>) {
					try {
						::std::move(destination_, destinationCurrent_, begin_);
					} catch (...) {
						::dcool::core::goWeak<strategyC_>();
					}
				} else {
					::dcool::core::goWeak<strategyC_>();
				}
				::dcool::core::batchDestruct(destination_, destinationCurrent_);
				throw;
			}
			++sourceCurrent_;
			++destinationCurrent_;
		}
		return destinationCurrent_;
	}

	// Two ranges shall not overlap.
	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveConstruct(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchMoveConstruct<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveConstructForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> ::dcool::core::OverlappedResult<DestinationIteratorT_> {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (::dcool::core::QualifiedOfSame<SourceValue_, DestinationValue_>) {
			using Value_ = SourceValue_;
			if constexpr (::dcool::core::isTriviallyCopyable<Value_>) {
				if constexpr (
					::dcool::core::ContiguousIterator<SourceIteratorT_> && ::dcool::core::ContiguousIterator<DestinationIteratorT_>
				) {
					DestinationIteratorT_ result_ = ::dcool::core::batchMoveForward<strategyC_>(begin_, end_, destination_);
					Value_ const* destinationAddress_ = ::dcool::core::rawPointerOf(destination_);
					Value_ const* resultAddress_ = ::dcool::core::rawPointerOf(result_);
					Value_ const* beginAddress_ = ::dcool::core::rawPointerOf(begin_);
					return { resultAddress_ > beginAddress_ ? beginAddress_ - destinationAddress_ : ::dcool::core::nullOptional, result_ };
				}
				if constexpr (
					::dcool::core::ReversedContiguousIterator<SourceIteratorT_> &&
					::dcool::core::ReversedContiguousIterator<DestinationIteratorT_>
				) {
					DestinationIteratorT_ result_ = ::dcool::core::batchMoveForward<strategyC_>(begin_, end_, destination_);
					Value_ const* destinationAddress_ = ::dcool::core::rawPointerOf(destination_);
					Value_ const* resultAddress_ = ::dcool::core::rawPointerOf(result_);
					Value_ const* beginAddress_ = ::dcool::core::rawPointerOf(begin_);
					return { resultAddress_ < beginAddress_ ? destinationAddress_ - beginAddress_ : ::dcool::core::nullOptional, result_ };
				}
			}
			SourceIteratorT_ sourceCurrent_ = begin_;
			DestinationIteratorT_ destinationCurrent_ = destination_;
			::dcool::core::IteratorDifferenceType<DestinationIteratorT_> countBeforeOverlap_ = 0;
			while (sourceCurrent_ != end_) {
				if (::dcool::core::iteratorsToSame(destinationCurrent_, begin_)) {
					while (sourceCurrent_ != end_) {
						try {
							::dcool::core::dereference(destinationCurrent_) = ::dcool::core::move(::dcool::core::dereference(sourceCurrent_));
						} catch (...) {
							try {
								::std::move_backward(destination_, destinationCurrent_, begin_);
							} catch (...) {
								::dcool::core::goWeak<strategyC_>();
							}
							::dcool::core::batchDestructN(destination_, countBeforeOverlap_);
							throw;
						}
						++sourceCurrent_;
						++destinationCurrent_;
					}
					return { countBeforeOverlap_, destinationCurrent_ };
				}
				try {
					new (::dcool::core::rawPointerOf(destinationCurrent_)) Value_(
						::dcool::core::move(::dcool::core::dereference(sourceCurrent_))
					);
				} catch (...) {
					try {
						::std::move(destination_, destinationCurrent_, begin_);
					} catch (...) {
						::dcool::core::goWeak<strategyC_>();
					}
					::dcool::core::batchDestruct(destination_, destinationCurrent_);
					throw;
				}
				++sourceCurrent_;
				++destinationCurrent_;
				++countBeforeOverlap_;
			}
			return { ::dcool::core::nullOptional, destinationCurrent_ };
		}
		return {
			::dcool::core::nullOptional, ::dcool::core::batchMoveConstruct<strategyC_>(begin_, end_, destination_)
		};
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveConstructForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> ::dcool::core::OverlappedResult<DestinationIteratorT_> {
		return ::dcool::core::batchMoveConstructForward<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> requires ::dcool::core::isSame<
		::dcool::core::IteratorValueType<SourceIteratorT_>, ::dcool::core::IteratorValueType<DestinationIteratorT_>
	> constexpr auto batchRelocateForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		auto moveConstructResult_ = ::dcool::core::batchMoveConstructForward<strategyC_>(begin_, end_, destination_);
		if (moveConstructResult_.countBeforeOverlap.valid()) {
			::dcool::core::batchDestructN(moveConstructResult_.destinationEnd, moveConstructResult_.countBeforeOverlap.access());
		} else {
			::dcool::core::batchDestruct(begin_, end_);
		}
		return moveConstructResult_.destinationEnd;
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> requires ::dcool::core::isSame<
		::dcool::core::IteratorValueType<SourceIteratorT_>, ::dcool::core::IteratorValueType<DestinationIteratorT_>
	> constexpr auto batchRelocateForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchRelocateForward<
			::dcool::core::defaultExceptionSafetyStrategy, ::dcool::core::IteratorValueType<SourceIteratorT_>
		>(begin_, end_, destination_);
	}

	// Two ranges shall not overlap.
	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> requires ::dcool::core::isSame<
		::dcool::core::IteratorValueType<SourceIteratorT_>, ::dcool::core::IteratorValueType<DestinationIteratorT_>
	> constexpr auto batchRelocate(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) noexcept(noexcept(::dcool::core::batchRelocateForward<strategyC_>(begin_, end_, destination_))) -> DestinationIteratorT_ {
		DestinationIteratorT_ result_ = ::dcool::core::batchMoveConstruct<strategyC_>(begin_, end_, destination_);
		::dcool::core::batchDestruct(begin_, end_);
		return result_;
	}

	// Two ranges shall not overlap.
	template <
		::dcool::core::ForwardIterator SourceIteratorT_, ::dcool::core::ForwardIterator DestinationIteratorT_
	> requires ::dcool::core::isSame<
		::dcool::core::IteratorValueType<SourceIteratorT_>, ::dcool::core::IteratorValueType<DestinationIteratorT_>
	> constexpr auto batchRelocate(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) noexcept(
		noexcept(::dcool::core::batchRelocate<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_))
	) -> DestinationIteratorT_ {
		return ::dcool::core::batchRelocate<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}
}

#endif
