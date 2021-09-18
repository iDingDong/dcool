#ifndef DCOOL_CORE_INTEGER_HPP_INCLUDED_
#	define DCOOL_CORE_INTEGER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/compare.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/min_max.hpp>
#	include <dcool/core/range_operation.hpp>
#	include <dcool/core/storage.hpp>
#	include <dcool/core/value_list.hpp>

#	include <dcool/basic.hpp>

#	include <array>
#	include <bit>
#	include <limits>
#	include <type_traits>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core::detail_,
	HasValueNegatableForInteger_,
	extractedNegatableForIntegerValue_,
	negatable
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core::detail_,
	HasValueFoldedWidthForInteger_,
	extractedFoldedWidthForIntegerValue_,
	foldedWidth
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::core::detail_,
	HasValueOutOfRangeStrategryForInteger_,
	extractedOutOfRangeStrategryForIntegerValue_,
	outOfRangeStrategry
)

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

	template <::dcool::core::UnsignedIntegral T_> constexpr auto addAssignToCarry(
		T_& left_, T_ const& right_, ::dcool::core::Boolean alreadyCarrying_
	) noexcept -> ::dcool::core::Boolean {
		if (alreadyCarrying_) {
			++left_;
			if (left_ == 0) {
				left_ = right_;
				return true;
			}
		}
		left_ += right_;
		return left_ < right_;
	}

	template <::dcool::core::UnsignedIntegral T_> constexpr auto subtractAssignToCarry(
		T_& left_, T_ const& right_, ::dcool::core::Boolean alreadyCarrying_
	) noexcept -> ::dcool::core::Boolean {
		if (alreadyCarrying_) {
			--left_;
			if (left_ == ::std::numeric_limits<T_>::max()) {
				left_ = ~right_;
				return true;
			}
		}
		auto previousLeft_ = left_;
		left_ -= right_;
		return previousLeft_ < left_;
	}

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
				if (width_ > 1) {
					IntegerT_ maxCeil_ = (IntegerT_(1) << (width_ - 2));
					result_ = maxCeil_ + (maxCeil_ - 1);
				} else {
					result_ = 0;
				}
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

	template <::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_> constexpr auto bitFieldAdditionResultWithinRange(
		::dcool::core::UndeducedType<IntegerT_> const& left_, ::dcool::core::UndeducedType<IntegerT_> const& right_
	) noexcept -> ::dcool::core::Boolean {
		constexpr IntegerT_ max_ = ::dcool::core::detail_::bitFieldMax_<IntegerT_>(widthC_);
		constexpr IntegerT_ min_ = ::dcool::core::detail_::bitFieldMin_<IntegerT_>(widthC_);
		if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
			if (right_ < 0) {
				return left_ >= min_ - right_;
			}
		}
		return left_ <= max_ - right_;
	}

	template <
		::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_
	> constexpr auto bitFieldSubtractionResultWithinRange(
		::dcool::core::UndeducedType<IntegerT_> const& left_, ::dcool::core::UndeducedType<IntegerT_> const& right_
	) noexcept -> ::dcool::core::Boolean {
		constexpr IntegerT_ max_ = ::dcool::core::detail_::bitFieldMax_<IntegerT_>(widthC_);
		constexpr IntegerT_ min_ = ::dcool::core::detail_::bitFieldMin_<IntegerT_>(widthC_);
		if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
			if (right_ < 0) {
				return left_ <= right_ + max_;
			}
		}
		return left_ >= right_ + min_;
	}

	template <
		::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_
	> constexpr auto bitFieldMultiplicationResultWithinRange(
		::dcool::core::UndeducedType<IntegerT_> const& left_, ::dcool::core::UndeducedType<IntegerT_> const& right_
	) noexcept -> ::dcool::core::Boolean {
		constexpr IntegerT_ max_ = ::dcool::core::detail_::bitFieldMax_<IntegerT_>(widthC_);
		constexpr IntegerT_ min_ = ::dcool::core::detail_::bitFieldMin_<IntegerT_>(widthC_);
		if (right_ == 0) {
			return true;
		}
		if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
			if (left_ == -1 && right_ == min_) {
				return false;
			}
			if (left_ == min_ && right_ == -1) {
				return false;
			}
			if (left_ < min_ / right_) {
				return false;
			}
		}
		return left_ <= max_ / right_;
	}

	template <
		::dcool::core::Integral IntegerT_, ::dcool::core::Length widthC_
	> constexpr auto bitFieldDivisionResultWithinRange(
		::dcool::core::UndeducedType<IntegerT_> const& left_, ::dcool::core::UndeducedType<IntegerT_> const& right_
	) noexcept -> ::dcool::core::Boolean {
		constexpr IntegerT_ max_ = ::dcool::core::detail_::bitFieldMax_<IntegerT_>(widthC_);
		constexpr IntegerT_ min_ = ::dcool::core::detail_::bitFieldMin_<IntegerT_>(widthC_);
		if constexpr (::dcool::core::SignedIntegral<IntegerT_>) {
			if (left_ == min_ && right_ == -1) {
				return false;
			}
		}
		return true;
	}

	namespace detail_ {
		template <
			typename ConfigT_, ::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_
		> class IntegerConfigAdaptor_ {
			public: using Config = ConfigT_;
			public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
			public: static constexpr ::dcool::core::Boolean width = widthC_;

			public: static constexpr ::dcool::core::Boolean negatable =
				::dcool::core::detail_::extractedNegatableForIntegerValue_<Config>(true)
			;
			public: static constexpr ::dcool::core::Length foldedWidth =
				::dcool::core::detail_::extractedFoldedWidthForIntegerValue_<Config>(0)
			;
			public: static constexpr ::dcool::core::SusbicionStrategy outOfRangeStrategy =
				::dcool::core::detail_::extractedOutOfRangeStrategryForIntegerValue_<Config>(
					hasSigness ? ::dcool::core::SusbicionStrategy::undefined : ::dcool::core::SusbicionStrategy::ignore
				)
			;
			static_assert((!hasSigness) || outOfRangeStrategy != ::dcool::core::SusbicionStrategy::ignore);
		};
	}

	template <typename T_, ::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_> concept IntegerConfig = requires {
		typename ::dcool::core::detail_::IntegerConfigAdaptor_<T_, hasSignessC_, widthC_>;
	};

	template <
		typename T_, ::dcool::core::Length widthC_
	> concept UnsignedIntegerConfig = ::dcool::core::IntegerConfig<T_, false, widthC_>;


	template <
		typename T_, ::dcool::core::Length widthC_
	> concept SignedIntegerConfig = ::dcool::core::IntegerConfig<T_, true, widthC_>;

	template <
		typename ConfigT_, ::dcool::core::Boolean hasSignessC_, ::dcool::core::Length widthC_
	> requires (::dcool::core::IntegerConfig<ConfigT_, hasSignessC_, widthC_>) using IntegerConfigAtaptor =
		::dcool::core::detail_::IntegerConfigAdaptor_<ConfigT_, hasSignessC_, widthC_>
	;

	template <
		::dcool::core::Boolean hasSignessC_,
		::dcool::core::Length widthC_,
		::dcool::core::IntegerConfig<hasSignessC_, widthC_> ConfigT_ = ::dcool::core::Empty<>
	> struct Integer;

	template <
		::dcool::core::Length widthC_, ::dcool::core::UnsignedIntegerConfig<widthC_> ConfigT_ = ::dcool::core::Empty<>
	> using UnsignedInteger = ::dcool::core::Integer<false, widthC_>;

	template <
		::dcool::core::Length widthC_, ::dcool::core::SignedIntegerConfig<widthC_> ConfigT_ = ::dcool::core::Empty<>
	> using SignedInteger = ::dcool::core::Integer<true, widthC_>;

	template <auto maxC_, auto minC_ = 0> requires (maxC_ >= minC_) using ExtendedIntegerTypeToRepresent = ::dcool::core::Integer<
		(minC_ < 0), ::dcool::core::max(::std::bit_width(maxC_), ::std::bit_width(minC_ < 0 ? minC_ + 1 : maxC_))
	>;

	constexpr ::dcool::core::Length builtInIntegerUnderlyingLimit = ::dcool::core::widthOfInteger<
		::dcool::core::UnsignedMaxInteger
	>;

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

	template <::dcool::core::Length widthC_> constexpr ::dcool::core::Length recommendedExtendedIntegerFoldedWidth =
		::dcool::core::generalWidthOfInteger<::dcool::core::detail_::SingleUnitIntegerUnderlyingType_<false, widthC_>> -
		widthC_
	;

	template <::dcool::core::GeneralInteger IntegerT_> requires (
		::dcool::core::generalWidthOfInteger<IntegerT_> > 0
	) using ExtendedShiftStepType = ::dcool::core::ExtendedIntegerTypeToRepresent<
		::dcool::core::generalWidthOfInteger<IntegerT_> - 1
	>;

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

	template <::dcool::core::Integral IntegerT_> constexpr auto toExtendedInteger(
		IntegerT_ const& from_
	) noexcept -> ::dcool::core::ExtendedIntegerType<IntegerT_> {
		return ::dcool::core::ExtendedIntegerType<IntegerT_>(from_);
	}

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
		::dcool::core::Integral IntegerT_, typename IteratorT_
	> constexpr auto writeToBinaryStr(
		IntegerT_ const& value_, IteratorT_ destination_, ::dcool::core::Length width_ = ::dcool::core::widthOfInteger<IntegerT_>
	) noexcept -> IteratorT_ {
		::dcool::core::Length current_ = width_;
		while (current_ > 0) {
			--current_;
			*destination_ = (((value_ & (static_cast<IntegerT_>(1) << static_cast<IntegerT_>(current_))) == 0) ? '0' : '1');
			++destination_;
		}
		return destination_;
	}

	template <::dcool::core::Length widthC_, ::dcool::core::Integral IntegerT_> constexpr auto toBinaryStr(
		IntegerT_ const& value_
	) noexcept -> ::std::array<char, widthC_> {
		::std::array<char, widthC_> result_;
		::dcool::core::writeToBinaryStr(value_, result_.begin(), widthC_);
		return result_;
	}

	template <::dcool::core::Integral IntegerT_> constexpr auto toBinaryStr(
		IntegerT_ const& value_
	) noexcept -> ::std::array<char, ::dcool::core::widthOfInteger<IntegerT_>> {
		return ::dcool::core::toBinaryStr<::dcool::core::widthOfInteger<IntegerT_>>(value_);
	}

	namespace detail_ {
		template <::dcool::core::SusbicionStrategy strategy_> constexpr auto handleExtendedIntegerOutOfRange_() {
			if constexpr (strategy_ == ::dcool::core::SusbicionStrategy::undefined) {
				DCOOL_CORE_ASSERT(false);
			} else if constexpr (strategy_ == ::dcool::core::SusbicionStrategy::terminate) {
				::dcool::core::terminate();
			} else if constexpr (strategy_ == ::dcool::core::SusbicionStrategy::exception) {
				throw ::dcool::core::RangeError("`dcool::core::Integer` value out of range.");
			}
		}

		constexpr auto significantWidth_(::dcool::core::Length width_) noexcept -> ::dcool::core::Length {
			if (width_ == 0) {
				return 0;
			}
			::dcool::core::Length result_ = width_ % ::dcool::core::generalWidthOfInteger<::dcool::core::UnsignedMaxInteger>;
			if (result_ == 0) {
				return ::dcool::core::generalWidthOfInteger<::dcool::core::UnsignedMaxInteger>;
			}
			return result_;
		}
	}

	template <
		::dcool::core::Boolean hasSignessC_,
		::dcool::core::Length widthC_,
		::dcool::core::IntegerConfig<hasSignessC_, widthC_> ConfigT_
	> struct Integer: private ::dcool::core::Integer<hasSignessC_, ::dcool::core::detail_::significantWidth_(widthC_), ConfigT_> {
		private: using Self_ = Integer<hasSignessC_, widthC_, ConfigT_>;
		private: using Super_ = ::dcool::core::Integer<hasSignessC_, ::dcool::core::detail_::significantWidth_(widthC_), ConfigT_>;
		public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
		public: static constexpr ::dcool::core::Length width = widthC_;
		public: using Config = ConfigT_;
		template <
			::dcool::core::Boolean hasSignessC__,
			::dcool::core::Length widthC__,
			::dcool::core::IntegerConfig<hasSignessC__, widthC__> ConfigT__
		> friend struct ::dcool::core::Integer;

		public: using Super_::folded;

		private: using ConfigAdaptor_ = ::dcool::core::IntegerConfigAtaptor<Config, hasSigness, width>;
		public: using UnsignedSub = ::dcool::core::UnsignedInteger<width - (hasSigness ? 0 : 1), Config>;
		public: using SignedSuper = ::dcool::core::SignedInteger<width + (hasSigness ? 0 : 1), Config>;
		public: using ShiftStepType = ::dcool::core::ExtendedShiftStepType<Self_>;
		public: using Str = ::std::array<char, width>;
		private: using Unit_ = ::dcool::core::UnsignedMaxInteger;
		private: using UnitShiftStepType_ = Unit_;
		private: using SignificantPart_ = Super_;
		public: static constexpr ::dcool::core::Boolean negatable = ConfigAdaptor_::negatable;
		public: template <
			typename ToRepresentT__
		> static constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::canRepresent<Self_, ToRepresentT__>;
		private: static constexpr ::dcool::core::Boolean usingBuiltInUnderlying = false;
		private: static constexpr ::dcool::core::Length significantWidth_ = Super_::width;
		private: static constexpr ::dcool::core::Length unitWidth_ = ::dcool::core::generalWidthOfInteger<Unit_>;
		private: static constexpr ::dcool::core::Length unitCount_ = (width - significantWidth_) / unitWidth_;

		public: using Units = ::std::array<Unit_, unitCount_>;

		private: Units m_units_ = {};

		public: constexpr Integer() noexcept = default;
		public: constexpr Integer(Integer const& other_) noexcept = default;

		public: template <::dcool::core::Integral ValueT__> constexpr explicit(!canRepresent<ValueT__>) Integer(
			ValueT__ const& initialValue_
		) noexcept: m_units_ { Unit_(initialValue_) } {
			// Check for absurd implementation like libstdc++'s zero digit `__int128` before 10.3 with `-std=gnu++*`,
			// or libc++'s integral `__int128`.
			static_assert(sizeof(ValueT__) <= sizeof(Unit_));
		}

		public: template <
			::dcool::core::ExtendedInteger ValueT__
		> constexpr explicit((!canRepresent<ValueT__>) || !(::dcool::core::ExtendedIntegerOfConfig<ValueT__, Config>)) Integer(
			ValueT__ const& initialValue_
		) noexcept: Self_(from_(initialValue_)) {
		}

		private: template <
			::dcool::core::ExtendedInteger ValueT__
		> constexpr auto from_(ValueT__ const& value_) noexcept -> Self_ {
			if constexpr (ValueT__::width == 0) {
				return Self_(0);
			}
			if constexpr (ValueT__::usingBuiltInUnderlying) {
				return Self_(value_.underlying_());
			}
			Self_ result_;
			::dcool::core::Length commonUnitCount_;
			if constexpr (unitCount_ < ValueT__::unitCount_) {
				commonUnitCount_ = unitCount_;
				result_.mutableSignificantPart_() = SignificantPart_(value_.units_()[unitCount_]);
			} else {
				commonUnitCount_ = ValueT__::unitCount_;
				if constexpr (unitCount_ > ValueT__::unitCount_) {
					if (value_.significantPart_() < 0) {
						result_.mutableSignificantPart_() = ~SignificantPart_(0);
						::dcool::core::batchFill(~Unit_(0), result_.m_units_.begin() + commonUnitCount_, result_.m_units_.end());
					}
				} else {
					result_.mutableSignificantPart_() = SignificantPart_(value_.significantPart_());
				}
			}
			::dcool::core::batchCopyN(value_.units_().begin(), commonUnitCount_, result_.m_units_.begin());
			return result_;
		}

		private: constexpr auto significantPart_() const noexcept -> SignificantPart_ const& {
			return *this;
		}

		private: constexpr auto mutableSignificantPart_() noexcept -> SignificantPart_& {
			return *this;
		}

		private: constexpr auto units_() const noexcept -> Units const& {
			return this->m_units_;
		}

		public: static constexpr auto compare(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::StrongOrdering {
			auto significantResult_ = (left_.significantPart_() <=> right_.significantPart_());
			if (significantResult_ != 0) {
				return significantResult_;
			}
			auto current_ = unitCount_;
			while (current_ > 0) {
				--current_;
				auto currentResult_ = (left_.units_()[current_] <=> right_.units_()[current_]);
				if (currentResult_ != 0) {
					return significantResult_;
				}
			}
			return ::dcool::core::StrongOrdering::equal;
		}

		public: constexpr auto operator +() const noexcept -> Self_ {
			return *this;
		}

		public: constexpr auto operator -() const noexcept -> Self_ requires (negatable) {
			Self_ result_ = ~*this;
			++result_;
			return result_;
		}

		public: constexpr auto operator ~() const noexcept -> Self_ {
			Self_ result_;
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				result_.m_units_[i_] = ~(this->units_()[i_]);
			}
			result_.mutableSignificantPart_() = ~(this->significantPart_());
			return result_;
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			for (Unit_& unit_: this->m_units_) {
				++unit_;
				if (unit_ != 0) [[likely]] {
					return *this;
				}
			}
			++(this->mutableSignificantPart_());
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_(*this);
			++*this;
			return result_;
		}

		public: constexpr auto operator --() noexcept -> Self_& {
			for (Unit_& unit_: this->m_units_) {
				--unit_;
				if (unit_ != ::std::numeric_limits<Unit_>::max()) [[likely]] {
					return *this;
				}
			}
			--(this->mutableSignificantPart_());
			return *this;
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_(*this);
			--*this;
			return result_;
		}

		public: constexpr auto operator +=(Self_ const& right_) noexcept -> Self_& {
			::dcool::core::Boolean carrying_ = false;
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				carrying_ = ::dcool::core::addAssignToCarry(this->m_units_[i_], right_.units_()[i_], carrying_);
			}
			if (carrying_) {
				if constexpr (hasSigness) {
					if (right_.significantPart_() != right_.significantPart_().max) [[likely]] {
						this->mutableSignificantPart_() += (right_.significantPart_() + 1);
						return *this;
					}
					++(this->mutableSignificantPart_());
				}
			}
			this->mutableSignificantPart_() += right_.significantPart_();
			return *this;
		}

		public: constexpr auto operator -=(Self_ const& right_) noexcept -> Self_& {
			::dcool::core::Boolean carrying_ = false;
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				carrying_ = ::dcool::core::subtractAssignToCarry(this->m_units_[i_], right_.units_()[i_], carrying_);
			}
			if (carrying_) {
				if constexpr (hasSigness) {
					if (right_.significantPart_() != right_.significantPart_().max) [[likely]] {
						this->mutableSignificantPart_() -= (right_.significantPart_() + 1);
						return *this;
					}
					--(this->mutableSignificantPart_());
				}
			}
			this->mutableSignificantPart_() -= right_.significantPart_();
			return *this;
		}

		public: constexpr auto operator &=(Self_ const& right_) noexcept -> Self_& {
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				this->m_units_[i_] &= right_.units_()[i_];
			}
			this->mutableSignificantPart_() &= right_.significantPart_();
			return *this;
		}

		public: constexpr auto operator |=(Self_ const& right_) noexcept -> Self_& {
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				this->m_units_[i_] |= right_.units_()[i_];
			}
			this->mutableSignificantPart_() |= right_.significantPart_();
			return *this;
		}

		public: constexpr auto operator ^=(Self_ const& right_) noexcept -> Self_& {
			for (::dcool::core::Length i_ = 0; i_ < unitCount_; ++i_) {
				this->m_units_[i_] ^= right_.units_()[i_];
			}
			this->mutableSignificantPart_() ^= right_.significantPart_();
			return *this;
		}

		public: constexpr auto operator <<=(ShiftStepType const& right_) noexcept -> Self_& {
			ShiftStepType unitStep_ = right_ / unitWidth_;
			auto effectiveBegin_ = this->m_units_.begin() + static_cast<::dcool::core::Difference>(unitStep_);
			if (unitStep_ > 0) {
				this->mutableSignificantPart_() = SignificantPart_(0);
				this->mutableSignificantPart_() |= SignificantPart_(
					*(this->m_units_.end() - static_cast<::dcool::core::Difference>(unitStep_))
				);
				::dcool::core::batchCopyForward(
					this->m_units_.begin(), this->m_units_.end() - static_cast<::dcool::core::Difference>(unitStep_), effectiveBegin_
				);
				::dcool::core::batchFill(Unit_(0), this->m_units_.begin(), effectiveBegin_);
			}
			auto subStep_ = UnitShiftStepType_(right_ % unitWidth_);
			if (subStep_ == 0) {
				return *this;
			}
			auto carriedToRetreat_ = unitWidth_ - subStep_;
			Unit_ maskToReserve_ = ~(Unit_(0)) >> subStep_;
			Unit_ maskToCarry_ = ~maskToReserve_;
			Unit_ carried_ = Unit_(0);
			for (auto current_ = effectiveBegin_; current_ != this->m_units_.end(); ++current_) {
				Unit_ toCarry_ = (*current_ & maskToCarry_);
				*current_ <<= subStep_;
				*current_ |= carried_;
				carried_ = (toCarry_ >> carriedToRetreat_);
			}
			if (subStep_ >= significantWidth_) {
				this->mutableSignificantPart_() = SignificantPart_(0);
			} else {
				this->mutableSignificantPart_() <<= typename SignificantPart_::ShiftStepType(subStep_);
			}
			this->mutableSignificantPart_() |= SignificantPart_(carried_);
			return *this;
		}

		public: constexpr auto operator >>=(ShiftStepType const& right_) noexcept -> Self_& {
			::dcool::core::Boolean negative_ = this->significantPart_() < 0;
			ShiftStepType unitStep_ = right_ / unitWidth_;
			auto subStep_ = UnitShiftStepType_(right_ % unitWidth_);
			auto carriedToRetreat_ = unitWidth_ - subStep_;
			Unit_ maskToReserve_ = ~(Unit_(0)) << subStep_;
			Unit_ maskToCarry_ = ~maskToReserve_;
			Unit_ carried_ = (negative_ ? ((~Unit_(0) & maskToCarry_) << carriedToRetreat_) : Unit_(0));
			auto effectiveReverseBegin_ = this->m_units_.rbegin() + static_cast<::dcool::core::Difference>(unitStep_);
			if (unitStep_ > 0) {
				::dcool::core::batchCopyForward(
					this->m_units_.rbegin(),
					this->m_units_.rend() - static_cast<::dcool::core::Difference>(unitStep_),
					effectiveReverseBegin_
				);
				--effectiveReverseBegin_;
				*effectiveReverseBegin_ = static_cast<Unit_>(this->significantPart_().underlying_());
				if (negative_) {
					this->mutableSignificantPart_() = ~SignificantPart_(0);
					::dcool::core::batchFill(~Unit_(0), this->m_units_.rbegin(), effectiveReverseBegin_);
				} else {
					this->mutableSignificantPart_() = SignificantPart_(0);
					::dcool::core::batchFill(Unit_(0), this->m_units_.rbegin(), effectiveReverseBegin_);
				}
			} else if (right_ < significantWidth_) {
				carried_ = ((static_cast<Unit_>(this->significantPart_()) & maskToCarry_) << carriedToRetreat_);
				this->mutableSignificantPart_() >>= typename SignificantPart_::ShiftStepType(subStep_);
			}
			for (auto current_ = effectiveReverseBegin_; current_ != this->m_units_.rend(); ++current_) {
				Unit_ toCarry_ = (*current_ & maskToCarry_);
				*current_ >>= UnitShiftStepType_(subStep_);
				*current_ |= carried_;
				carried_ = (toCarry_ << carriedToRetreat_);
			}
			return *this;
		}

		public: template <
			::dcool::core::Integral ToT_
		> constexpr explicit(!::dcool::core::canRepresent<ToT_, Self_>) operator ToT_(
		) const noexcept {
			// Check for absurd implementation like libstdc++'s zero digit `__int128` before 10.3 with `-std=gnu++*`,
			// or libc++'s integral `__int128`.
			static_assert(sizeof(ToT_) <= sizeof(Unit_), "Integral type that does not fit into `*intmax_t` is not supported.");
			return ToT_(this->units_()[0]);
		}

		public: template <typename IteratorT_> constexpr auto writeToBinaryStr(
			IteratorT_ destination_
		) const noexcept -> IteratorT_ {
			destination_ = this->significantPart_().writeToBinaryStr(destination_);
			for (auto current_ = this->units_().crbegin(); current_ != this->units_().crend(); ++current_) {
				destination_ = ::dcool::core::writeToBinaryStr(*current_, destination_);
			}
			return destination_;
		}

		public: constexpr auto toBinaryStr() const noexcept -> Str {
			Str result_;
			this->writeToBinaryStr(result_.begin());
			return result_;
		}
	};

	template <
		::dcool::core::Boolean hasSignessC_, typename ConfigT_
	> struct Integer<hasSignessC_, 0, ConfigT_> {
		private: using Self_ = Integer<hasSignessC_, 0, ConfigT_>;
		public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
		public: static constexpr ::dcool::core::Length width = 0;
		public: using Config = ConfigT_;
		static_assert(!hasSigness, "Zero-bit integer cannot be signed.");

		private: using ConfigAdaptor_ = ::dcool::core::IntegerConfigAtaptor<Config, hasSigness, width>;
		public: using Underlying_ = void;
		public: using UnsignedSub = Self_;
		public: using SignedSuper = ::dcool::core::SignedInteger<1, Config>;
		public: static constexpr ::dcool::core::Boolean usingBuiltInUnderlying = false;
		public: static constexpr Self_ max = Self_();
		public: static constexpr Self_ min = Self_();
		public: static constexpr ::dcool::core::Boolean negatable = ConfigAdaptor_::negatable;
		public: template <
			typename ToRepresentT__
		> static constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::canRepresent<Self_, ToRepresentT__>;
		public: static constexpr ::dcool::core::SusbicionStrategy outOfRangeStrategy = ConfigAdaptor_::outOfRangeStrategy;
		private: static constexpr ::dcool::core::Boolean outOfRangeDisallowed_ = (
			outOfRangeStrategy != ::dcool::core::SusbicionStrategy::ignore
		);

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

		public: static constexpr auto compare(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::StrongOrdering {
			return ::dcool::core::StrongOrdering::equal;
		}

		public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& = default;

		public: template <typename ValueT__> requires (canRepresent<ValueT__>) constexpr auto operator =(
			ValueT__ newValue_
		) noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator +() const noexcept -> Self_ {
			return *this;
		}

		public: constexpr auto operator -() const noexcept -> Self_ requires (negatable) {
			return Self_();
		}

		public: constexpr auto operator ~() const noexcept -> Self_ {
			return Self_();
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
			}
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			if constexpr (outOfRangeDisallowed_) {
				::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
			}
			return *this;
		}

		public: constexpr auto operator --() noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
			}
			return *this;
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			if constexpr (outOfRangeDisallowed_) {
				::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
			}
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

		public: constexpr auto operator %=(Self_ const& right_) noexcept -> Self_& {
			return *this;
		}

		public: template <
			::dcool::core::Integral ToT_
		> constexpr explicit(!::dcool::core::canRepresent<ToT_, Self_>) operator ToT_(
		) const noexcept {
			return 0;
		}
	};

	namespace detail_ {
		template <
			::dcool::core::Integral UnderlyingT_, ::dcool::core::Length widthC_, ::dcool::core::Length foldedWidthC_
		> struct IntegerBase_ {
			public: using Underlying_ = UnderlyingT_;
			public: static constexpr ::dcool::core::Length foldedWidth = foldedWidthC_;

			public: Underlying_ m_underlying_: widthC_ { 0 };
			public: Underlying_ folded: foldedWidth { 0 };
		};

		template <::dcool::core::Integral UnderlyingT_, ::dcool::core::Length widthC_> struct IntegerBase_<
			UnderlyingT_, widthC_, 0
		> {
			public: using Underlying_ = UnderlyingT_;
			public: static constexpr ::dcool::core::Length foldedWidth = 0;

			public: Underlying_ m_underlying_: widthC_ { 0 };
			public: [[no_unique_address]] ::dcool::core::Empty<> folded;
		};
	}

	template <
		::dcool::core::Boolean hasSignessC_,
		::dcool::core::Length widthC_,
		::dcool::core::IntegerConfig<hasSignessC_, widthC_> ConfigT_
	> requires (
		widthC_ > 0 && widthC_ <= ::dcool::core::builtInIntegerUnderlyingLimit
	) struct Integer<hasSignessC_, widthC_, ConfigT_>: private ::dcool::core::detail_::IntegerBase_<
		::dcool::core::detail_::SingleUnitIntegerUnderlyingType_<hasSignessC_, widthC_>,
		widthC_,
		::dcool::core::IntegerConfigAtaptor<ConfigT_, hasSignessC_, widthC_>::foldedWidth
	> {
		private: using Self_ = Integer<hasSignessC_, widthC_, ConfigT_>;
		private: using Super_ = ::dcool::core::detail_::IntegerBase_<
			::dcool::core::detail_::SingleUnitIntegerUnderlyingType_<hasSignessC_, widthC_>,
			widthC_,
			::dcool::core::IntegerConfigAtaptor<ConfigT_, hasSignessC_, widthC_>::foldedWidth
		>;
		public: static constexpr ::dcool::core::Boolean hasSigness = hasSignessC_;
		public: static constexpr ::dcool::core::Length width = widthC_;
		public: using Config = ConfigT_;
		template <
			::dcool::core::Boolean hasSignessC__,
			::dcool::core::Length widthC__,
			::dcool::core::IntegerConfig<hasSignessC__, widthC__> ConfigT__
		> friend struct ::dcool::core::Integer;

		private: using typename Super_::Underlying_;
		public: using Super_::foldedWidth;
		private: using Super_::m_underlying_;
		public: using Super_::folded;

		private: using ConfigAdaptor_ = ::dcool::core::IntegerConfigAtaptor<Config, hasSigness, width>;
		public: using UnsignedSub = ::dcool::core::UnsignedInteger<width - (hasSigness ? 0 : 1), Config>;
		public: using SignedSuper = ::dcool::core::SignedInteger<width + (hasSigness ? 0 : 1), Config>;
		public: using ShiftStepType = ::dcool::core::ExtendedShiftStepType<Self_>;
		public: using Str = ::std::array<char, width>;
		public: static constexpr ::dcool::core::Boolean negatable = ConfigAdaptor_::negatable;
		public: static constexpr ::dcool::core::Boolean usingBuiltInUnderlying = true;
		private: static constexpr Underlying_ underlyingMax_ = ::dcool::core::bitFieldMax<Underlying_, width>;
		private: static constexpr Underlying_ underlyingMin_ = ::dcool::core::bitFieldMin<Underlying_, width>;
		public: static constexpr Self_ max = Self_(underlyingMax_);
		public: static constexpr Self_ min = Self_(underlyingMin_);
		public: template <
			typename ToRepresentT__
		> static constexpr ::dcool::core::Boolean canRepresent = ::dcool::core::canRepresent<Self_, ToRepresentT__>;
		public: static constexpr ::dcool::core::SusbicionStrategy outOfRangeStrategy = ConfigAdaptor_::outOfRangeStrategy;
		private: static constexpr ::dcool::core::Boolean outOfRangeDisallowed_ = (
			outOfRangeStrategy != ::dcool::core::SusbicionStrategy::ignore
		);

		// A bug prevent us from using 'Self_' or 'Integer<widthC_, ConfigT_>' for the copy constructor.
		// See document/dependency_bugs#Bug_3 for more details.
		public: constexpr Integer() noexcept = default;
		public: constexpr Integer(Integer const& other_) noexcept = default;

		public: template <::dcool::core::Integral ValueT__> constexpr explicit(!canRepresent<ValueT__>) Integer(
			ValueT__ const& initialValue_
		) noexcept: Super_ { .m_underlying_ = Underlying_(initialValue_) } {
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
				this->m_underlying_ = newValue_->underlying_();
			} else {
				this->m_underlying_ = newValue_;
			}
			return *this;
		}

		private: template <::dcool::core::ExtendedInteger OtherT__> static constexpr auto from_(
			OtherT__ const& other_
		) noexcept -> Self_ {
			Underlying_ underlying_;
			if constexpr (OtherT__::width == 0) {
				underlying_ = 0;
			} else if constexpr (OtherT__::usingBuiltInUnderlying) {
				underlying_ = static_cast<Underlying_>(other_.underlying_());
			} else {
				// Not yet implemented.
				::dcool::core::terminate();
			}
			return Self_(underlying_);
		}

		private: constexpr auto underlying_() const noexcept -> Underlying_ {
			return this->m_underlying_;
		}

		public: static constexpr auto compare(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::StrongOrdering {
			return left_.underlying_() <=> right_.underlying_();
		}

		public: constexpr auto operator +() const noexcept -> Self_ {
			return *this;
		}

		public: constexpr auto operator -() const noexcept -> Self_ requires (negatable) {
			if constexpr (outOfRangeDisallowed_) {
				if (this->underlying_() == underlyingMin_) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			return Self_(-(this->underlying_()));
		}

		public: constexpr auto operator ~() const noexcept -> Self_ {
			return Self_(~(this->underlying_()));
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (this->underlying_() < underlyingMax_) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			++(this->m_underlying_);
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_(*this);
			++*this;
			return result_;
		}

		public: constexpr auto operator --() noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (this->underlying_() > underlyingMin_) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			--(this->m_underlying_);
			return *this;
		}

		public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			Self_ result_(*this);
			--*this;
			return result_;
		}

		public: constexpr auto operator +=(Self_ const& right_) noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (!::dcool::core::bitFieldAdditionResultWithinRange<Underlying_, width>(this->underlying_(), right_.underlying_())) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			this->m_underlying_ += right_.underlying_();
			return *this;
		}

		public: constexpr auto operator -=(Self_ const& right_) noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (
					!::dcool::core::bitFieldSubtractionResultWithinRange<Underlying_, width>(this->underlying_(), right_.underlying_())
				) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			this->m_underlying_ -= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator *=(Self_ const& right_) noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (
					!::dcool::core::bitFieldMultiplicationResultWithinRange<Underlying_, width>(this->underlying_(), right_.underlying_())
				) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			this->m_underlying_ *= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator /=(Self_ const& right_) noexcept -> Self_& {
			if constexpr (outOfRangeDisallowed_) {
				if (!::dcool::core::bitFieldDivisionResultWithinRange<Underlying_, width>(this->underlying_(), right_.underlying_())) {
					::dcool::core::detail_::handleExtendedIntegerOutOfRange_<outOfRangeStrategy>();
				}
			}
			this->m_underlying_ /= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator %=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ %= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator &=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ &= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator |=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ |= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator ^=(Self_ const& right_) noexcept -> Self_& {
			this->m_underlying_ ^= right_.underlying_();
			return *this;
		}

		public: constexpr auto operator <<=(ShiftStepType const& right_) noexcept -> Self_& {
			if constexpr (width > 1) {
				this->m_underlying_ <<= right_.underlying_();
			}
			return *this;
		}

		public: constexpr auto operator >>=(ShiftStepType const& right_) noexcept -> Self_& {
			if constexpr (width > 1) {
				this->m_underlying_ >>= right_.underlying_();
			}
			return *this;
		}

		public: template <::dcool::core::Integral ToT_> constexpr explicit(!::dcool::core::canRepresent<ToT_, Self_>) operator ToT_(
		) const noexcept {
			return ToT_(this->underlying_());
		}

		public: template <typename IteratorT_> constexpr auto writeToBinaryStr(
			IteratorT_ destination_
		) const noexcept -> IteratorT_ {
			return ::dcool::core::writeToBinaryStr(this->underlying_(), destination_, width);
		}

		public: constexpr auto toBinaryStr() const noexcept -> Str {
			return ::dcool::core::toBinaryStr<width>(this->underlying_());
		}
	};

	namespace detail_ {
		template <::dcool::core::GeneralInteger... IntegerTs_> constexpr ::dcool::core::Length findFirstExtendedInteger_ = 0;

		template <
			::dcool::core::GeneralInteger FirstT_, ::dcool::core::GeneralInteger... IntegerTs_
		> constexpr ::dcool::core::Length findFirstExtendedInteger_<
			FirstT_, IntegerTs_...
		> = (::dcool::core::ExtendedInteger<FirstT_> ? 0 : ::dcool::core::detail_::findFirstExtendedInteger_<IntegerTs_...> + 1);
	}

	template <
		::dcool::core::GeneralInteger... IntegerTs_
	> constexpr ::dcool::core::Boolean containsExtendedInteger = (
		::dcool::core::detail_::findFirstExtendedInteger_<IntegerTs_...> < sizeof...(IntegerTs_)
	);

	template <
		::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralInteger RightT_
	> requires (::dcool::core::containsExtendedInteger<LeftT_, RightT_>) constexpr auto operator <=>(
		LeftT_ const& left_, RightT_ const& right_
	) noexcept -> ::dcool::core::StrongOrdering {
		using Common_ = ::dcool::core::ExtendedCommonIntegerType<LeftT_, RightT_>;
		Common_ const& commonLeft_ = left_;
		Common_ const& commonRight_ = right_;
		return Common_::compare(commonLeft_, commonRight_);
	}

	template <
		::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralInteger RightT_
	> requires (::dcool::core::containsExtendedInteger<LeftT_, RightT_>) constexpr auto operator ==(
		LeftT_ const& left_, RightT_ const& right_
	) noexcept -> ::dcool::core::Boolean {
		return (left_ <=> right_) == 0;
	}
}

