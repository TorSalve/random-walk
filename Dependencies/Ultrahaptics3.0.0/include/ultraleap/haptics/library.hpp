#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/logging.hpp"
#include "ultraleap/haptics/device_info.hpp"
#include "ultraleap/haptics/device.hpp"
#include "ultraleap/haptics/stl_compat.hpp"
#include "ultraleap/haptics/version.hpp"
#include "ultraleap/haptics/app_info.hpp"

#include <cstdlib>
#include <string>
#include <vector>

namespace Ultraleap
{
namespace Haptics
{
/** \cond EXCLUDE_FROM_DOCS */
class LibraryImplementation;
class HardwareLibrary;
/** \endcond */

/** Helper class for creating event handlers.
 *
 * Contains a set of functions which will be called when the
 * event they refer to occurs on a device given by its identifier.
 *
 * Inheriting from this class and overiding one or more of its functions
 * allows handling to be triggered when one of the contained events occurs.
 *
 * \rst
.. warning::
    Attempting to add or remove LibraryEventListeners from within these
    functions is not supported and may result in deadlock.
\endrst
 *
 * \brief Event listener base class for handling library events */
class LibraryEventListener
{
public:
    /** Called when a device is connected or reconnected to the system.
     *
     * @param identifier full identifer for the device which has been connected */
    virtual void onDeviceConnected(const char* identifier);
    /** Called when a device is disconnected from the system.
     *
     * @param identifier full identifer for the device which has been disconnected */
    virtual void onDeviceDisconnected(const char* identifier);
    /** Called when a device has its modification rights claimed.
     *
     * @see ModifiableDevice
     *
     * @param identifier full identifer for the device which has been claimed */
    virtual void onDeviceModificationRightsClaimed(const char* identifier);
    /** Called when a device has its modification rights rescinded.
     *
     * @see ModifiableDevice
     *
     * @param identifier full identifer for the device which has been rescinded */
    virtual void onDeviceModificationRightsRescinded(const char* identifier);
    /** Called when an error is detected on a device.
     *
     * Note that this callback will only be called for devices which are opened for querying.
     *
     * @param identifier full identifier for the device which has had an error
     *
     * @param device_error_code the code of the error detected */
    virtual void onDeviceErrorDetected(const char* identifier, DeviceError device_error_code);
    /** Called when an error is cleared on a device.
     *
     * Note that this callback will only be called for devices which are opened for querying.
     *
     * @param identifier full identifier for the device which has had an error
     *
     * @param device_error_code the code of the error cleared */
    virtual void onDeviceErrorCleared(const char* identifier, DeviceError device_error_code);
    /** Called when this library connects to a service.
     *
     * @param info an object holding information about the service this library has connected to */
    virtual void onLibraryConnected(const EndpointInfo& info);
    /** Called when this library disconnects from a service.
     *
     * @param info an object holding information about the service
     * this library has disconnected from */
    virtual void onLibraryDisconnected(const EndpointInfo& info);
    /** Called when a client connects to the service this library is connected to.
     *
     * @param info an object holding information about the client which has connected */
    virtual void onClientConnected(const EndpointInfo& info);
    /** Called when a client disconnects from the service this library is connected to.
     *
     * @param info an object holding information about the client which has disconnected */
    virtual void onClientDisconnected(const EndpointInfo& info);
    /** Destroy any resources held by this listener. */
    virtual ~LibraryEventListener() {}

protected:
    /** Create or assign any resources to be held by this listener. */
    LibraryEventListener() {}
};

/** Embodiment of UH library resources.
 *
 * This library manages the connection to the ultraleap haptics daemon (if present),
 * event listeners, connected or previously connected devices,
 * logging and thread and process management.
 *
 * This class is copyable and movable.
 *
 * \rst
.. warning::
    Storing a Library object in a variable with static storage duration may cause
    unexpected behaviour on program exit; for this reason we recommend not storing
    Library objects as static or global variables.
\endrst
 *
 * \brief Main library and controller object */
class ULH_API_CLASS Library
{
public:
    /** Constructs a library object
     *
     * @param app_info information about the end application using the library */
    Library(AppVersionInfo app_info)
    {
        init(app_info);
        VersionInfo::checkVersion();
    }
    /** Constructs a library object */
    Library()
        : Library(AppVersionInfo(""))
    {}
    /** \cond EXCLUDE_FROM_DOCS */
    /** Copy constructor */
    ULH_API Library(const Library& other);
    /** Move constructor */
    ULH_API Library(Library&& other) noexcept;
    /** Copy assignment operator */
    ULH_API Library& operator=(const Library& other);
    /** Move assignment operator */
    ULH_API Library& operator=(Library&& other) noexcept;
    /** \endcond */
    /** Destroys the library and all its resources */
    ULH_API virtual ~Library();

    /** Equality operator
     *
     * @param other the object to compare this library to
     * @return true if this library is connected to the same backend as the other library, false otherwise */
    ULH_API bool operator==(const Library& other) const;
    /** Inequality operator
     *
     * @param other the object to compare this library to
     * @return false if this library is connected to the same backend as the other library, true otherwise */
    ULH_API bool operator!=(const Library& other) const;

