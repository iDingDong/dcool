#ifndef DCOOL_BINARY_BIN_VIEW_HPP_INCLUDED_
#	define DCOOL_BINARY_BIN_VIEW_HPP_INCLUDED_ 1

#	include <dcool/binary/bin.hpp>

#	include <dcool/core.hpp>

namespace dcool::binary {
	struct BinView {
		private: using Self_ = BinView;
		public: using Length = ::dcool::core::Length;
		public: using Index = ::dcool::core::Index;

		private: void const* m_begin_ = ::dcool::core::nullPointer;
		private: Length m_length_ = 0;

		public: constexpr BinView() noexcept = default;

		public: template <typename ConfigT_> constexpr BinView(
			::dcool::binary::Bin<ConfigT_> bin_
		) noexcept: BinView(bin_.value(), bin_.length()) {
		}

		public: constexpr BinView(void const* begin_, Length length_) noexcept: m_begin_(begin_), m_length_(length_) {
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->m_length_;
		}

		public: constexpr auto value() const noexcept -> void const* {
			return this->m_begin_;
		}

		public: constexpr auto sub(Index begin_, Index end_) const noexcept -> Self_ {
			return Self_(::dcool::core::stepByByte(this->value(), begin_), end_ - begin_);
		}

		public: constexpr void trimLeft(Length toTrim_) noexcept {
			this->m_begin_ = ::dcool::core::stepByByte(this->m_begin_, toTrim_);
			this->m_length_ -= toTrim_;
		}

		public: constexpr void trimRight(Length toTrim_) noexcept {
			this->m_length_ -= toTrim_;
		}
	};
}

#endif
