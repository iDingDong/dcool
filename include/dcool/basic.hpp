#ifndef DCOOL_BASIC_HPP_INCLUDED_
#	define DCOOL_BASIC_HPP_INCLUDED_ 1

#	include <dcool/version.h>
#	include <dcool/config.h>
#	include <dcool/config.hpp>

#	include <cstdint>

namespace dcool {
	inline constexpr ::std::uintmax_t majorVersion = DCOOL_MAJOR_VERSION;
	inline constexpr ::std::uintmax_t minorVersion = DCOOL_MINOR_VERSION;
	inline constexpr ::std::uintmax_t patchVersion = DCOOL_PATCH_VERSION;
	inline constexpr char8_t versionName[] = DCOOL_VERSION_NAME;
	inline constexpr char versionNativeName[] = DCOOL_VERSION_NATIVE_NAME;
	inline constexpr char8_t configDescription[] = DCOOL_CONFIG_DESCRIPTION;
	inline constexpr char configNativeDescription[] = DCOOL_CONFIG_NATIVE_DESCRIPTION;
	inline constexpr char8_t commonSelfDescription[] = DCOOL_SELF_DESCRIPTION;
	inline constexpr char commonSelfNativeDescription[] = DCOOL_SELF_NATIVE_DESCRIPTION;

	struct Version {
		::std::uintmax_t major;
		::std::uintmax_t minor;
		::std::uintmax_t patch;
		char8_t const* name;
		char const* nativeName;
	};

	inline constexpr ::dcool::Version version = {
		.major = ::dcool::majorVersion,
		.minor = ::dcool::minorVersion,
		.patch = ::dcool::patchVersion,
		.name = ::dcool::versionName,
		.nativeName = ::dcool::versionNativeName
	};
}

#endif
