#ifndef DCOOL_CORE_RANGE_OPERATION_HPP_INCLUDED_
#	define DCOOL_CORE_RANGE_OPERATION_HPP_INCLUDED_

#	include <dcool/core/compare.hpp>
#	include <dcool/core/contamination.hpp>
#	include <dcool/core/destruct.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/range.hpp>

#	include <algorithm>
#	include <cstring>

namespace dcool::core {
	namespace detail_ {
		template <
			::dcool::core::ForwardIterator BeginIteratorT_, ::dcool::core::ForwardIterator EndIteratorT_
		> constexpr void batchDestruct_(BeginIteratorT_ begin_, EndIteratorT_ end_) noexcept {
			using Value_ = ::dcool::core::IteratorValueType<BeginIteratorT_>;
			if constexpr (!::dcool::core::isTriviallyDestructible<Value_>) {
				while (!::dcool::core::iteratorsToSame(begin_, end_)) {
					::dcool::core::destruct(::dcool::core::dereference(begin_));
					++begin_;
				}
			}
		}
	}

	template <
		::dcool::core::ForwardIterator IteratorT_
	> constexpr void batchDestruct(IteratorT_ begin_, IteratorT_ end_) noexcept {
		::dcool::core::detail_::batchDestruct_(begin_, end_);
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

	template <::dcool::core::ForwardIterator IteratorT_> struct BatchMoveConstructOverlappedResult {
		::dcool::core::Boolean overlapped;
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
				::dcool::core::Size size_ = sizeof(SourceValue_) * length_;
				::std::memcpy(::dcool::core::rawPointerOf(destination_), ::dcool::core::rawPointerToLivingOf(begin_), size_);
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
				::std::memcpy(::dcool::core::rawPointerOf(destinationEnd_ - 1), ::dcool::core::rawPointerToLivingOf(end_ - 1), size_);
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
		if constexpr (::dcool::core::isSame<SourceValue_, DestinationValue_> && ::dcool::core::isTriviallyCopyable<SourceValue_>) {
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
			return ::std::uninitialized_copy(begin_, end_, destination_);
		}
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (::dcool::core::isSame<SourceValue_, DestinationValue_>) {
			using Value_ = SourceValue_;
			if constexpr (::dcool::core::isTriviallyCopyable<Value_>) {
				return ::dcool::core::batchMove<strategyC_>(begin_, end_, destination_);
			}
			SourceIteratorT_ sourceCurrent_ = begin_;
			DestinationIteratorT_ destinationCurrent_ = destination_;
			while (sourceCurrent_ != end_) {
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
			}
			return destinationCurrent_;
		}
		DestinationIteratorT_ result_;
		try {
			result_ = ::std::uninitialized_move(begin_, end_, destination_);
		} catch (...) {
			::dcool::core::goWeak<strategyC_>();
			throw;
		}
		return result_;
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
	) -> ::dcool::core::BatchMoveConstructOverlappedResult<DestinationIteratorT_> {
		using SourceValue_ = ::dcool::core::IteratorValueType<SourceIteratorT_>;
		using DestinationValue_ = ::dcool::core::IteratorValueType<DestinationIteratorT_>;
		if constexpr (::dcool::core::isSame<SourceValue_, DestinationValue_>) {
			using Value_ = SourceValue_;
			if constexpr (::dcool::core::isTriviallyCopyable<Value_>) {
				if constexpr (
					::dcool::core::ContiguousIterator<SourceIteratorT_> && ::dcool::core::ContiguousIterator<DestinationIteratorT_>
				) {
					DestinationIteratorT_ result_ = ::dcool::core::batchMoveForward<strategyC_>(begin_, end_, destination_);
					return { ::dcool::core::rawPointerOf(result_) > ::dcool::core::rawPointerOf(begin_), result_ };
				}
				if constexpr (
					::dcool::core::ReversedContiguousIterator<SourceIteratorT_> &&
					::dcool::core::ReversedContiguousIterator<DestinationIteratorT_>
				) {
					DestinationIteratorT_ result_ = ::dcool::core::batchMoveForward<strategyC_>(begin_, end_, destination_);
					return { ::dcool::core::rawPointerOf(result_) < ::dcool::core::rawPointerOf(begin_), result_ };
				}
			}
			SourceIteratorT_ sourceCurrent_ = begin_;
			DestinationIteratorT_ destinationCurrent_ = destination_;
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
							::dcool::core::detail_::batchDestruct_(destination_, begin_);
							throw;
						}
						++sourceCurrent_;
						++destinationCurrent_;
					}
					return { true, destinationCurrent_ };
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
			}
			return { false, destinationCurrent_ };
		}
		return { false, ::dcool::core::batchMoveConstruct<strategyC_>(begin_, end_, destination_) };
	}

	template <
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchMoveConstructForward(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) -> DestinationIteratorT_ {
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
		if (moveConstructResult_.overlapped) {
			::dcool::core::detail_::batchDestruct_(moveConstructResult_.destinationEnd, end_);
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
	) noexcept(noexcept(::std::uninitialized_move(begin_, end_, destination_))) -> DestinationIteratorT_ {
		return ::dcool::core::batchRelocate<::dcool::core::defaultExceptionSafetyStrategy>(begin_, end_, destination_);
	}
}

#endif
