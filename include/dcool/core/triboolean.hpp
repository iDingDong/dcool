#ifndef DCOOL_CORE_TRIBOOLEAN_HPP_INCLUDED_
#	define DCOOL_CORE_TRIBOOLEAN_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	namespace detail_ {
		enum class TribooleanUnderlying_: unsigned char {
			determinateFalse_,
			determinateTrue_,
			indeterminate_
		};

		struct TribooleanMaker_;
	}

	struct Triboolean {
		private: using Self_ = Triboolean;
		friend ::dcool::core::detail_::TribooleanMaker_;

		private: using Underlying_ = ::dcool::core::detail_::TribooleanUnderlying_;

		public: Underlying_ underlying_;

		public: constexpr Triboolean(
			::dcool::core::Boolean value_
		) noexcept: underlying_(value_ ? Underlying_::determinateTrue_ : Underlying_::determinateFalse_) {
		}

		private: constexpr Triboolean(Underlying_ underlying_) noexcept: underlying_(underlying_) {
		}

		public: constexpr auto isDeterminate() const noexcept -> ::dcool::core::Boolean {
			return this->underlying_ != Underlying_::indeterminate_;
		}

		public: constexpr auto isDeterminateTrue() const noexcept -> ::dcool::core::Boolean {
			return this->underlying_ == Underlying_::determinateTrue_;
		}

		public: constexpr auto isDeterminateFalse() const noexcept -> ::dcool::core::Boolean {
			return this->underlying_ == Underlying_::determinateFalse_;
		}

		public: constexpr auto operator !() const noexcept -> ::dcool::core::Triboolean {
			if (this->isDeterminateFalse()) {
				return Underlying_::determinateTrue_;
			}
			if (this->isDeterminateTrue()) {
				return Underlying_::determinateFalse_;
			}
			return Underlying_::indeterminate_;
		}

		public: friend constexpr auto operator &&(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Triboolean {
			if (left_.isDeterminateFalse()) {
				return Underlying_::determinateFalse_;
			}
			if (left_.isDeterminateTrue()) {
				return right_;
			}
			return right_.isDeterminateFalse() ? Underlying_::determinateFalse_ : Underlying_::indeterminate_;
		}

		public: friend constexpr auto operator ||(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Triboolean {
			if (left_.isDeterminateFalse()) {
				return right_;
			}
			if (left_.isDeterminateTrue()) {
				return Underlying_::determinateTrue_;
			}
			return right_.isDeterminateTrue() ? Underlying_::determinateTrue_ : Underlying_::indeterminate_;
		}

		public: friend constexpr auto operator ==(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Triboolean {
			if (left_.isDeterminateFalse()) {
				return !right_;
			}
			if (left_.isDeterminateTrue()) {
				return right_;
			}
			return Underlying_::indeterminate_;
		}

		public: friend constexpr auto operator !=(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Triboolean {
			return !(left_ == right_);
		}
	};

	namespace detail_ {
		struct TribooleanMaker_ {
			static constexpr auto make_(
				::dcool::core::detail_::TribooleanUnderlying_ underlying_
			) noexcept -> ::dcool::core::Triboolean {
				return underlying_;
			}
		};
	}

	constexpr ::dcool::core::Triboolean determinateFalse = ::dcool::core::detail_::TribooleanMaker_::make_(
		::dcool::core::detail_::TribooleanUnderlying_::determinateFalse_
	);
	constexpr ::dcool::core::Triboolean determinateTrue = ::dcool::core::detail_::TribooleanMaker_::make_(
		::dcool::core::detail_::TribooleanUnderlying_::determinateTrue_
	);
	constexpr ::dcool::core::Triboolean indeterminate = ::dcool::core::detail_::TribooleanMaker_::make_(
		::dcool::core::detail_::TribooleanUnderlying_::indeterminate_
	);
}

#endif
