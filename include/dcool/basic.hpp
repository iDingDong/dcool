#ifndef DCOOL_BASIC_HPP_INCLUDED_
#	define DCOOL_BASIC_HPP_INCLUDED_ 1

#	include <dcool/version.h>
#	include <dcool/config.h>
#	include <dcool/config.hpp>

#	include <cstdint>

namespace dcool {
	constexpr ::std::uintmax_t majorVersion = DCOOL_MAJOR_VERSION;
	constexpr ::std::uintmax_t minorVersion = DCOOL_MINOR_VERSION;
	constexpr ::std::uintmax_t patchVersion = DCOOL_PATCH_VERSION;
	constexpr char8_t versionName[] = DCOOL_VERSION_NAME;
	constexpr char versionNativeName[] = DCOOL_VERSION_NATIVE_NAME;
	constexpr char8_t configDescription[] = DCOOL_CONFIG_DESCRIPTION;
	constexpr char configNativeDescription[] = DCOOL_CONFIG_NATIVE_DESCRIPTION;
	constexpr char8_t commonSelfDescription[] = DCOOL_SELF_DESCRIPTION;
	constexpr char commonSelfNativeDescription[] = DCOOL_SELF_NATIVE_DESCRIPTION;

	struct Version {
		::std::uintmax_t major = ::dcool::majorVersion;
		::std::uintmax_t minor = ::dcool::minorVersion;
		::std::uintmax_t patch = ::dcool::patchVersion;
		char8_t const* name = ::dcool::versionName;
		char const* nativeName = ::dcool::versionNativeName;
	};

	constexpr ::dcool::Version version;
}

#endif