#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_HELPER_(binaryOperator_, compoundOperator_) \
		namespace dcool::core { \
			template < \
				::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralInteger RightT_ \
			> requires (::dcool::core::containsExtendedInteger<LeftT_, RightT_>) constexpr auto operator binaryOperator_( \
				LeftT_ const& left_, RightT_ const& right_ \
			) noexcept -> ::dcool::core::ExtendedCommonIntegerType<LeftT_, RightT_> { \
				::dcool::core::ExtendedCommonIntegerType<LeftT_, RightT_> result_ = left_; \
				result_ compoundOperator_ right_; \
				return result_; \
			} \
		}
#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(binaryOperator_) \
		DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_HELPER_(binaryOperator_, DCOOL_GLUE_(binaryOperator_, =))

DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(+)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(-)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(*)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(/)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(%)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(&)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(|)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_ARITHMETIC_OPERATOR_(^)

#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_SHIFT_OPERATOR_HELPER_(binaryOperator_, compoundOperator_) \
		namespace dcool::core { \
			template < \
				::dcool::core::GeneralInteger LeftT_, ::dcool::core::GeneralUnsignedInteger RightT_ \
			> requires ( \
				::dcool::core::containsExtendedInteger<LeftT_, RightT_> && \
				::dcool::core::ConvertibleTo<RightT_, ::dcool::core::ExtendedShiftStepType<LeftT_>> \
			) constexpr auto operator binaryOperator_( \
				LeftT_ const& left_, RightT_ const& right_ \
			) noexcept -> LeftT_ { \
				::dcool::core::ExtendedCommonIntegerType<LeftT_, RightT_> result_ = left_; \
				result_ compoundOperator_ right_; \
				return result_; \
			} \
		}
