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
#include "ultraleap/haptics/device_info.hpp"
#include "ultraleap/haptics/transform.hpp"
#include "ultraleap/haptics/app_info.hpp"
#include "ultraleap/haptics/stl_compat.hpp"

#include <memory>
#include <string>

namespace Ultraleap
{
namespace Haptics
{
/** \cond EXCLUDE_FROM_DOCS */
class DeviceImplementation;
class DeviceBackend;
class DriverDevice;
class ModifiableDevice;
/** \endcond */

/** The status of modification claims on a device */
enum class DeviceClaimStatus {
    /** No client currently has a modification claim */
    None,
    /** An object created by this client library has a modification claim */
    ThisClient,
    /** A client other than this one has a modification claim */
    OtherClient
};

/** A Device object contains the information about the device and manages its modification rights.
 *
 * This class is copyable and movable.
 *
 * \brief A reference to an Ultraleap Haptics device */
class ULH_API_CLASS Device
{
    /** \cond EXCLUDE_FROM_DOCS */
    friend class LibraryBackend;
    /** \endcond */
public:
    /** \cond EXCLUDE_FROM_DOCS */
    /** Copy constructor */
    ULH_API Device(const Device& other);
    /** Move constructor */
    ULH_API Device(Device&& other);
    /** Copy assignment operator */
    ULH_API Device& operator=(const Device& other);
    /** Move assignment operator */
    ULH_API Device& operator=(Device&& other);
    /** \endcond */
    /** Destroys the device and all its resources */
    ULH_API virtual ~Device();

    /** Equality operator
     *
     * @param other the device to check this device against
     * @return true if this device represents the same device using the same library, false otherwise */
    ULH_API bool operator==(const Device& other) const;
    /** Inequality operator
     *
     * @param other the device to check this device against
     * @return false if this device represents the same device using the same library, false otherwise */
    ULH_API bool operator!=(const Device& other) const;

    /** Checks if this device is connected.
     *
     * @return true if connected, false if not or an error. */
    ULH_API result<bool> isConnected() const;
    /** Refreshes the library's list of connections and checks if this device is connected.
     *
     * @return true if connected, false if not or an error. */
    ULH_API result<bool> checkConnection();
    /** Checks that this device is connected and that it is ready to emit.
     *
     * @return true if ready, false if not or an error. */
    ULH_API result<bool> isReady() const;

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API result<uint32_t> getActiveErrorBitfield() const;

    ULH_API result<uint32_t> getChangedErrorBitfield() const;
    /** \endcond */

    /** Gets the device info for this device.
     *
     * @see DeviceInfo
     *
     * @return a DeviceInfo object or an error. */
    ULH_API result<DeviceInfo> getInfo() const;
    /** Gets the device identifier for this device e.g. 'USX:00000001'.
     *
     * Please note that this returns a pointer to a string held within the Device object.
     *
     * @return the full identifier for this device. */
    ULH_API const char* getIdentifier() const;
    /** Get the default "kit" transform for this device.
     *
     * Gets the default geometric transformation which converts vectors from the basis of the
     * tracking system attached to this device, to the basis of this device.
     *
     * @see Transform
     * @see DeviceTransforms
     *
     * Note that the transform returned will only be valid for
     * Ultraleap Haptics products which include a tracking system.
     *
     * @return the default kit Transform for this device if it could be determined, or an Error. */
    ULH_API result<Transform> getKitTransform() const;
    /** Gets the minimum separation at which points can be emitted from this device.
     *
     * @return the minimum point separation in metres or an Error. */
    ULH_API result<float> getMinimumPointSeparation() const;

    /** Get the current status of a modification claim on this device
     *
     * @see ModifiableDevice
     *
     * @return the current status of this device's modification claim, if any */
    ULH_API result<DeviceClaimStatus> getClaimStatus() const;

    /** Get the client which currently has a modification claim on this device
     *
     * @see ModifiableDevice
     *
     * @return the client with a claim on this device, or NoMatches if no client has a claim */
    ULH_API result<EndpointInfo> getClaimOwner() const;

    /** Get the modifiable version of this device.
     *
     * In order to obtain the modifiable version, this device must hold the modification claim.
     *
     * @see ModifiableDevice
     *
     * @return a ModifiableDevice or an Error. */
    ULH_API result<ModifiableDevice> getModifiableDevice();

