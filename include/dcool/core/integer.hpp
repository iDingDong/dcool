#ifndef DCOOL_CORE_INTEGER_HPP_INCLUDED_
#	define DCOOL_CORE_INTEGER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/min_max.hpp>
#	include <dcool/core/storage.hpp>

#	include <array>
#	include <bit>
#	include <limits>
#	include <type_traits>

namespace dcool::core {
	namespace detail_ {
		template <
			auto maxC_, typename IntegerT_, ::dcool::core::Boolean fitC_, typename... RestIntegerTs_
		> struct TryUnsignedAsIntegerHelper_ {
			using Result_ = IntegerT_;
		};

		template <
			auto maxC_, typename IntegerT_, typename NextIntegerT_, typename... RestIntegerTs_
		> struct TryUnsignedAsIntegerHelper_<maxC_, IntegerT_, false, NextIntegerT_, RestIntegerTs_...> {
			using Result_ = ::dcool::core::detail_::TryUnsignedAsIntegerHelper_<
				maxC_, NextIntegerT_, maxC_ <= ::std::numeric_limits<IntegerT_>::max(), RestIntegerTs_...
			>::Result_;
		};

		template <auto maxC_, typename IntegerT_> struct TryUnsignedAsIntegerHelper_<maxC_, IntegerT_, false> {
			using Result_ = void;
		};

		template <auto maxC_, typename IntegerT_, typename... RestIntegerTs_> struct TryUnsignedInteger_ {
			using Result_ = ::dcool::core::detail_::TryUnsignedAsIntegerHelper_<
				maxC_, IntegerT_, maxC_ <= ::std::numeric_limits<IntegerT_>::max(), RestIntegerTs_...
			>::Result_;
		};

		template <
			auto maxC_, auto minC_, typename IntegerT_, ::dcool::core::Boolean fitC_, typename... RestIntegerTs_
		> struct TrySignedAsIntegerHelper_ {
			using Result_ = IntegerT_;
		};

		template <
			auto maxC_, auto minC_, typename IntegerT_, typename NextIntegerT_, typename... RestIntegerTs_
		> struct TrySignedAsIntegerHelper_<maxC_, minC_, IntegerT_, false, NextIntegerT_, RestIntegerTs_...> {
			using Result_ = ::dcool::core::detail_::TrySignedAsIntegerHelper_<
				maxC_,
				minC_,
				NextIntegerT_,
				maxC_ <= ::std::numeric_limits<IntegerT_>::max() && minC_ >= ::std::numeric_limits<IntegerT_>::min(),
				RestIntegerTs_...
			>::Result_;
		};

		template <auto maxC_, auto minC_, typename IntegerT_> struct TrySignedAsIntegerHelper_<maxC_, minC_, IntegerT_, false> {
		};

		template <auto maxC_, auto minC_, typename IntegerT_, typename... RestIntegerTs_> struct TrySignedInteger_ {
			using Result_ = ::dcool::core::detail_::TrySignedAsIntegerHelper_<
				maxC_,
				minC_,
				IntegerT_,
				maxC_ <= ::std::numeric_limits<IntegerT_>::max() && minC_ >= ::std::numeric_limits<IntegerT_>::min(),
				RestIntegerTs_...
			>::Result_;
		};

		template <auto maxC_, auto minC_, ::dcool::core::Boolean isUnsignedC_ = (minC_ >= 0)> struct IntegerType_ {
			static_assert(maxC_ >= minC_);
			using Result_ = ::dcool::core::detail_::TryUnsignedInteger_<
				maxC_, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long, UnsignedMaxInteger
			>::Result_;
		};

		template <auto maxC_, auto minC_> struct IntegerType_<maxC_, minC_, false> {
			static_assert((maxC_ >= 0 && minC_ <= 0) || maxC_ >= minC_);
			using Result_ = ::dcool::core::detail_::TrySignedInteger_<
				maxC_, minC_, char, short, int, long, long long, SignedMaxInteger
			>::Result_;
		};
	}

	template <
		auto maxC_, auto minC_ = 0
	> requires (::dcool::core::Integral<decltype(maxC_)> && ::dcool::core::Integral<decltype(minC_)>) using IntegerType =
		::dcool::core::detail_::IntegerType_<maxC_, minC_>::Result_
	;

