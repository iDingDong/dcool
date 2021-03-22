#ifndef DCOOL_BASIC_HPP_INCLUDED_
#	define DCOOL_BASIC_HPP_INCLUDED_ 1

#	include <dcool/version.hpp>

#	include <cstdint>

namespace dcool {
	constexpr std::uintmax_t majorVersion = DCOOL_MAJOR_VERSION;
	constexpr std::uintmax_t minorVersion = DCOOL_MINOR_VERSION;
	constexpr std::uintmax_t patchVersion = DCOOL_PATCH_VERSION;
	constexpr char8_t versionName[] = DCOOL_VERSION_NAME;
	constexpr char versionNativeName[] = DCOOL_VERSION_NATIVE_NAME;
}

#endif
