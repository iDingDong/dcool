#ifndef DCOOL_CORE_ALIGNMENT_HPP_INCLUDED_
#	define DCOOL_CORE_ALIGNMENT_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

#	include <limits>

namespace dcool::core {
	constexpr ::dcool::core::Alignment defaultNewAlignment = __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	constexpr ::dcool::core::Alignment minRepresentableAlignment = 1;
	constexpr ::dcool::core::Alignment maxRepresentableAlignment =
		minRepresentableAlignment << (::std::numeric_limits<::dcool::core::Alignment>::digits - 1)
	;
	template <::dcool::core::Size sizeC_> constexpr ::dcool::core::Alignment defaultAlignmentFor = alignof(
		::std::aligned_storage_t<sizeC_>
	);
}

#endif
