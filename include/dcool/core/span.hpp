#ifndef DCOOL_CORE_SPAN_HPP_INCLUDED_
#	define DCOOL_CORE_SPAN_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/dereference.hpp>
#	include <dcool/core/integer.hpp>
#	include <dcool/core/range.hpp>

#	include <span>
#	include <limits>
#	include <stdexcept>

namespace dcool::core {
	inline constexpr ::dcool::core::Length dynamicExtent = ::std::dynamic_extent;

	template <typename ValueT_, ::dcool::core::Length extentC_> struct Span {
		private: using Self_ = Span<ValueT_, extentC_>;
		public: using Value = ValueT_;
		private: static constexpr ::dcool::core::Length extent_ = extentC_;
		static_assert(
			extent_ <= ::std::numeric_limits<::dcool::core::SignedMaxInteger>::max(),
			"No proper difference type available for 'dcool::core::Span'."
		);

		public: using Length = ::dcool::core::IntegerType<extent_>;
		public: using Index = Length;
		public: using Difference = ::dcool::core::IntegerType<extent_, -static_cast<::dcool::core::SignedMaxInteger>(extent_)>;
		public: using Iterator = Value*;
		public: using ConstIterator = Value const*;
		public: template <::dcool::core::Length lengthC__> using RawArrayOfLength = Value[lengthC__];
		public: static constexpr ::dcool::core::Boolean lengthFixed = true;
		public: static constexpr Length extent = static_cast<Length>(extent_);

		public: using RawArray = RawArrayOfLength<extent>;

		public: using value_type = Value;
		public: using size_type = Length;
		public: using difference_type = Difference;

		private: Value* m_begin_ = ::dcool::core::nullPointer;

		public: constexpr Span() noexcept = default;

		public: template <
			::dcool::core::ContiguousIterator IteratorT__
		> constexpr Span(IteratorT__ begin_) noexcept: m_begin_(::dcool::core::rawPointerOf(begin_)) {
		}

		public: template <
			::dcool::core::Length lengthC__
		> requires (lengthC__ >= extent) constexpr Span(RawArrayOfLength<lengthC__>& array_) noexcept: Self_(array_) {
		}

		public: template <::dcool::core::Length lengthC__> requires (
			Span<Value, lengthC__>::lengthFixed && Span<Value, lengthC__>::extent >= extent
		) constexpr Span(Span<Value, lengthC__> const& other_) noexcept: Self_(other_.begin()) {
		}

		public: consteval auto length() const noexcept -> Length {
			return extent;
		}

		public: constexpr auto begin() const noexcept -> Iterator {
			return this->m_begin_;
		}

		public: constexpr auto end() const noexcept -> Iterator {
			return this->begin() + this->length();
		}

		public: constexpr auto position(Index index_) const noexcept -> Iterator {
			return this->begin() + index_;
		}

		public: constexpr auto access(Index index_) const noexcept -> Value& {
			return ::dcool::core::dereference(this->position(index_));
		}

		public: constexpr auto operator[](Index index_) const noexcept -> Value& {
			return this->access(index_);
		}
	};

	template <typename ValueT_> struct Span<ValueT_, ::dcool::core::dynamicExtent> {
		private: using Self_ = Span<ValueT_, ::dcool::core::dynamicExtent>;
		public: using Value = ValueT_;
		public: static constexpr ::dcool::core::Length extent = ::dcool::core::dynamicExtent;

		public: using Length = ::dcool::core::Length;
		public: using Index = Length;
		public: using Difference = ::dcool::core::Difference;
		public: using Iterator = Value*;
		public: using ConstIterator = Value const*;
		public: template <::dcool::core::Length lengthC__> using RawArrayOfLength = Value[lengthC__];
		public: static constexpr ::dcool::core::Boolean lengthFixed = false;

		public: using value_type = Value;
		public: using size_type = Length;
		public: using difference_type = Difference;

		private: Value* m_begin_ = ::dcool::core::nullPointer;
		private: Length m_length_ = 0;

		public: constexpr Span() noexcept = default;

		public: template <::dcool::core::ContiguousIterator IteratorT__> constexpr Span(
			IteratorT__ begin_, Length length_
		): m_begin_(::dcool::core::rawPointerOf(begin_)), m_length_(length_) {
		}

		public: template <::dcool::core::ContiguousIterator IteratorT__> constexpr Span(
			IteratorT__ begin_, IteratorT__ end_
		): Self_(begin_, static_cast<Length>(end_ - begin_)) {
		}

		public: template <
			::dcool::core::Length lengthC__
		> constexpr Span(RawArrayOfLength<lengthC__>& array_) noexcept: Self_(array_, lengthC__) {
		}

		public: template <
			::dcool::core::Length lengthC__
		> constexpr Span(Span<Value, lengthC__> const& other_) noexcept: Self_(other_.begin(), other_.length()) {
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->m_length_;
		}

		public: constexpr auto begin() const noexcept -> Iterator {
			return this->m_begin_;
		}

		public: constexpr auto end() const noexcept -> Iterator {
			return this->begin() + this->length();
		}

		public: constexpr auto position(Index index_) const noexcept -> Iterator {
			return this->begin() + index_;
		}

		public: constexpr auto access(Index index_) const noexcept -> Value& {
			return ::dcool::core::dereference(this->position(index_));
		}

		public: constexpr auto operator[](Index index_) const noexcept -> Value& {
			return this->access(index_);
		}
	};
}

#endif
