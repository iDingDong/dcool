#ifndef DCOOL_CORE_FOLDED_POINTER_HPP_INCLUDED_
#	define DCOOL_CORE_FOLDED_POINTER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/raw_pointer_operation.hpp>

#	include <dcool/config.h>

#	include <limits>

#	define DCOOL_POINTER_FOLDABLE_SIZE ((DCOOL_POINTER_HIGH_FOLDABLE_SIZE) + (DCOOL_POINTER_LOW_FOLDABLE_SIZE))

namespace dcool::core {
	namespace detail_ {
#	if DCOOLER_ENABLE_POINTER_FOLDING_BY_ALIGNMENT
		template <typename T_> constexpr ::dcool::core::Boolean pointerUnfoldableByAlignment_ = (alignof(T_) <= 1);
		template <> constexpr ::dcool::core::Boolean pointerUnfoldableByAlignment_<void> = true;
#	else
		template <typename T_> constexpr ::dcool::core::Boolean pointerUnfoldableByAlignment_ = true;
#	endif
		template <typename T_> concept PointerUnfoldableByAlignment_ = ::dcool::core::detail_::pointerUnfoldableByAlignment_<T_>;
	}

	template <typename ValueT_, typename ConfigT_ = ::dcool::core::Empty<>> struct FoldedPointer {
		private: using Self_ = FoldedPointer<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: using Pointer = Value*;
		private: using Underlying_ = ::dcool::core::PointerInteger;
		public: static constexpr ::dcool::core::Length foldedSize = ::std::bit_width(alignof(Value)) - 1;
		private: static constexpr ::dcool::core::Length valueSize_ =
			::std::numeric_limits<::dcool::core::PointerInteger>::digits - foldedSize
		;

		public: Underlying_ folded: foldedSize { 0 };
		private: Underlying_ m_value_: valueSize_ {
			::dcool::core::toPointerInteger(::dcool::core::nullPointerFor<Value>) >> foldedSize
		};

		public: constexpr FoldedPointer() noexcept = default;
		public: constexpr FoldedPointer(Self_ const&) noexcept = default;

		public: constexpr FoldedPointer(
			Pointer pointer_
		) noexcept: m_value_(::dcool::core::toPointerInteger(pointer_) >> foldedSize) {
		}

		public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;

		public: constexpr auto operator =(Pointer pointer_) noexcept -> Self_& {
			this->m_value_ = (::dcool::core::toPointerInteger(pointer_) >> foldedSize);
			return *this;
		}

		public: constexpr auto rawPointer() const noexcept -> Pointer {
			return ::dcool::core::fromPointerInteger(::dcool::core::fromPointerInteger<Value>(this->m_value_ << foldedSize));
		}

		public: constexpr auto deferenceSelf() const noexcept -> Value& {
			return *(this->rawPointer());
		}

		public: constexpr auto operator ->() const noexcept {
			return this->rawPointer();
		}

		public: constexpr auto operator *() const noexcept {
			return this->deferenceSelf();
		}

		public: constexpr auto operator +=(::dcool::core::Difference difference_)& noexcept -> Self_& {
			this->m_value_ = (::dcool::core::toPointerInteger(this->rawPointer() + difference_) >> foldedSize);
			return *this;
		}

		public: constexpr auto operator -=(::dcool::core::Difference difference_)& noexcept -> Self_& {
			this->m_value_ = (::dcool::core::toPointerInteger(this->rawPointer() - difference_) >> foldedSize);
			return *this;
		}

		public: constexpr operator Pointer() const noexcept {
			return this->rawPointer();
		}
	};

	template <
		::dcool::core::detail_::PointerUnfoldableByAlignment_ ValueT_, typename ConfigT_
	> struct FoldedPointer<ValueT_, ConfigT_> {
		private: using Self_ = FoldedPointer<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: using Pointer = Value*;
		public: static constexpr ::dcool::core::Length foldedSize = 0;

		private: Pointer m_pointer_ = ::dcool::core::nullPointer;

		// A bug prevent us from using 'Self_' or 'FoldedPointer<ValueT_, ConfigT_>' for the copy constructor.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr FoldedPointer() noexcept = default;
		public: constexpr FoldedPointer(FoldedPointer const&) noexcept = default;

		public: constexpr FoldedPointer(Pointer pointer_) noexcept: m_pointer_(pointer_) {
		}

		// A bug prevent us from using 'Self_' or 'FoldedPointer<ValueT_, ConfigT_>' for the copy assignment operator.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr auto operator =(FoldedPointer const&) noexcept -> Self_& = default;

		public: constexpr auto operator =(Pointer pointer_) noexcept -> Self_& {
			this->m_pointer_ = pointer_;
			return *this;
		}

		public: constexpr auto rawPointer() const noexcept -> Pointer {
			return this->m_pointer_;
		}

		public: constexpr auto deferenceSelf() const noexcept -> Value& {
			return *(this->rawPointer());
		}

		public: constexpr auto operator ->() const noexcept {
			return this->rawPointer();
		}

		public: constexpr auto operator *() const noexcept {
			return this->deferenceSelf();
		}

		public: constexpr auto operator +=(::dcool::core::Difference difference_)& noexcept -> Self_& {
			this->m_pointer_ += difference_;
			return *this;
		}

		public: constexpr auto operator -=(::dcool::core::Difference difference_)& noexcept -> Self_& {
			this->m_pointer_ += difference_;
			return *this;
		}

		public: constexpr operator Pointer() const noexcept {
			return this->rawPointer();
		}
	};
}

#endif
