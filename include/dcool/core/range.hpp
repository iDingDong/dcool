#ifndef DCOOL_CORE_RANGE_HPP_INCLUDED_
#	define DCOOL_CORE_RANGE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>
#	include <dcool/core/dereference.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/type_difference_detector.hpp>
#	include <dcool/core/type_value_detector.hpp>
#	include <dcool/core/utility.hpp>

#	include <algorithm>
#	include <iterator>
#	include <ranges>
#	include <cstring>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeIteratorCatagory, ExtractedIteratorCatagoryType, IteratorCategory)

namespace dcool::core {
	using ContiguousIteratorTag = ::std::contiguous_iterator_tag;

	namespace detail_ {
		template <typename IteratorT_> using IteratorValueType_ = ::dcool::core::ExtractedValueType<
			IteratorT_, typename ::std::iterator_traits<IteratorT_>::value_type
		>;

		template <typename IteratorT_> using IteratorBasicValueType_ = ::dcool::core::ReferenceRemovedType<
			decltype(*(::dcool::core::declval<IteratorT_&>()))
		>;
	}

	template <typename IteratorT_> using IteratorValueType = ::dcool::core::ConditionalType<
		::dcool::core::FormOfSame<
			::dcool::core::detail_::IteratorValueType_<IteratorT_>,
			::dcool::core::detail_::IteratorBasicValueType_<IteratorT_>
		>,
		::dcool::core::IdenticallyQualifiedType<
			::dcool::core::detail_::IteratorValueType_<IteratorT_>, ::dcool::core::detail_::IteratorBasicValueType_<IteratorT_>
		>,
		::dcool::core::detail_::IteratorValueType_<IteratorT_>
	>;

	template <typename IteratorT_> using IteratorPointerType = ::std::iterator_traits<IteratorT_>::pointer;

	template <typename IteratorT_> using IteratorReferenceType = ::std::iterator_traits<IteratorT_>::reference;

	template <typename IteratorT_> using IteratorDifferenceType = ::dcool::core::ExtractedDifferenceType<
		IteratorT_, typename ::std::iterator_traits<IteratorT_>::difference_type
	>;

	template <typename IteratorT_> using IteratorCatagoryType = ::dcool::core::ExtractedIteratorCatagoryType<
		IteratorT_, typename ::std::iterator_traits<IteratorT_>::iterator_category
	>;

	template <typename IteratorT_> using ReverseIterator = ::std::reverse_iterator<IteratorT_>;

	template <typename IteratorT_> constexpr auto makeReverseIterator(IteratorT_ iterator_) noexcept {
		return ::std::make_reverse_iterator(iterator_);
	}

