#pragma once

#include "ultraleap/haptics/common.hpp"

/*! \file */

/**
 * @defgroup VERSION_DEFINES Version Definitions
 *
 * @{
 */
/** The version of this SDK as an integer, with one byte for each of the major, minor and patch versions */
#define ULH_SDK_VERSION_INT   0x030000
/** A string representing the version of this SDK, without any platform or variant information */
#define ULH_SDK_VERSION       "3.0.0-beta.9"
/** A string representing the version of this SDK, with platform and variant information as appropriate */
#define ULH_SDK_VERSION_LONG  "3.0.0-beta.9-Windows-Win64-Research"
/** The major version of this SDK as an integer */
#define ULH_SDK_VERSION_MAJOR 3
/** The minor version of this SDK as an integer */
#define ULH_SDK_VERSION_MINOR 0
/** The patch version of this SDK as an integer */
#define ULH_SDK_VERSION_PATCH 0
/** A string representing the date this SDK was built, in ISO8601 format */
#define ULH_SDK_BUILD_DATE    "2021-05-05"

/** The name of the platform this SDK is built for */
#define ULH_SDK_PLATFORM_NAME "Windows-Win64"
/** The variant of this SDK build */
#define ULH_SDK_VARIANT       "Research"
/** @} */

namespace Ultraleap
{
namespace Haptics
{

/** A structure representing the version of this SDK
 *
 * Note that unlike most other classes this does not require a Library to access;
 * it can be created and accessed standalone prior to creating any other library context. */
struct ULH_API_CLASS VersionInfo
{
public:
    /** A string representing the version of this SDK, without any platform or variant information */
    const char* const  VersionString;
    /** A string representing the version of this SDK, with platform and variant information as appropriate */
    const char* const  LongVersionString;
    /** A string representing the date this SDK was built, in ISO8601 format */
    const char* const  BuildDate;
    /** The name of the platform this SDK is built for */
    const char* const  PlatformName;
    /** The variant of this SDK build */
    const char* const  VariantName;

    /** The major version of this SDK */
    const unsigned int MajorVersion;
    /** The minor version of this SDK */
    const unsigned int MinorVersion;
    /** The patch version of this SDK */
    const unsigned int PatchVersion;

    VersionInfo();


    ULH_API
    static void checkVersion(const unsigned int MajorVersion,
                             const unsigned int MinorVersion,
                             const unsigned int PatchVersion);

    static inline void checkVersion()
    {
        checkVersion(ULH_SDK_VERSION_MAJOR,
                     ULH_SDK_VERSION_MINOR,
                     ULH_SDK_VERSION_PATCH);
    }
};

} // namespace Haptics
} // namespace Ultraleap


namespace Ultrahaptics
{
using namespace Ultraleap::Haptics;
}