    /** Connects to the SDK service by the default connection path
     *
     * @return void if the operation succeeded, or an Error if it did not */
    ULH_API virtual result<void> connect();

    /** Connects to the SDK service by a given protocol or path.
     *
     * @param connect_path connection path to SDK service e.g. 'local:path' to connect to a daemon at path or 'direct' to create a direct connection to the SDK.
     *
     * @return void if the operation succeeded, or an Error if it did not */
    ULH_API virtual result<void> connect(const char* connect_path);

    /** Disconnects from the SDK service
     *
     * @return void if the operation succeeded, or an Error if it did not */
    ULH_API result<void> disconnect();

    /** Check if the library is ready.
     *
     * Checks if this library holds a valid connection to the SDK backend.
     * Always returns true when the library is operating in direct mode.
     *
     * @return true if the connection is valid, or false if it is invalid or disconnected. */
    ULH_API bool isReady() const;

    /** Gets a snapshot of the current state of the service this library is connected to.
     *
     * @return a %ServiceInfo object representing the service state at the time of the call, or an Error */
    ULH_API result<ServiceInfo> getServiceInfo() const;

    /** Checks if this library holds a particular event listener.
     *
     * @param handler pointer to the event listener to check for.
     * @see LibraryEventListener
     *
     * @return true if this library already contains this listener, false if not. */
    ULH_API bool hasEventListener(const LibraryEventListener* handler) const;
    /** Checks if this library holds a particular event listener.
     *
     * @param handler pointer to the event listener to check for.
     * @see LibraryEventListener
     *
     * @return true if this library already contains this listener, false if not. */
    inline bool hasEventListener(const std::shared_ptr<LibraryEventListener>& handler) const
    {
        return hasEventListener(handler.get());
    }
    /** Adds an event listener to this library.
     *
     * @param handler pointer to an event listener or derived object.
     * @see LibraryEventListener
     *
     * @return true if the listener is added, false if the pointer is null,
     * or the library already contains this listener. */
    inline bool addEventListener(std::shared_ptr<LibraryEventListener> handler)
    {
        std::shared_ptr<LibraryEventListener>* sentinel = new std::shared_ptr<LibraryEventListener>(std::move(handler));
        if (_addEventListener(sentinel->get(), sentinel, &_deleteEventListener)) {
            return true;
        } else {
            _deleteEventListener(sentinel->get(), sentinel);
            return false;
        }
    }
    /** Removes an event listener from this library.
     *
     * @param handler pointer to an event listener or derived object.
     * @see LibraryEventListener
     *
     * @return true if the listener is removed, false if the pointer is null,
     * or the library does not contain this listener. */
    ULH_API bool removeEventListener(LibraryEventListener* handler);
    /** Removes an event listener from this library.
     *
     * @param handler pointer to an event listener or derived object.
     * @see LibraryEventListener
     *
     * @return true if the listener is removed, false if the pointer is null,
     * or the library does not contain this listener. */
    inline bool removeEventListener(const std::shared_ptr<LibraryEventListener>& handler)
    {
        return removeEventListener(handler.get());
    }

    /** Access the library's logging interface.
     *
     * @return A handle to the library's logging interface. */
    ULH_API Logging getLogging() const;

    /** @return True if the SDK is currently configured to allow
     * changing the priority of the current process to improve performance. */
    ULH_API bool isProcessPriorityManagementAllowed() const;
    /** @return True if the SDK is currently configured to allow
     * changing the priority of SDK threads to improve performance. */
    ULH_API bool isThreadPriorityManagementAllowed() const;
    /** Controls whether or not the SDK is allowed to reconfigure
     * the OS priority of the current process to improve performance.
     *
     * @param allowed If true, allows the SDK to change the current process's priority. */
    ULH_API void setProcessPriorityManagementAllowed(bool allowed);
    /** Controls whether or not the SDK is allowed to reconfigure the OS priority
     * of SDK-controlled threads within the current process to improve performance.
     *
     * @param allowed If true, allows the SDK to change the priority of SDK-controlled threads. */
    ULH_API void setThreadPriorityManagementAllowed(bool allowed);