#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_SHIFT_OPERATOR_(binaryOperator_) \
		DCOOL_CORE_DEFINE_EXTENDED_INTEGER_SHIFT_OPERATOR_HELPER_(binaryOperator_, DCOOL_GLUE_(binaryOperator_, =))

DCOOL_CORE_DEFINE_EXTENDED_INTEGER_SHIFT_OPERATOR_(<<)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_SHIFT_OPERATOR_(>>)

#	define DCOOL_CORE_DEFINE_UNSIGNED_EXTENDED_INTEGER_LITERAL(width_) \
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

#	define DCOOL_CORE_DEFINE_SIGNED_EXTENDED_INTEGER_LITERAL(width_) \
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

#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(width_) \
		DCOOL_CORE_DEFINE_UNSIGNED_EXTENDED_INTEGER_LITERAL(width_) \
		DCOOL_CORE_DEFINE_SIGNED_EXTENDED_INTEGER_LITERAL(width_)

DCOOL_CORE_DEFINE_UNSIGNED_EXTENDED_INTEGER_LITERAL(0)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(1)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(2)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(3)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(4)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(5)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(6)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(7)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(8)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(9)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(10)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(11)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(12)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(13)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(14)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(15)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(16)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(17)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(18)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(19)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(20)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(21)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(22)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(23)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(24)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(25)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(26)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(27)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(28)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(29)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(30)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(31)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(32)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(33)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(34)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(35)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(36)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(37)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(38)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(39)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(40)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(41)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(42)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(43)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(44)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(45)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(46)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(47)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(48)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(49)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(50)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(51)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(52)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(53)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(54)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(55)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(56)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(57)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(58)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(59)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(60)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(61)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(62)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(63)
DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(64)

