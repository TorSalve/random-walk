#pragma once

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4583)
    #pragma warning(disable : 4582)
#endif
#include "ultraleap/haptics/result.hpp"
#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/stl_compat.hpp"

namespace Ultraleap
{
namespace Haptics
{

/** \cond EXCLUDE_FROM_DOCS */
class Library;
class LibraryImplementation;
/** \endcond */

/** Class representing the name and version of an application
 *
 * Objects of this type are "snapshots": they do not update with changes after their creation.
 *
 * This class is copyable and movable. */
class ULH_API_CLASS AppVersionInfo
{
    friend class LibraryImplementation;

public:
    /** Creates a new object with the specified information
     *
     * @param n the name of the end application
     * @param ver_major the major version of the end application
     * @param ver_minor the minor version of the end application
     * @param ver_patch the patch level of the end application
     * @param ver_build the build revision of the end application
     * @param ver_str the full version string of the end application */
    ULH_API explicit AppVersionInfo(
        const char* n,
        uint16_t ver_major = 0, uint16_t ver_minor = 0, uint16_t ver_patch = 0, uint16_t ver_build = 0,
        const char* ver_str = nullptr);

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API AppVersionInfo(const AppVersionInfo& info);
    ULH_API AppVersionInfo(AppVersionInfo&& info);
    ULH_API AppVersionInfo& operator=(const AppVersionInfo& other);
    ULH_API AppVersionInfo& operator=(AppVersionInfo&& other);
    /** \endcond */

    /** Destroys this version info object */
    ULH_API ~AppVersionInfo();

    /** The friendly name of the end application the library is being used in */
    inline std::string name() const { return _name(); }
    /** The major version of the end application the library is being used in */
    ULH_API uint16_t versionMajor() const;
    /** The minor version of the end application the library is being used in */
    ULH_API uint16_t versionMinor() const;
    /** The patch level of the end application the library is being used in */
    ULH_API uint16_t versionPatch() const;
    /** The build revision of the end application the library is being used in */
    ULH_API uint16_t versionBuild() const;
    /** The full version string of the end application the library is being used in */
    inline std::string versionString() const { return _versionString(); }

private:
    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API uh::string_wrapper _name() const;
    ULH_API uh::string_wrapper _versionString() const;
    class Implementation;
    Implementation* impl;
    /** \endcond */
};

/** Class representing an endpoint, such as a client connected to a service.
 *
 * Objects of this type are "snapshots": they do not update with changes after their creation.
 *
 * This class is copyable and movable. */
class ULH_API_CLASS EndpointInfo
{
public:
    /** Creates a new object from the specified details
     *
     * @param uuid a unique identifier representing this endpoint
     * @param libinfo the library version info of the endpoint
     * @param platform the runtime platform the endpoint is running on
     * @param appinfo the application version of the endpoint */
    ULH_API EndpointInfo(const char* uuid, AppVersionInfo libinfo, const char* platform, AppVersionInfo appinfo);
    /** Destroys this endpoint info object */
    ULH_API ~EndpointInfo();

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API EndpointInfo(const EndpointInfo& info);
    ULH_API EndpointInfo(EndpointInfo&& info);
    ULH_API EndpointInfo& operator=(const EndpointInfo& other);
    ULH_API EndpointInfo& operator=(EndpointInfo&& other);
    /** \endcond */

    /** The name and version of the Ultraleap Haptics runtime library this endpoint is using */
    ULH_API AppVersionInfo libraryVersion() const;
    /** The platform this endpoint's runtime library is running on */
    ULH_API const char* libraryPlatform() const;
    /** The name and version of the application running on this endpoint */
    ULH_API AppVersionInfo version() const;

    /** A unique identifier representing this endpoint
     *
     * NOTE: this UUID is only valid for the duration of the connection to this endpoint */
    inline std::string uuid() const
    {
        return _uuid();
    }

private:
    /** \cond EXCLUDE_FROM_DOCS */
    class Implementation;
    Implementation* impl;
    ULH_API uh::string_wrapper _uuid() const;
    /** \endcond */
};

/** Class representing the state of a runtime service.
 *
 * Objects of this type are "snapshots": they do not update with changes after their creation.
 *
 * This class is copyable and movable. */
class ULH_API_CLASS ServiceInfo
{
public:
    /** Destroys this service info object */
    ULH_API ~ServiceInfo();

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API ServiceInfo(const ServiceInfo& info);
    ULH_API ServiceInfo(ServiceInfo&& info);
    ULH_API ServiceInfo& operator=(const ServiceInfo& other);
    ULH_API ServiceInfo& operator=(ServiceInfo&& other);
    /** \endcond */

    /** The name and version of this service */
    ULH_API AppVersionInfo version() const;
    /** The connection path used to connect to this service */
    ULH_API const char* address() const;
    /** The runtime platform the service is running on */
    ULH_API const char* platform() const;
    /** The UUID of the requesting client on this service */
    ULH_API const char* this_client_uuid() const;

    /** The set of clients currently connected to the service, including this one */
    inline std::vector<EndpointInfo> clients() const
    {
        return _clients();
    }

    /** \cond EXCLUDE_FROM_DOCS */
    class Implementation;
    ServiceInfo(Implementation* impl);
    /** \endcond */
private:
    /** \cond EXCLUDE_FROM_DOCS */
    Implementation* impl;
    ULH_API uh::vector_wrapper<EndpointInfo> _clients() const;
    /** \endcond */
};

} // namespace Haptics
} // namespace Ultraleap