    /** Get a known device.
     *
     * Gets a known device using its full identifier. If the requested device is diconnected,
     * the identifier is saved and the device will be added to the library if it is connected later.
     *
     * @param identifier the full identifier of the requested device e.g. 'USX:00000001'.
     *
     * @return the requested Device if it is connected, an Error if not. */
    ULH_API result<Device> getDevice(const char* identifier);
    /** Finds a connected device.
     *
     * @return a Device if one is connected, an Error if not. */
    ULH_API result<Device> findDevice();
    /** Finds a connected device of a given model.
     *
     * @param model the model name of the requested device type e.g. 'USX'.
     *
     * @return a Device of the requested model if one is connected, an Error if not. */
    ULH_API result<Device> findDevice(const char* model);
    /** Finds a connected device with a given set of capabilities.
     *
     * @param caps the required capabilities of the requested device.
     * @see DeviceFeatures
     *
     * @return a Device with the requested capabilities if one is connected, an Error if not. */
    ULH_API result<Device> findDevice(DeviceFeatures caps);
    /** Finds a connected device with of given model and set of capabilities.
     *
     * @param caps the required capabilities of the requested device.
     * @see DeviceFeatures
     *
     * @param model the model name of the requested device type e.g. 'USX'.
     *
     * @return a Device of the requested model with the requested
     * capabilities if one is connected, an Error if not. */
    ULH_API result<Device> findDevice(DeviceFeatures caps, const char* model);
    // Not recommended (STL version preferred)
    /** Gets the number of connected devices.
     *
     * @return the number of connected devices or an Error */
    ULH_API result<size_t> getDeviceCount() const;
    /** Gets the device at the given index.
     *
     * @param index the index of the requested device.
     *
     * @return the Device at the request index or an Error */
    ULH_API result<Device> getDeviceAt(size_t index);
    /** Gets a list of the currently connected devices.
     *
     * @return a vector of the currently connected devices or an Error. */
    result<std::vector<Device>> getDevices() const
    {
        auto result = _getDevices();
        if (result)
            return result.value();
        else
            return make_unexpected(result.error());
    }
    /** Gets a list of the currently connected unclaimed devices.
     *
     * @see ModifiableDevice
     *
     * @return a vector of the currently connected unclaimed devices or an Error. */
    result<std::vector<Device>> getUnclaimedDevices() const
    {
        auto result = _getUnclaimedDevices();
        if (result)
            return result.value();
        else
            return make_unexpected(result.error());
    }
    /** Gets a list of identifiers for the currently connected devices.
     *
     * @return a vector of strings containing the identifiers
     * of the currently connected devices or an Error. */
    result<std::vector<std::string>> getDeviceIdentifiers() const
    {
        auto result = _getDeviceIdentifiers();
        if (result)
            return result.value();
        else
            return make_unexpected(result.error());
    }
    /** Gets a list of identifiers for the currently connected unclaimed devices.
     *
     * @see ModifiableDevice
     *
     * @return a vector of strings containing the identifiers
     * of the currently connected unclaimed devices or an Error. */
    result<std::vector<std::string>> getUnclaimedDeviceIdentifiers() const
    {
        auto result = _getUnclaimedDeviceIdentifiers();
        if (result)
            return result.value();
        else
            return make_unexpected(result.error());
    }
    /** Checks with a given identifier is connected.
     *
     * @param device_identifier the full identifier for the device to check for.
     *
     * @return true if the requested device is connected, flase if it is not or an Error. */
    ULH_API result<bool> isDeviceConnected(const char* device_identifier) const;
    /** Checks with a given identifier is claimed.
     *
     * @param device_identifier the full identifier for the device to check for.
     *
     * @return true if the requested device is claimed, flase if it is not or an Error. */
    ULH_API result<bool> isDeviceClaimed(const char* device_identifier) const;

    /** \cond EXCLUDE_FROM_DOCS */
    // These do not need to be exported
    const LibraryImplementation* getImplementation() const;
    LibraryImplementation* getImplementation();

protected:
    /** Pointer for holding the library implementation */
    LibraryImplementation* impl;

private:
    ULH_API void init(AppVersionInfo app_info);

    ULH_API result<uh::vector_wrapper<Device>> _getDevices() const;
    ULH_API result<uh::vector_wrapper<Device>> _getUnclaimedDevices() const;
    ULH_API result<uh::vector_wrapper<uh::string_wrapper>> _getDeviceIdentifiers() const;
    ULH_API result<uh::vector_wrapper<uh::string_wrapper>> _getUnclaimedDeviceIdentifiers() const;

    using EventListenerDeleter = void (*)(LibraryEventListener* ptr, void* sentinel);
    ULH_API bool _addEventListener(LibraryEventListener* ptr, void* sentinel, EventListenerDeleter deleter);
    inline static void _deleteEventListener(LibraryEventListener* ptr, void* sentinel)
    {
        std::shared_ptr<LibraryEventListener>* sptr = reinterpret_cast<std::shared_ptr<LibraryEventListener>*>(sentinel);
        delete sptr;
    }
    /** \endcond */
};

/** \cond EXCLUDE_FROM_DOCS */
inline void LibraryEventListener::onDeviceConnected(const char*) {}
inline void LibraryEventListener::onDeviceDisconnected(const char*) {}
inline void LibraryEventListener::onDeviceModificationRightsClaimed(const char*) {}
inline void LibraryEventListener::onDeviceModificationRightsRescinded(const char*) {}
inline void LibraryEventListener::onDeviceErrorDetected(const char*, DeviceError) {}
inline void LibraryEventListener::onDeviceErrorCleared(const char*, DeviceError) {}
inline void LibraryEventListener::onLibraryConnected(const EndpointInfo&) {}
inline void LibraryEventListener::onLibraryDisconnected(const EndpointInfo&) {}
inline void LibraryEventListener::onClientConnected(const EndpointInfo&) {}
inline void LibraryEventListener::onClientDisconnected(const EndpointInfo&) {}
/** \endcond */

} // namespace Haptics

} // namespace Ultraleap