#	define DCOOL_CORE_PROVIDED_EXTENDED_INTEGER_LITERAL_MAX_WIDTH 64

#	define DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL(width_) \
	static_assert( \
		width_ > DCOOL_CORE_PROVIDED_EXTENDED_INTEGER_LITERAL_MAX_WIDTH, \
		"Extended " DCOOL_TO_STR_(width_) "-bit integer literal is already provided by `dcool`." \
	); \
	DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL_(width_);

namespace dcool::core {
	namespace detail_ {
		template <unsigned baseC_ = 0> constexpr auto toDigit_(char digit_) noexcept -> unsigned {
			unsigned result_;
			switch (digit_) {
				case '0':
				result_ = 0;
				break;
				case '1':
				result_ = 1;
				break;
				case '2':
				result_ = 2;
				break;
				case '3':
				result_ = 3;
				break;
				case '4':
				result_ = 4;
				break;
				case '5':
				result_ = 5;
				break;
				case '6':
				result_ = 6;
				break;
				case '7':
				result_ = 7;
				break;
				case '8':
				result_ = 8;
				break;
				case '9':
				result_ = 9;
				break;
				case 'A':
				case 'a':
				result_ = 10;
				break;
				case 'B':
				case 'b':
				result_ = 11;
				break;
				case 'C':
				case 'c':
				result_ = 12;
				break;
				case 'D':
				case 'd':
				result_ = 13;
				break;
				case 'E':
				case 'e':
				result_ = 14;
				break;
				case 'F':
				case 'f':
				result_ = 15;
				break;
				default:
				::dcool::core::terminate();
			}
			if constexpr (baseC_ > 0) {
				if (result_ >= baseC_) {
					::dcool::core::terminate();
				}
			}
			return result_;
		}