	namespace detail_ {
		template <
			::dcool::core::Size maxSizeC_,
			::dcool::core::Alignment maxAlignmentC_,
			typename IntegerT_,
			::dcool::core::Boolean fitC_,
			typename... RestIntegerTs_
		> struct StorableIntegerHelper_ {
			using Result_ = IntegerT_;
		};

		template <
			::dcool::core::Size maxSizeC_,
			::dcool::core::Alignment maxAlignmentC_,
			typename IntegerT_,
			typename NextIntegerT_,
			typename... RestIntegerTs_
		> struct StorableIntegerHelper_<maxSizeC_, maxAlignmentC_, IntegerT_, false, NextIntegerT_, RestIntegerTs_...> {
			using Result_ = ::dcool::core::detail_::StorableIntegerHelper_<
				maxSizeC_,
				maxAlignmentC_,
				NextIntegerT_,
				sizeof(NextIntegerT_) <= maxSizeC_ && alignof(NextIntegerT_) <= maxAlignmentC_,
				RestIntegerTs_...
			>::Result_;
		};

		template <
			::dcool::core::Size maxSizeC_, ::dcool::core::Alignment maxAlignmentC_, typename IntegerT_
		> struct StorableIntegerHelper_<maxSizeC_, maxAlignmentC_, IntegerT_, false> {
			using Result_ = void;
		};
	}

	template <
		::dcool::core::Size maxSizeC_, ::dcool::core::Alignment maxAlignmentC_ = ::dcool::core::defaultAlignmentFor<maxSizeC_>
	> using StorableUnsignedIntegerType = ::dcool::core::detail_::StorableIntegerHelper_<
		maxSizeC_,
		maxAlignmentC_,
		::dcool::core::UnsignedMaxInteger,
		sizeof(::dcool::core::UnsignedMaxInteger) <= maxSizeC_ && alignof(::dcool::core::UnsignedMaxInteger) <= maxAlignmentC_,
		unsigned long long,
		unsigned long,
		unsigned,
		unsigned short,
		unsigned char
	>::Result_;

	template <
		::dcool::core::Size maxSizeC_, ::dcool::core::Alignment maxAlignmentC_ = ::dcool::core::defaultAlignmentFor<maxSizeC_>
	> using StorableSignedIntegerType = ::dcool::core::detail_::StorableIntegerHelper_<
		maxSizeC_,
		maxAlignmentC_,
		::dcool::core::SignedMaxInteger,
		sizeof(::dcool::core::SignedMaxInteger) <= maxSizeC_ && alignof(::dcool::core::SignedMaxInteger) <= maxAlignmentC_,
		long long,
		long,
		int,
		short,
		signed char
	>::Result_;

	template <typename T_, typename SubT_> concept SuperSetOfNumeric =
		(::dcool::core::SignedIntegral<T_> || ::dcool::core::UnsignedIntegral<SubT_>) &&
		::std::numeric_limits<T_>::min() <= ::std::numeric_limits<SubT_>::min() &&
		::std::numeric_limits<SubT_>::max() <= ::std::numeric_limits<T_>::max()
	;

	template <::dcool::core::Integral SourceT_> using SignessRemovedType = ::std::make_unsigned_t<SourceT_>;
	template <::dcool::core::Integral SourceT_> using SignessAddedType = ::std::make_signed_t<SourceT_>;

	template <typename IntegerT_> concept RepresentableBySigned = ::dcool::core::SuperSetOfNumeric<
		::dcool::core::SignedMaxInteger, IntegerT_
	>;

	template <
		::dcool::core::RepresentableBySigned SourceT_
	> using NegativeRepresentableSignedType = ::dcool::core::IntegerType<
		::std::numeric_limits<SourceT_>::max(),
		-static_cast<::dcool::core::SignedMaxInteger>(::std::numeric_limits<SourceT_>::max())
	>;

	template <::dcool::core::Integral IntegerT_> constexpr ::dcool::core::Length widthOfInteger =
		::std::numeric_limits<IntegerT_>::digits + (::dcool::core::SignedIntegral<IntegerT_> ? 1 : 0)
	;

