#ifndef DCOOL_CORE_INTEGER_HPP_INCLUDED_
#	define DCOOL_CORE_INTEGER_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/storage.hpp>

#	include <limits>

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
	> requires Integer<decltype(maxC_)> && Integer<decltype(minC_)> using IntegerType = ::dcool::core::detail_::IntegerType_<
		maxC_, minC_
	>::Result_;

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
}

#endif