		template <
			auto baseC_, ::dcool::core::Length digitCountC_
		> requires (baseC_ == ::std::bit_floor(baseC_)) constexpr auto toExtendedUnsignedInteger_(
			::std::array<decltype(baseC_), digitCountC_> const& from_
		) noexcept -> ::dcool::core::UnsignedInteger<::std::bit_width(baseC_ - 1) * digitCountC_> {
			using ResultType_ = ::dcool::core::UnsignedInteger<::std::bit_width(baseC_ - 1) * digitCountC_>;
			ResultType_ result_ = {};
			auto digitWidth_ = static_cast<typename ResultType_::ShiftStepType>(::std::bit_width(baseC_ - 1));
			for (::dcool::core::Length i_ = 0; i_ < digitCountC_; ++i_) {
				result_ <<= digitWidth_;
				result_ |= ResultType_(from_[i_]);
			}
			return result_;
		}

		template <::dcool::core::ValueList CharactersT_> struct ExtendedLiteralConverterHelper_;

		template <char... digitCs_> struct ExtendedLiteralConverterHelper_<::dcool::core::Values<'0', 'x', digitCs_...>> {
			static constexpr auto convert_() noexcept {
				return ::dcool::core::detail_::toExtendedUnsignedInteger_<16U>(
					::std::array<unsigned, sizeof...(digitCs_)>{ ::dcool::core::detail_::toDigit_<16U>(digitCs_)... }
				);
			}
		};