	template <typename IteratorT_> struct StandardIterator {
		private: using Self_ = StandardIterator<IteratorT_>;
		public: using Iterator = IteratorT_;

		public: using Difference = ::dcool::core::IteratorDifferenceType<Iterator>;
		public: using Value = ::dcool::core::IteratorValueType<Iterator>;
		public: using Pointer = ::dcool::core::IteratorPointerType<Iterator>;
		public: using Reference = ::dcool::core::IteratorReferenceType<Iterator>;
		public: using IteratorCategory = ::dcool::core::IteratorCatagoryType<Iterator>;

		public: using difference_type = Difference;
		public: using value_type = Value;
		public: using pointer = Pointer;
		public: using reference = Reference;
		public: using iterator_category = IteratorCategory;

		public: Iterator iterator;

		public: constexpr auto operator ++() noexcept -> Self_& {
			++(this->iterator);
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_ = *this;
			++(this->iterator);
			return result_;
		}

		public: constexpr auto operator --() noexcept -> Self_& {
			--(this->iterator);
			return *this;
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_ = *this;
			--(this->iterator);
			return result_;
		}

		public: constexpr auto operator +=(Difference step_) noexcept -> Self_& {
			this->iterator += step_;
			return *this;
		}

		public: friend constexpr auto operator +(Self_ iterator_, Difference step_) noexcept -> Self_ {
			iterator_ += step_;
			return iterator_;
		}

		public: friend constexpr auto operator +(Difference step_, Self_ iterator_) noexcept -> Self_ {
			iterator_ += step_;
			return iterator_;
		}

		public: constexpr auto operator -=(Difference step_) noexcept -> Self_& {
			this->iterator -= step_;
			return *this;
		}

		public: friend constexpr auto operator -(Self_ iterator_, Difference step_) noexcept -> Self_ {
			iterator_ -= step_;
			return iterator_;
		}

		public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Self_ {
			return left_.iterator - right_.iterator;
		}

		public: constexpr auto operator *() const noexcept -> Reference {
			return ::dcool::core::dereference(this->iterator);
		}

		public: constexpr auto operator ->() const noexcept -> Pointer {
			return ::dcool::core::rawPointer(this->iterator);
		}

		public: friend auto operator <=>(Self_ const&, Self_ const&) noexcept -> ::dcool::core::StrongOrdering = default;
		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator <(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator >(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator <=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator >=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;

		public: constexpr operator Iterator() const noexcept {
			return this->iterator;
		}
	};

	template <typename T_> concept InputIterator = ::std::input_iterator<::dcool::core::StandardIterator<T_>>;
	template <typename T_, typename ToOutputT_> concept OutputIterator = ::std::output_iterator<
		::dcool::core::StandardIterator<T_>, ToOutputT_
	>;
	template <typename T_> concept ForwardIterator = ::std::forward_iterator<::dcool::core::StandardIterator<T_>>;
	template <typename T_> concept BidirectionalIterator = ::std::bidirectional_iterator<::dcool::core::StandardIterator<T_>>;
	template <typename T_> concept RandomAccessIterator = ::std::random_access_iterator<::dcool::core::StandardIterator<T_>>;
	template <typename T_> concept ContiguousIterator = ::std::contiguous_iterator<::dcool::core::StandardIterator<T_>>;

	template <typename T_> constexpr ::dcool::core::Boolean isReversedContiguousIterator = false;

	template <
		::dcool::core::ContiguousIterator T_
	> constexpr ::dcool::core::Boolean isReversedContiguousIterator<::std::reverse_iterator<T_>> = true;

	template <typename T_> concept ReversedContiguousIterator = ::dcool::core::isReversedContiguousIterator<T_>;

	template <typename T_> concept RandomAccessRange = ::std::ranges::random_access_range<T_>;
	template <typename T_> concept BorrowedRange = ::std::ranges::borrowed_range<T_>;

	enum class IterationDirection {
		forward,
		backward
	};

	namespace detail_ {
		template <::dcool::core::RandomAccessIterator IteratorT_> constexpr auto rangeLength_(
			IteratorT_ begin_, IteratorT_ end_
		) noexcept -> ::dcool::core::IteratorDifferenceType<IteratorT_> {
			return end_ - begin_;
		}

		template <typename IteratorT_> constexpr auto rangeLength_(
			IteratorT_ begin_, IteratorT_ end_
		) noexcept -> ::dcool::core::IteratorDifferenceType<IteratorT_> {
			::dcool::core::IteratorDifferenceType<IteratorT_> result_ = 0;;
			while (begin_ != end_) {
				++begin_;
				++result_;
			}
			return result_;
		}
	}

	template <::std::input_or_output_iterator IteratorT_> constexpr auto rangeLength(
		IteratorT_ begin_, IteratorT_ end_
	) noexcept -> ::dcool::core::IteratorDifferenceType<IteratorT_> {
		return ::dcool::core::detail_::rangeLength_(begin_, end_);
	}

	template <
		::dcool::core::ForwardIterator IteratorT_
	> constexpr void batchDestruct(IteratorT_ begin_, IteratorT_ end_) noexcept {
		using Value_ = ::dcool::core::IteratorValueType<IteratorT_>;
		while (begin_ != end_) {
			::dcool::core::dereference(begin_).~Value_();
			++begin_;
		}
	}

	struct RangeInputTag {
	};

	constexpr RangeInputTag rangeInput;

	template <::dcool::core::BidirectionalIterator IteratorT_> constexpr auto previousOf(IteratorT_ iterator_) -> IteratorT_ {
		--iterator_;
		return iterator_;
	}

	template <typename ToAccessT_> constexpr auto beginOf(ToAccessT_& toAccess_) noexcept(noexcept(::std::begin(toAccess_))) {
		return ::std::begin(toAccess_);
	}

	template <typename ToAccessT_> constexpr auto endOf(ToAccessT_& toAccess_) noexcept(noexcept(::std::end(toAccess_))) {
		return ::std::end(toAccess_);
	}

	namespace detail_ {
		template <typename T_> concept DirectlyFrontAccessible_ = requires (T_ toAccess_) {
			{ toAccess_.front() } -> ::dcool::core::Reference;
		};

		template <typename ToAccessT_> constexpr auto frontOf_(ToAccessT_& toAccess_) {
			return ::dcool::core::dereference(::dcool::core::beginOf(toAccess_));
		}

		template <::dcool::core::detail_::DirectlyFrontAccessible_ ToAccessT_> constexpr auto frontOf_(ToAccessT_& toAccess_) {
			return toAccess_.front();
		}
	}

	template <typename T_> concept FrontAccessible = requires (T_& toAccess_) {
		{ ::dcool::core::detail_::frontOf_(toAccess_) } -> ::dcool::core::Reference;
	};

	template <::dcool::core::FrontAccessible ToAccessT_> constexpr auto frontOf(ToAccessT_& toAccess_) noexcept(
		noexcept(::dcool::core::detail_::frontOf_(toAccess_))
	) {
		return ::dcool::core::detail_::frontOf_(toAccess_);
	}

	namespace detail_ {
		template <typename T_> concept DirectlyBackAccessible_ = requires (T_ toAccess_) {
			{ toAccess_.back() } -> ::dcool::core::Reference;
		};

		template <typename ToAccessT_> constexpr auto backOf_(ToAccessT_& toAccess_) {
			return ::dcool::core::dereference(::dcool::core::previousOf(::dcool::core::endOf(toAccess_)));
		}

		template <::dcool::core::detail_::DirectlyFrontAccessible_ ToAccessT_> constexpr auto backOf_(ToAccessT_& toAccess_) {
			return toAccess_.back();
		}
	}

	template <typename T_> concept BackAccessible = requires (T_& toAccess_) {
		{ ::dcool::core::detail_::backOf_(toAccess_) } -> ::dcool::core::Reference;
	};

	template <::dcool::core::BackAccessible ToAccessT_> constexpr auto backOf(ToAccessT_& toAccess_) noexcept(
		noexcept(::dcool::core::detail_::backOf_(toAccess_))
	) {
		return ::dcool::core::detail_::backOf_(toAccess_);
	}

	namespace detail_ {
		template <
			::dcool::core::ExceptionSafetyStrategy strategyC_,
			typename ValueT_,
			typename SourceIteratorT_,
			typename DestinationIteratorT_
		> constexpr auto batchRelocateForward_(
			SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
		) -> DestinationIteratorT_ {
			DestinationIteratorT_ result_;
			if constexpr (::dcool::core::atAnyCost(strategyC_) && (!noexcept(::std::uninitialized_move(begin_, end_, destination_)))) {
				result_ = ::std::uninitialized_copy(begin_, end_, destination_);
			} else {
				try {
					result_ = ::std::uninitialized_move(begin_, end_, destination_);
				} catch (...) {
					::dcool::core::goWeak<strategyC_>();
					throw;
				}
			}
			::dcool::core::batchDestruct(begin_, end_);
			return result_;
		}

		template <
			::dcool::core::ExceptionSafetyStrategy strategyC_,
			::dcool::core::TriviallyRelocatable ValueT_,
			::dcool::core::ContiguousIterator SourceIteratorT_,
			::dcool::core::ContiguousIterator DestinationIteratorT_
		> constexpr auto batchRelocateForward_(
			SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
		) noexcept -> DestinationIteratorT_ {
			auto length_ = end_ - begin_;
			::dcool::core::Size sizeToCopy_ = static_cast<::dcool::core::Size>(end_ - begin_) * sizeof(ValueT_);
			::std::memmove(::dcool::core::rawPointer(begin_), ::dcool::core::rawPointer(destination_), sizeToCopy_);
			return destination_ + static_cast<::dcool::core::IteratorDifferenceType<DestinationIteratorT_>>(length_);
		}

		template <
			::dcool::core::ExceptionSafetyStrategy strategyC_,
			::dcool::core::TriviallyRelocatable ValueT_,
			::dcool::core::ReversedContiguousIterator SourceIteratorT_,
			::dcool::core::ReversedContiguousIterator DestinationIteratorT_
		> constexpr auto batchRelocateForward_(
			SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
		) noexcept -> DestinationIteratorT_ {
			auto length_ = end_ - begin_;
			DestinationIteratorT_ destinationEnd_ =
				destination_ + static_cast<::dcool::core::IteratorDifferenceType<DestinationIteratorT_>>(length_)
			;
			::dcool::core::Size sizeToCopy_ = static_cast<::dcool::core::Size>(end_ - begin_) * sizeof(ValueT_);
			::std::memmove(::dcool::core::rawPointer(end_ - 1), ::dcool::core::rawPointer(destinationEnd_ - 1), sizeToCopy_);
			return destinationEnd_;
		}
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
		return ::dcool::core::detail_::batchRelocateForward_<
			strategyC_, ::dcool::core::IteratorValueType<SourceIteratorT_>
		>(begin_, end_, destination_);
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

	template <
		::dcool::core::ExceptionSafetyStrategy strategyC_,
		::dcool::core::ForwardIterator SourceIteratorT_,
		::dcool::core::ForwardIterator DestinationIteratorT_
	> constexpr auto batchRelocate(
		SourceIteratorT_ begin_, SourceIteratorT_ end_, DestinationIteratorT_ destination_
	) noexcept(noexcept(::dcool::core::batchRelocateForward<strategyC_>(begin_, end_, destination_))) -> DestinationIteratorT_ {
		return ::dcool::core::batchRelocateForward<strategyC_>(begin_, end_, destination_);
	}

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