	namespace detail_ {
		template <
			typename T_, ::dcool::core::Length leastDigitCountC_
		> concept DigitsHoldable_ = ::dcool::core::Integral<T_> && (leastDigitCountC_ <= ::dcool::core::widthOfInteger<T_>);

		template <
			typename T_, ::dcool::core::Length maxDigitCountC_
		> concept HoldableByDigits_ = ::dcool::core::Integral<T_> && (maxDigitCountC_ >= ::dcool::core::widthOfInteger<T_>);

		template <
			::dcool::core::Length widthC_, typename FirstT_, typename... CandidatesT_
		> struct SingleUnitIntegerUnderlying_ {
			using Result_ = ::dcool::core::detail_::SingleUnitIntegerUnderlying_<widthC_, CandidatesT_...>::Result_;
		};

		template <
			::dcool::core::Length widthC_,
			::dcool::core::detail_::DigitsHoldable_<widthC_> FirstT_,
			typename... CandidatesT_
		> struct SingleUnitIntegerUnderlying_<widthC_, FirstT_, CandidatesT_...> {
			using Result_ = FirstT_;
		};

		template <
			::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_
		> using SingleUnitIntegerUnderlyingType_ = ::dcool::core::ConditionalType<
			hasSignessC_,
			typename ::dcool::core::detail_::SingleUnitIntegerUnderlying_<
				widthC_,
				signed char,
				signed short,
				signed int,
				signed long,
				signed long long,
				::dcool::core::SignedMaxInteger
			>::Result_,
			typename ::dcool::core::detail_::SingleUnitIntegerUnderlying_<
				widthC_,
				unsigned char,
				unsigned short,
				unsigned int,
				unsigned long,
				unsigned long long,
				::dcool::core::UnsignedMaxInteger
			>::Result_
		>;
	}

	namespace detail_ {
		template <
			::dcool::core::Integral IntegerT_
		> constexpr auto bitFieldMax_(::dcool::core::Length width_) noexcept -> IntegerT_ {
			IntegerT_ result_;
			if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
				DCOOL_CORE_ASSERT(width_ > 0);
				IntegerT_ maxCeil_ = (IntegerT_(1) << (width_ - 1));
				result_ = maxCeil_ + (maxCeil_ - 1);
			} else {
				result_ = ~((~(IntegerT_(0))) << width_);
			}
			return result_;
		}