    /** Get an option value related to this device
     *
     * @param key the name of the option to get
     * @return the option value if the option was recognised, or an error otherwise */
    result<std::string> getExtendedOption(const char* key) const
    {
        auto r = _getExtendedOption(key);
        if (r.has_value())
            return r.value();
        else
            return make_unexpected(r.error());
    }

    /** Get the driver device for this device.
     *
     * @see DriverDevice
     *
     * @return a shared pointer to a const DriverDevice or an Error. */
    result<std::shared_ptr<const DriverDevice>> getDriverDevice() const
    {
        const DriverDevice* ddptr = nullptr;
        void* sentinel = nullptr;
        auto alloc_result = _getDriverDevice_alloc(ddptr, sentinel);
        if (alloc_result) {
            if (ddptr != nullptr) {
                return std::shared_ptr<const DriverDevice>(
                    ddptr,
                    [sentinel](const DriverDevice* d) { _getDriverDevice_dealloc(d, sentinel); });
            } else {
                return nullptr;
            }
        } else {
            return make_unexpected(alloc_result);
        }
    }
    /** Get the driver device for this device.
     *
     * @see DriverDevice
     *
     * @return a raw pointer to a const DriverDevice or an Error. */
    ULH_API result<const DriverDevice*> getRawDriverDevice() const;

    /** \cond EXCLUDE_FROM_DOCS */
    const DeviceImplementation* getImplementation() const;

protected:
    Device(DeviceImplementation* iimpl);
    Device(std::shared_ptr<DeviceBackend> backend);
    DeviceImplementation* impl;

private:
    ULH_API result<uh::string_wrapper> _getExtendedOption(const char* key) const;

    ULH_API result<void> _getDriverDevice_alloc(const DriverDevice*& device, void*& sentinel) const;

    ULH_API static result<void> _getDriverDevice_dealloc(const DriverDevice* device, void* sentinel);
    /** \endcond */
};

/** A representation of a hardware device which can be modified.
 *
 * A %ModifiableDevice object allows the properties of the device to be changed.
 * Only one such object can exist for any given device at a time.
 *
 * This class is movable.
 *
 * \brief A device with claimed modification rights */
class ULH_API_CLASS ModifiableDevice : public Device
{
    /** \cond EXCLUDE_FROM_DOCS */
    friend class Device;
    friend class LibraryBackend;
    /** \endcond */
public:
    /** \cond EXCLUDE_FROM_DOCS */
    /** Move constructor */
    ULH_API ModifiableDevice(ModifiableDevice&& other);
    /** Move assignment operator */
    ULH_API ModifiableDevice& operator=(ModifiableDevice&& other);
    ModifiableDevice(const ModifiableDevice& other) = delete;
    ModifiableDevice& operator=(const ModifiableDevice& other) = delete;
    /** \endcond */

    /** Destroys the modifiable device and all its resources */
    ULH_API ~ModifiableDevice();

    ULH_API result<void> setExtendedOption(const char* key, const char* value);

    /** Hard resets this device.
     *
     * @return nothing or an Error. */
    ULH_API result<void> hardReset();

    using Device::getDriverDevice;
    /** Get the driver device for this device.
     *
     * @see DriverDevice
     *
     * @return a shared pointer to a DriverDevice or an Error. */
    result<std::shared_ptr<DriverDevice>> getDriverDevice()
    {
        DriverDevice* ddptr = nullptr;
        void* sentinel = nullptr;
        auto alloc_result = _getDriverDevice_alloc(ddptr, sentinel);
        if (alloc_result) {
            if (ddptr != nullptr) {
                return std::shared_ptr<DriverDevice>(
                    ddptr,
                    [sentinel](DriverDevice* d) { _getDriverDevice_dealloc(d, sentinel); });
            } else {
                return nullptr;
            }
        } else {
            return make_unexpected(alloc_result);
        }
    }
    using Device::getRawDriverDevice;
    /** Get the driver device for this device.
     *
     * @see DriverDevice
     *
     * @return a raw pointer to a DriverDevice or an Error. */
    ULH_API result<DriverDevice*> getRawDriverDevice();

    /** \cond EXCLUDE_FROM_DOCS */
    using Device::getImplementation;
    DeviceImplementation* getImplementation();

protected:
    ModifiableDevice(DeviceImplementation* iimpl);

private:
    ULH_API result<void> _getDriverDevice_alloc(DriverDevice*& device, void*& sentinel);
    ULH_API static result<void> _getDriverDevice_dealloc(DriverDevice* device, void* sentinel);
    /** \endcond */
};

} // namespace Haptics
} // namespace Ultraleap
