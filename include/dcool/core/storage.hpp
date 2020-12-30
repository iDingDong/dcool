#ifndef DCOOL_CORE_STORAGE_HPP_INCLUDED_
#	define DCOOL_CORE_STORAGE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <limits>
#	include <memory>

namespace dcool::core {
	constexpr ::dcool::core::Alignment defaultNewAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	constexpr ::dcool::core::Alignment minRepresentableAlignment = 1;
	constexpr ::dcool::core::Alignment maxRepresentableAlignment =
		minRepresentableAlignment << (::std::numeric_limits<::dcool::core::Alignment>::digits - 1)
	;
	template <::dcool::core::Size sizeC_> constexpr ::dcool::core::Alignment defaultAlignmentFor = alignof(
		::std::aligned_storage_t<sizeC_>
	);

	template <
		typename T_, ::dcool::core::Size sizeC_, ::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> constexpr ::dcool::core::Boolean isStorable = sizeof(T_) <= sizeC_ && alignof(T_) <= alignmentC_;

	template <
		typename T_, ::dcool::core::Size sizeC_, ::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> concept Storable = isStorable<T_, sizeC_, alignmentC_>;

	template <
		::dcool::core::Size sizeC_, ::dcool::core::Alignment alignmentC_ = ::dcool::core::defaultAlignmentFor<sizeC_>
	> using AlignedStorage = ::std::aligned_storage_t<sizeC_, alignmentC_>;

	template <::dcool::core::Object ValueT_> using StorageFor = ::dcool::core::AlignedStorage<sizeof(ValueT_), alignof(ValueT_)>;
}

#endif
