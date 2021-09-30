#ifndef DCOOL_CORE_RANGE_HPP_INCLUDED_
#	define DCOOL_CORE_RANGE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>
#	include <dcool/core/dereference.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/reverse_iterator.hpp>
#	include <dcool/core/type_difference_detector.hpp>
#	include <dcool/core/type_value_detector.hpp>
#	include <dcool/core/utility.hpp>

#	include <iterator>
#	include <ranges>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeIteratorCategory, ExtractedIteratorCategoryType, IteratorCategory)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::core::detail_, HasStandardTypeIteratorConcept_, ExtractedStandardIteratorConceptType_, iterator_concept
)

namespace dcool::core {
	using InputIteratorTag = ::std::input_iterator_tag;
	using OutputIteratorTag = ::std::output_iterator_tag;
	using RandomAccessIteratorTag = ::std::random_access_iterator_tag;
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

	template <typename IteratorT_> using IteratorCategoryType = ::dcool::core::ExtractedIteratorCategoryType<
		IteratorT_,
		::dcool::core::detail_::ExtractedStandardIteratorConceptType_<
			::std::iterator_traits<IteratorT_>, typename ::std::iterator_traits<IteratorT_>::iterator_category
		>
	>;

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
		public: using IteratorCategory = ::dcool::core::IteratorCategoryType<Iterator>;

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

		public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Difference {
			return left_.iterator - right_.iterator;
		}

		public: constexpr auto rawPointer() const noexcept -> Pointer {
			return ::dcool::core::rawPointerOf(this->iterator);
		}

		public: constexpr auto operator *() const noexcept -> Reference {
			return ::dcool::core::dereference(this->iterator);
		}

		public: constexpr auto operator ->() const noexcept -> Pointer {
			return this->rawPointer();
		}

		public: friend auto operator <=>(Self_ const&, Self_ const&) noexcept = default;
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

	struct RangeInputTag {
		constexpr explicit RangeInputTag() noexcept = default;
	};

	constexpr ::dcool::core::RangeInputTag rangeInput = ::dcool::core::RangeInputTag();

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

	template <
		::dcool::core::ForwardIterator LeftIteratorT_, ::dcool::core::ForwardIterator RightIteratorT_
	> constexpr auto iteratorsToSame(
		LeftIteratorT_ const& left_, RightIteratorT_ const& right_
	) noexcept -> ::dcool::core::Boolean {
		return ::dcool::core::rawPointerOf(left_) == ::dcool::core::rawPointerOf(right_);
	}

	template <::dcool::core::ForwardIterator IteratorT_> constexpr auto next(IteratorT_ iterator_) noexcept -> IteratorT_ {
		return ++iterator_;
	}

	template <::dcool::core::ForwardIterator IteratorT_> struct PlacementOutputIterator {
		private: using Self_ = PlacementOutputIterator<IteratorT_>;
		public: using Iterator = IteratorT_;

		public: using Difference = ::dcool::core::IteratorDifferenceType<Iterator>;
		public: using Value = ::dcool::core::IteratorValueType<Iterator>;
		public: using Pointer = ::dcool::core::IteratorPointerType<Iterator>;
		public: using Reference = ::dcool::core::IteratorReferenceType<Iterator>;
		public: using IteratorCategory = ::dcool::core::IteratorCategoryType<Iterator>;

		private: struct AssignmentHijacker_ {
			Iterator iterator;

			constexpr auto operator =(Value const& value_) {
				new (::dcool::core::rawPointerOf(this->iterator)) Value(value_);
				return *this;
			}

			constexpr auto operator =(Value&& value_) {
				new (::dcool::core::rawPointerOf(this->iterator)) Value(::dcool::core::move(value_));
				return *this;
			}
		};

		public: using difference_type = Difference;
		public: using value_type = Value;
		public: using pointer = Pointer;
		public: using reference = Reference;
		public: using iterator_category = IteratorCategory;

		private: mutable AssignmentHijacker_ m_iterator_;

		public: constexpr PlacementOutputIterator() noexcept = default;

		public: constexpr explicit PlacementOutputIterator(Iterator iterator_) noexcept: m_iterator_ { .iterator = iterator_ } {
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			++(this->m_iterator.iterator);
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_ = *this;
			++(this->m_iterator.iterator);
			return result_;
		}

		public: constexpr auto operator *() const noexcept -> AssignmentHijacker_& {
			return this->m_iterator_;
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;

		public: constexpr explicit operator Iterator() const noexcept {
			return this->m_iterator.iterator;
		}
	};
}

#endif