		template <
			::dcool::core::Integral IntegerT_
		> constexpr auto bitFieldMin_(::dcool::core::Length width_) noexcept -> IntegerT_ {
			if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
				return -::dcool::core::detail_::bitFieldMax_<IntegerT_>(width_) - 1;
			}
			return 0;
		}
	}

	template <
		::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_
	> requires (
		::dcool::core::UnsignedIntegral<IntegerT_> || widthC_ > 0
	) constexpr IntegerT_ bitFieldMax = ::dcool::core::detail_::bitFieldMax_<IntegerT_>(widthC_);

	template <
		::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_
	> requires (
		::dcool::core::UnsignedIntegral<IntegerT_> || widthC_ > 0
	) constexpr IntegerT_ bitFieldMin = ::dcool::core::detail_::bitFieldMin_<IntegerT_>(widthC_);

	template <
		::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_, typename ConfigT_ = ::dcool::core::Empty<>
	> struct Integer;

	template <
		::dcool::core::Length widthC_, typename ConfigT_ = ::dcool::core::Empty<>
	> using UnsignedInteger = ::dcool::core::Integer<false, widthC_>;

	template <
		::dcool::core::Length widthC_, typename ConfigT_ = ::dcool::core::Empty<>
	> using SignedInteger = ::dcool::core::Integer<true, widthC_>;

	constexpr ::dcool::core::Length builtInIntegerUnderlyingLimit = ::dcool::core::widthOfInteger<::dcool::core::UnsignedMaxInteger>;

	template <typename T_> constexpr ::dcool::core::Boolean isExtendedInteger = false;
	template <
		::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_, typename ConfigT_
	> constexpr ::dcool::core::Boolean isExtendedInteger<::dcool::core::Integer<hasSignessC_, widthC_, ConfigT_>> = true;

	template <typename T_> concept ExtendedInteger = ::dcool::core::isExtendedInteger<T_>;
	template <typename T_> concept ExtendedSignedInteger = ::dcool::core::ExtendedInteger<T_> && T_::hasSigness;
	template <
		typename T_
	> concept ExtendedUnsignedInteger = ::dcool::core::ExtendedInteger<T_> && (!::dcool::core::ExtendedSignedInteger<T_>);
	template <typename T_, typename ConfigT_> concept ExtendedIntegerOfConfig =
		::dcool::core::ExtendedInteger<T_> && ::dcool::core::SameAs<typename T_::Config, ConfigT_>
	;

	template <typename T_> concept GeneralInteger = ::dcool::core::Integral<T_> || ::dcool::core::ExtendedInteger<T_>;
	template <
		typename T_
	> concept GeneralSignedInteger = ::dcool::core::SignedIntegral<T_> || ::dcool::core::ExtendedSignedInteger<T_>;
	template <
		typename T_
	> concept GeneralUnsignedInteger = ::dcool::core::UnsignedIntegral<T_> || ::dcool::core::ExtendedUnsignedInteger<T_>;

	template <
		::dcool::core::GeneralInteger T_
	> constexpr ::dcool::core::Length generalWidthOfInteger = T_::width;

	template <
		::dcool::core::Integral T_
	> constexpr ::dcool::core::Length generalWidthOfInteger<T_> = ::dcool::core::widthOfInteger<T_>;

	namespace detail_ {
		template <
			::dcool::core::GeneralSignedInteger T_, ::dcool::core::GeneralSignedInteger ToRepresentT_
		> constexpr auto canRepresent_() noexcept -> ::dcool::core::Boolean {
			return ::dcool::core::generalWidthOfInteger<T_> >= ::dcool::core::generalWidthOfInteger<ToRepresentT_>;
		}

		template <
			::dcool::core::GeneralSignedInteger T_, ::dcool::core::GeneralUnsignedInteger ToRepresentT_
		> constexpr auto canRepresent_() noexcept -> ::dcool::core::Boolean {
			return ::dcool::core::generalWidthOfInteger<T_> > ::dcool::core::generalWidthOfInteger<ToRepresentT_>;
		}

		template <
			::dcool::core::GeneralUnsignedInteger T_, ::dcool::core::GeneralSignedInteger ToRepresentT_
		> constexpr auto canRepresent_() noexcept -> ::dcool::core::Boolean {
			return false;
		}

		template <
			::dcool::core::GeneralUnsignedInteger T_, ::dcool::core::GeneralUnsignedInteger ToRepresentT_
		> constexpr auto canRepresent_() noexcept -> ::dcool::core::Boolean {
			return ::dcool::core::generalWidthOfInteger<T_> >= ::dcool::core::generalWidthOfInteger<ToRepresentT_>;
		}
	}

	template <
		::dcool::core::GeneralInteger T_, ::dcool::core::GeneralInteger ToRepresentT_
	> constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::detail_::canRepresent_<T_, ToRepresentT_>();

	template <::dcool::core::GeneralInteger IntegerT_> using ExtendedIntegerType = ::dcool::core::ConditionalType<
		::dcool::core::ExtendedInteger<IntegerT_>,
		IntegerT_,
		::dcool::core::Integer<::dcool::core::GeneralSignedInteger<IntegerT_>, ::dcool::core::generalWidthOfInteger<IntegerT_>>
	>;

	template <
		typename ConfigT_, ::dcool::core::GeneralInteger... IntegerTs_
	> constexpr ::dcool::core::Boolean isCompatibleToConfigGeneralInteger = true;

	template <
		typename ConfigT_, typename FirstT_, ::dcool::core::GeneralInteger... RestTs_
	> constexpr ::dcool::core::Boolean isCompatibleToConfigGeneralInteger<ConfigT_, FirstT_, RestTs_...> =
		(::dcool::core::Integral<FirstT_> || ::dcool::core::ExtendedIntegerOfConfig<FirstT_, ConfigT_>) &&
		::dcool::core::isCompatibleToConfigGeneralInteger<ConfigT_, RestTs_...>
	;

	template <
		::dcool::core::GeneralInteger IntegerT_, typename ConfigT_
	> requires (
		::dcool::core::isCompatibleToConfigGeneralInteger<ConfigT_, IntegerT_>
	) using ExtendedIntegerTypeOfConfig = ::dcool::core::Integer<
		::dcool::core::GeneralSignedInteger<IntegerT_>, ::dcool::core::generalWidthOfInteger<IntegerT_>, ConfigT_
	>;

	namespace detail_ {
		template <::dcool::core::GeneralInteger... IntegerTs_> constexpr ::dcool::core::Boolean commonSigness_ = false;
		template <
			::dcool::core::GeneralInteger FirstT_, ::dcool::core::GeneralInteger... RestTs_
		> constexpr ::dcool::core::Boolean commonSigness_<FirstT_, RestTs_...> =
			::dcool::core::GeneralSignedInteger<FirstT_> || ::dcool::core::detail_::commonSigness_<RestTs_...>
		;

		template <
			::dcool::core::Boolean hasSignessC_, ::dcool::core::GeneralInteger... IntegerTs_
		> constexpr ::dcool::core::Length commonDigitCountHelper_ = (hasSignessC_ ? 1 : 0);

		template <
			::dcool::core::Boolean hasSignessC_, ::dcool::core::GeneralInteger FirstT_, ::dcool::core::GeneralInteger... RestTs_
		> constexpr ::dcool::core::Length commonDigitCountHelper_<hasSignessC_, FirstT_, RestTs_...> =
			::dcool::core::max(
				::dcool::core::generalWidthOfInteger<FirstT_> + ((hasSignessC_ && ::dcool::core::GeneralUnsignedInteger<FirstT_>) ? 1 : 0),
				::dcool::core::detail_::commonDigitCountHelper_<hasSignessC_, RestTs_...>
			)
		;

		template <
			::dcool::core::GeneralInteger... IntegerTs_
		> constexpr ::dcool::core::Length commonDigitCount_ = ::dcool::core::detail_::commonDigitCountHelper_<
			::dcool::core::detail_::commonSigness_<IntegerTs_...>, IntegerTs_...
		>;

		template <::dcool::core::GeneralInteger... IntegerTs_> struct FirstExtendedIntegerConfig_ {
			using Result_ = ::dcool::core::Empty<>;
		};

		template <
			::dcool::core::Integral FirstT_, ::dcool::core::GeneralInteger... RestTs_
		> struct FirstExtendedIntegerConfig_<FirstT_, RestTs_...> {
			using Result_ = ::dcool::core::detail_::FirstExtendedIntegerConfig_<RestTs_...>::Result_;
		};

		template <
			::dcool::core::ExtendedInteger FirstT_, ::dcool::core::GeneralInteger... RestTs_
		> struct FirstExtendedIntegerConfig_<FirstT_, RestTs_...> {
			using Result_ = FirstT_::Config;
		};

		template <typename ConfigT_, ::dcool::core::GeneralInteger... IntegerTs_> requires (
			::dcool::core::isCompatibleToConfigGeneralInteger<ConfigT_, IntegerTs_...>
		) using ExtendedCommonIntegerOfConfigType_ = ::dcool::core::Integer<
			::dcool::core::detail_::commonSigness_<IntegerTs_...>, ::dcool::core::detail_::commonDigitCount_<IntegerTs_...>, ConfigT_
		>;
	}

	template <::dcool::core::GeneralInteger... IntegerTs_> using ExtendedCommonIntegerType =
		::dcool::core::detail_::ExtendedCommonIntegerOfConfigType_<
			typename ::dcool::core::detail_::FirstExtendedIntegerConfig_<IntegerTs_...>::Result_, IntegerTs_...
		>
	;

	template <
		::dcool::core::Boolean hasSignessC_, typename ConfigT_
	> struct Integer<hasSignessC_, 0, ConfigT_> {
		private: using Self_ = Integer<hasSignessC_, 0, ConfigT_>;
		public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
		public: static constexpr ::dcool::core::Length width = 0;
		public: using Config = ConfigT_;
		static_assert(!hasSigness, "Zero-bit integer cannot be signed.");

		public: using Underlying = void;
		public: using UnsignedSub = Self_;
		public: using SignedSuper = ::dcool::core::SignedInteger<1, Config>;
		public: static constexpr ::dcool::core::Boolean usingBuiltInUnderlying = false;
		public: static constexpr Self_ max = Self_();
		public: static constexpr Self_ min = Self_();
		public: template <
			typename ToRepresentT__
		> static constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::canRepresent<Self_, ToRepresentT__>;

		// A bug prevent us from using 'Self_' or 'Integer<widthC_, ConfigT_>' for the copy constructor.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr Integer() noexcept = default;
		public: constexpr Integer(Integer const& other_) noexcept = default;

		public: template <::dcool::core::Integral ValueT__> constexpr explicit Integer(
			ValueT__ const& initialValue_
		) noexcept: Self_() {
		}

		public: template <
			::dcool::core::ExtendedInteger ValueT__
		> constexpr explicit((!canRepresent<ValueT__>) || !(::dcool::core::ExtendedIntegerOfConfig<ValueT__, Config>)) Integer(
			ValueT__ const& initialValue_
		) noexcept: Self_() {
		}

		public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& = default;

		public: template <typename ValueT__> requires (canRepresent<ValueT__>) constexpr auto operator =(
			ValueT__ newValue_
		) noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			return *this;
		}

		public: constexpr auto operator +=(Self_ const& right_) noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator -=(Self_ const& right_) noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator *=(Self_ const& right_) noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator /=(Self_ const& right_) noexcept -> Self_& {
			return *this;
		}
	};

	template <
		::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_, typename ConfigT_
	> requires (
		widthC_ > 0 && widthC_ <= ::dcool::core::builtInIntegerUnderlyingLimit
	) struct Integer<hasSignessC_, widthC_, ConfigT_> {
		private: using Self_ = Integer<hasSignessC_, widthC_, ConfigT_>;
		public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
		public: static constexpr ::dcool::core::Length width = widthC_;
		public: using Config = ConfigT_;

		public: using Underlying = ::dcool::core::detail_::SingleUnitIntegerUnderlyingType_<hasSigness, width>;
		public: using UnsignedSub = ::dcool::core::UnsignedInteger<width - (hasSigness ? 0 : 1), Config>;
		public: using SignedSuper = ::dcool::core::SignedInteger<width + (hasSigness ? 0 : 1), Config>;
		public: static constexpr ::dcool::core::Boolean usingBuiltInUnderlying = true;
		public: static constexpr Self_ max = Self_(::dcool::core::bitFieldMax<Underlying, width>);
		public: static constexpr Self_ min = Self_(::dcool::core::bitFieldMin<Underlying, width>);
		public: template <
			typename ToRepresentT__
		> static constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::canRepresent<Self_, ToRepresentT__>;

		private: Underlying m_underlying_: width { 0 };

		// A bug prevent us from using 'Self_' or 'Integer<widthC_, ConfigT_>' for the copy constructor.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr Integer() noexcept = default;
		public: constexpr Integer(Integer const& other_) noexcept = default;

		public: template <::dcool::core::Integral ValueT__> constexpr explicit(!canRepresent<ValueT__>) Integer(
			ValueT__ const& initialValue_
		) noexcept: m_underlying_(initialValue_) {
		}

		public: template <
			::dcool::core::ExtendedInteger ValueT__
		> constexpr explicit((!canRepresent<ValueT__>) || !(::dcool::core::ExtendedIntegerOfConfig<ValueT__, Config>)) Integer(
			ValueT__ const& initialValue_
		) noexcept: Self_(from_(initialValue_)) {
		}

		// A bug prevent us from using 'Self_' or 'UnsignedInteger<widthC_, ConfigT_>' for the copy assignment operator.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr auto operator =(Integer const& other_) noexcept -> Self_& = default;

		public: template <typename ValueT__> requires (canRepresent<ValueT__>) constexpr auto operator =(
			ValueT__ newValue_
		) noexcept -> Self_& {
			if constexpr (::dcool::core::ExtendedInteger<ValueT__>) {
				this->m_underlying_ = newValue_->underlying();
			} else {
				this->m_underlying_ = newValue_;
			}
			return *this;
		}

		private: template <::dcool::core::ExtendedInteger OtherT__> static constexpr auto from_(
			OtherT__ const& other_
		) noexcept -> Self_ {
			Underlying underlying_;
			if constexpr (OtherT__::width == 0) {
				underlying_ = 0;
			} else if constexpr (OtherT__::usingBuiltInUnderlying) {
				underlying_ = static_cast<Underlying>(other_.underlying());
			} else {
				// Not yet implemented.
				::dcool::core::terminate();
			}
			return Self_(underlying_);
		}

		public: constexpr auto underlying() const noexcept -> Underlying {
			return this->m_underlying_;
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			if constexpr (hasSigness) {
				DCOOL_CORE_ASSERT(this->underlying() < max);
			}
			++(this->m_underlying_);
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_(*this);
			++*this;
			return result_;
		}

		public: constexpr auto operator +=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ += right_.underlying();
			return *this;
		}

		public: constexpr auto operator -=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ -= right_.underlying();
			return *this;
		}

		public: constexpr auto operator *=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ *= right_.underlying();
			return *this;
		}

		public: constexpr auto operator /=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ /= right_.underlying();
			return *this;
		}
	};

	template <
		::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralInteger RightT_
	> constexpr auto operator <=>(LeftT_ const& left_, RightT_ const& right_) noexcept -> ::dcool::core::StrongOrdering {
		using Common_ = ::dcool::core::ExtendedCommonIntegerType<LeftT_, RightT_>;
		Common_ const& commonLeft_ = left_;
		Common_ const& commonRight_ = right_;
		if constexpr (Common_::width == 0) {
			return ::dcool::core::StrongOrdering::equal;
		}
		return commonLeft_.underlying() <=> commonRight_.underlying();
	}

	template <
		::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralInteger RightT_
	> constexpr auto operator ==(LeftT_ const& left_, RightT_ const& right_) noexcept -> ::dcool::core::Boolean {
		return (left_ <=> right_) == 0;
	}
}