		template <char... digitCs_> struct ExtendedLiteralConverterHelper_<::dcool::core::Values<'0', digitCs_...>> {
			static constexpr auto convert_() noexcept {
				return ::dcool::core::detail_::toExtendedUnsignedInteger_<8U>(
					::std::array<unsigned, sizeof...(digitCs_)>{ ::dcool::core::detail_::toDigit_<8U>(digitCs_)... }
				);
			}
		};

		template <char... digitCs_> struct ExtendedLiteralConverterHelper_<::dcool::core::Values<'0', 'b', digitCs_...>> {
			static constexpr auto convert_() noexcept {
				return ::dcool::core::detail_::toExtendedUnsignedInteger_<2U>(
					::std::array<unsigned, sizeof...(digitCs_)>{ ::dcool::core::detail_::toDigit_<2U>(digitCs_)... }
				);
			}
		};

		template <char... characterCs_> using ExtendedLiteralConverter_ = ::dcool::core::detail_::ExtendedLiteralConverterHelper_<
			typename ::dcool::core::Values<characterCs_...>::RemoveAll<'\''>
		>;
	}

	namespace extendedIntegerLiteral {
		template <char... characterCs_> consteval auto operator ""_UI() noexcept {
			return ::dcool::core::detail_::ExtendedLiteralConverter_<characterCs_...>::convert_();
		}

		template <char... characterCs_> consteval auto operator ""_SI() noexcept {
			auto result_ = ::dcool::core::detail_::ExtendedLiteralConverter_<characterCs_...>::convert_();
			return ::dcool::core::SignedInteger<decltype(result_)::width>(result_);
		}
	}
}

#endif
