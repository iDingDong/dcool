#ifndef DCOOL_CORE_CONTAMINATION_HPP_INCLUDED_
#	define DCOOL_CORE_CONTAMINATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/destruct.hpp>
#	include <dcool/core/range.hpp>

#	include <new>
#	include <iterator>

namespace dcool::core {
	using ::std::launder;

	template <typename ValueT_> struct ContaminatedPointer {
		private: using Self_ = ContaminatedPointer<ValueT_>;
		public: using Value = ValueT_;

		public: using Pointer = Value*;
		public: using Reference = Value&;
		public: using Difference = ::dcool::core::Difference;
		public: using IteratorCategory = ::dcool::core::ContiguousIteratorTag;

		public: using difference_type = Difference;
		public: using value_type = Value;
		public: using pointer = Pointer;
		public: using reference = Reference;
		public: using iterator_category = IteratorCategory;

		private: Pointer m_pointer_ = ::dcool::core::nullPointer;

		public: constexpr ContaminatedPointer() noexcept = default;
		public: constexpr ContaminatedPointer(Self_ const&) noexcept = default;
		public: constexpr ContaminatedPointer(Self_&&) noexcept = default;

		public: template <
			typename ValueT__
		> requires ::dcool::core::ConvertibleTo<ValueT__*, Pointer> constexpr ContaminatedPointer(
			::dcool::core::ContaminatedPointer<ValueT__> const& other_
		) noexcept: m_pointer_(other_.m_pointer_) {
		}

		public: constexpr explicit ContaminatedPointer(Pointer rawPointer_) noexcept: m_pointer_(rawPointer_) {
		}

		public: constexpr explicit ContaminatedPointer(
			::dcool::core::NullPointer
		) noexcept: m_pointer_(::dcool::core::nullPointer) {
		}

		public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
		public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

		public: template <
			typename ValueT__
		> requires ::dcool::core::ConvertibleTo<ValueT__*, Pointer> constexpr auto operator =(
			::dcool::core::ContaminatedPointer<ValueT__> const& other_
		) noexcept -> Self_& {
			this->m_pointer_ = other_.m_pointer_;
			return *this;
		}

		public: constexpr auto rawPointer() const noexcept -> Pointer {
			return this->m_pointer_;
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Reference {
			return *::dcool::core::launder(this->m_pointer_);
		}

		public: constexpr auto operator *() const noexcept -> Reference {
			return this->dereferenceSelf();
		}

		public: constexpr auto operator [](Difference difference_) const noexcept -> Reference {
			return this->next(difference_)->dereferenceSelf();
		}

		public: constexpr auto operator ->() const noexcept -> Pointer {
			return ::dcool::core::addressOf(this->dereferenceSelf());
		}

		public: template <typename... ArgumentTs__> constexpr void constructTarget(ArgumentTs__&&... parameters_) const noexcept {
			new (this->rawPointer()) Value(::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr void destructTarget() const noexcept {
			::dcool::core::destruct(this->dereferenceSelf())->~Value();
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			++(this->m_pointer_);
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_ = *this;
			++(this->m_pointer_);
			return result_;
		}

		public: constexpr auto operator --() noexcept -> Self_& {
			--(this->m_pointer_);
			return *this;
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_ = *this;
			--(this->m_pointer_);
			return result_;
		}

		public: constexpr auto operator +=(Difference step_) noexcept -> Self_& {
			this->m_pointer_ += step_;
			return *this;
		}

		public: friend constexpr auto operator +(Self_ pointer_, Difference step_) noexcept -> Self_ {
			pointer_ += step_;
			return pointer_;
		}

		public: friend constexpr auto operator +(Difference step_, Self_ pointer_) noexcept -> Self_ {
			pointer_ += step_;
			return pointer_;
		}

		public: constexpr auto operator -=(Difference step_) noexcept -> Self_& {
			this->m_pointer_ -= step_;
			return *this;
		}

		public: friend constexpr auto operator -(Self_ pointer_, Difference step_) noexcept -> Self_ {
			pointer_ -= step_;
			return pointer_;
		}

		public: friend constexpr auto operator -(Self_ const& left_, Self_ const& right_) noexcept -> Difference {
			return left_.m_pointer_ - right_.m_pointer_;
		}

		public: friend constexpr auto operator <=>(Self_ const&, Self_ const&) noexcept = default;
		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator <(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator >(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator <=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator >=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};
}

namespace std {
	template <typename ValueT_> struct pointer_traits<::dcool::core::ContaminatedPointer<ValueT_>> {
		using pointer = ::dcool::core::ContaminatedPointer<ValueT_>;
		using element_type = pointer::Value;
		using difference_type = pointer::Difference;

		static constexpr auto to_address(pointer const& pointer_) noexcept -> element_type* {
			return pointer_.rawPointer();
		}
	};

	template <typename ValueT_> struct pointer_traits<::std::reverse_iterator<::dcool::core::ContaminatedPointer<ValueT_>>> {
		using pointer = ::std::reverse_iterator<::dcool::core::ContaminatedPointer<ValueT_>>;
		using element_type = pointer::value_type;
		using difference_type = pointer::difference_type;

		static constexpr auto to_address(pointer const& pointer_) noexcept -> element_type* {
			auto base_ = pointer_.base();
			--base_;
			return base_.rawPointer();
		}
	};
}

#endif