#	define DCOOL_CORE_DEFINE_UNSIGNED_INTEGER_LITERAL(width_) \
		static_assert( \
			width_ <= ::dcool::core::widthOfInteger<unsigned long long>, \
			"Unable to provide `dcool::core::UnsignedInteger<" DCOOL_TO_STR_(width_) ">` literal." \
		); \
		namespace dcool::core::extendedIntegerLiteral { \
			consteval auto operator "" DCOOL_GLUE_(_UI, width_)( \
				unsigned long long value_ \
			) noexcept -> ::dcool::core::UnsignedInteger<width_> { \
				if (::std::bit_width(value_) > width_) { \
					::dcool::core::terminate(); \
				} \
				return ::dcool::core::UnsignedInteger<width_>(value_); \
			} \
		}

#	define DCOOL_CORE_DEFINE_SIGNED_INTEGER_LITERAL(width_) \
		static_assert( \
			width_ <= ::dcool::core::widthOfInteger<unsigned long long>, \
			"Unable to provide `dcool::core::SignedInteger<" DCOOL_TO_STR_(width_) ">` literal." \
		); \
		namespace dcool::core::extendedIntegerLiteral { \
			consteval auto operator "" DCOOL_GLUE_(_SI, width_)( \
				unsigned long long value_ \
			) noexcept -> ::dcool::core::SignedInteger<width_> { \
				if (::std::bit_width(value_) > width_) { \
					::dcool::core::terminate(); \
				} \
				return ::dcool::core::SignedInteger<width_>(value_); \
			} \
		}

#	define DCOOL_CORE_DEFINE_INTEGER_LITERAL(width_) \
		DCOOL_CORE_DEFINE_UNSIGNED_INTEGER_LITERAL(width_) \
		DCOOL_CORE_DEFINE_SIGNED_INTEGER_LITERAL(width_)

DCOOL_CORE_DEFINE_INTEGER_LITERAL(1)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(2)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(3)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(4)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(5)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(6)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(7)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(8)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(9)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(10)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(11)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(12)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(13)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(14)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(15)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(16)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(17)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(18)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(19)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(20)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(21)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(22)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(23)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(24)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(25)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(26)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(27)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(28)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(29)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(30)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(31)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(32)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(33)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(34)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(35)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(36)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(37)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(38)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(39)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(40)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(41)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(42)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(43)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(44)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(45)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(46)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(47)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(48)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(49)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(50)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(51)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(52)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(53)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(54)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(55)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(56)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(57)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(58)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(59)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(60)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(61)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(62)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(63)
DCOOL_CORE_DEFINE_INTEGER_LITERAL(64)

#endif
