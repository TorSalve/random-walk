#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/device_info.hpp"
#include "ultraleap/haptics/device.hpp"
#include "ultraleap/haptics/local_time.hpp"
#include "ultraleap/haptics/transform.hpp"

#include <cstdint>

namespace Ultraleap
{
namespace Haptics
{

/** \cond EXCLUDE_FROM_DOCS */
class DriverDevice;
class EmitterImplementation;
/** \endcond */

/** Device Selection Mode
 *
 * @see \rstref{UsingMultipleDevices.rst}
 *
 * This determines how an Emitter with multiple devices decides which device to emit to. */
enum class DeviceSelectionMode : uint32_t {
    /** Device is selected based on distance between the point and the device. */
    Distance = 0,
    /** Device is selected based on angle between the point's direction and the normal of the device. */
    Direction = 1,
    /** Device is selected based on a mix between Direction and Distance. */
    Hybrid = 2
};

/** Rate Adjustment Mode
 *
 * This determines if and how the update rates of devices attached to an emitter are
 * adjusted when the number of control points is changed.
 *
 * If the requested control point count is not compatible with the current
 * device update rates for any device attached to an emitter, the behaviour
 * chosen here will determine what happens to resolve the conflict.
 *
 * @see StreamingEmitter::setControlPointCount */
enum class AdjustRate : uint32_t {
    /**
     * No update rates may be adjusted as part of the operation; if any changes to device
     * update rates would be required, the operation will be cancelled and return an error. */
    None = 0,
    /**
     * Any devices with update rates which are out of range for the new control point count
     * will have their update rates adjusted to their maximum rate at the new count;
     * devices whose update rates are already within range will not be altered. */
    AsRequired = 1,
    /**
     * All of the emitter's devices will have their update rates adjusted to the
     * maximum permitted rate for the new control point count. */
    All = 2
};

/** Control Point Count Adjustment Mode
 *
 * This determines if and how the control point count is adjusted when the update rate
 * of a device attached to an emitter is changed.
 *
 * @see Emitter::setDeviceUpdateRate */
enum class AdjustPointCount : uint32_t {
    /**
     * The control point count may not be adjusted as part of the operation. If a change
     * in control point count would be required, the operation will return an error. */
    No = 0,
    /**
     * The control point count may be adjusted as required to accommodate the newly-set
     * update rate. */
    AsRequired = 1
};

/** Specify whether points should be reemitted after setting a device transform. */
enum class ReEmitPoints : uint32_t {
    No = 0,
    Yes = 1
};

class ULH_API_CLASS Emitter
{
public:
    /** Add a device to this emitter. This action also has the effect of
     * acquiring a modification claim on the specified device.
     *
     * Adds a given device with an associated geometric transform to this emitter.
     * @see Device
     * @see Transform
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * @param device The Device to be added to this emitter.
     * @param transform The geometric transform for this device. */
    ULH_API result<void> addDevice(Device device, Transform transform);
    /** Add a modifiable device to this emitter.
     *
     * Adds a given modifiable device with an associated geometric transform to this emitter.
     * @see ModifiableDevice
     * @see Transform
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * \rst
     * Note that this operation takes ownership of the ModifiableDevice object passed in,
     * and thus it must be passed in using ``std::move``.
     * \endrst
     *
     * @param device The device to be added to this emitter.
     * @param transform The geometric transform for this device. */
    ULH_API result<void> addDevice(ModifiableDevice&& device, Transform transform);

    /** Removes a given device from this emitter.
     *
     * @param device The Device to be removed to this emitter. */
    ULH_API result<void> removeDevice(const Device& device);
    /** Removes a device at a given index from this emitter.
     *
     * @param device_index The index of the Device to be removed to this emitter. */
    ULH_API result<void> removeDevice(size_t device_index);
    /** Removes a given device from this emitter and returns it to the caller.
     *
     * @param device The Device to be removed to this emitter.
     * @return the modifiable device which has been removed. */
    ULH_API result<ModifiableDevice> removeAndTakeDevice(const Device& device);
    /** Removes a device at a given index from this emitter.
     *
     * @param device_index The index of the Device to be removed to this emitter.
     * @return the modifiable device which has been removed. */
    ULH_API result<ModifiableDevice> removeAndTakeDevice(size_t device_index);
    /** Removes all devices from this emitter. */
    ULH_API result<void> clearDevices();

    /** Get the geometric transform for a given device.
     *
     * Gets the position and orientation of a given device with respect to the basis of this emitter
     * i.e. the coordinate system in which controlpoint positions are being specified.
     * @see Transform
     *
     * @param device The Device to set the Transform for.
     * @return The affine transformation matrix for the specified device. */
    ULH_API result<Transform> getDeviceTransform(const Device& device) const;
    /** Get the geometric transform for a device at a given index.
     *
     * Gets the position and orientation of a given device with respect to the basis of this emitter
     * i.e. the coordinate system in which controlpoint positions are being specified.
     * @see Transform
     *
     * @param device_index The index of the device to set the Transform for.
     * @return The affine transformation matrix for the device at the specified index. */
    ULH_API result<Transform> getDeviceTransform(size_t device_index) const;

    /** Set a geometric transform for a given device.
     *
     * Sets the position and orientation of a given device with respect to the basis of this emitter
     * i.e. the coordinate system in which controlpoint positions are being specified.
     * @see Transform
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * @param device The Device to set the Transform for.
     * @param transform The affine transformation matrix for device. */
    ULH_API result<void> setDeviceTransform(const Device& device, Transform transform);
    /** Set a geometric transform for the device at a given index.
     *
     * Sets the position and orientation of the device at the given index
     * with respect to the basis of this emitter.
     * i.e. the coordinate system in which controlpoint positions are being specified.
     * @see Transform
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * @param device_index The index of the device to set the Transform for.
     * @param transform The affine transformation matrix for device. */
    ULH_API result<void> setDeviceTransform(size_t device_index, Transform transform);

    /** Sets the device selection mode for this emitter.
     *
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * @param mode the mode in which devices will be selected for output by this emitter.
     * @see DeviceSelectionMode */
    ULH_API result<void> setDeviceSelectionMode(DeviceSelectionMode mode);

    /** Gets the current device selection mode for this emitter.
     *
     * @see \rstref{UsingMultipleDevices.rst}
     *
     * @return An enum value describing the current device selection mode or an error.
     * @see DeviceSelectionMode. */
    ULH_API result<DeviceSelectionMode> getDeviceSelectionMode() const;

    /** Checks that this emitter has at least one device which is ready to emit.
     *
     * @return True if there is at least one ready device, false if not */
    ULH_API bool isReady() const;

    /** Checks whether this emitter is currently paused.
     *
     * @return True if the emitter is paused, false if not */
    ULH_API bool isPaused() const;

    /** Stops the emission of points from all active devices.
     *
     * @return True if there is at least one ready device, false if not */
    ULH_API result<void> stop();
    /** Pauses this emitter.
     *
     * @return void or an error if the pause request was unsuccessful. */
    ULH_API result<void> pause();
    /** Resumes this emitter.
     *
     * To be used on a paused emitter to resume emission.
     *
     * @return void or an error if the resume request was unsuccessful. */
    ULH_API result<void> resume();

    /** Enables this emitter.
     *
     * Enabling the emitter - if it is currently disabled - will re-claim all devices within
     * the emitter and reset them to the settings used by the emitter.
     *
     * If any devices cannot be re-claimed or (if connected) set back to the correct settings,
     * the operation will fail and all devices will be unclaimed again.
     *
     * @return void if the operation succeeded, or an error if it did not */
    ULH_API result<void> enable();

    /** Disables this emitter, stopping it if necessary.
     *
     * Disabling the emitter will unclaim all devices within the emitter, allowing them
     * to be used elsewhere. Whilst disabled, no changes can be made to the emitter.
     *
     * @return void if the operation succeeded, or an error if it did not. */
    ULH_API result<void> disable();

    /** Checks that this emitter is enabled.
     *
     * @return True if the emitter is enabled, false if not. */
    ULH_API bool isEnabled() const;

    /** Gets the maximum number of control points that can be outputted by this emitter.
     *
     * The control point limit of an emitter is determined by the lowest control point limit
     * of the devices attached to it. Different devices have different cp limits depending
     * on their current update rate. For a list of the cp limits for ultraleap haptics devices
     * at different update rates see \rstref{SupportedHardware.rst}.
     *
     * @return The control point limit of this emitter. */
    ULH_API size_t getControlPointLimit() const;

    /** Gets the rate at which timepoints should be supplied to this emitter.
     *
     * N.B. This function is only relevant to StreamingEmitter.
     * @see TimePointOnOutputInterval
     *
     * @return The current update rate of this emitter in Hertz. */
    ULH_API result<float> getEmitterUpdateRate() const;
    /** Gets the time interval between which timepoints should be supplied to this emitter.
     *
     * N.B. This function is only relevant to StreamingEmitter.
     * @see TimePointOnOutputInterval
     *
     * @return The duration of the current update interval of this emitter.
     * @see LocalDuration */
    ULH_API result<LocalDuration> getEmitterUpdateInterval() const;
    /** Sets the rate at which timepoints should be supplied to this emitter.
     *
     * N.B. This function is only relevant to StreamingEmitter.
     * @see TimePointOnOutputInterval
     *
     * @param new_rate The required update rate of this emitter in Hertz. */
    ULH_API result<void> setEmitterUpdateRate(float new_rate);

    /** Gets the current rate at which control points will be supplied to a given device
     * within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device The device to get the update rate for.
     * @return The update rate of the given device or an Error. */
    ULH_API result<float> getDeviceUpdateRate(const Device& device) const;
    /** Gets the current rate at which control points will be supplied to a device
     * at a given index within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device_index The index of the device to get the update interval for.
     * @return The update rate of the given device or an Error. */
    ULH_API result<float> getDeviceUpdateRate(size_t device_index) const;
    /** Gets the current interval between which control points will be supplied
     * to a given device within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device The device to get the update interval for.
     * @return The duration of the current update interval of the given device or an Error. */
    ULH_API result<LocalDuration> getDeviceUpdateInterval(const Device& device) const;
    /** Gets the current interval between which control points will be supplied to a device
     * at a given index within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device_index The index of the device to get the update interval for.
     * @return The duration of the current update interval of the device
     * at the given index or an Error. */
    ULH_API result<LocalDuration> getDeviceUpdateInterval(size_t device_index) const;
    /** Sets the rate at which control points will be supplied to a given device
     * within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device The device to set the update rate for.
     * @param new_rate The new update rate for device.
     * @param adjust An enum value specifying whether this request allows
     * the control point limit to be adjusted.
     * @see AdjustPointCount */
    ULH_API result<void> setDeviceUpdateRate(const Device& device, float new_rate, AdjustPointCount adjust);
    /** Sets the rate at which control points will be supplied to a device
     * at a given index within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device_index The index of the device to set the update rate for.
     * @param new_rate The new update rate for device.
     * @param adjust An enum value specifying whether this request allows
     * the control point limit to be adjusted.
     * @see AdjustPointCount */
    ULH_API result<void> setDeviceUpdateRate(size_t device_index, float new_rate, AdjustPointCount adjust);
    /** Gets the maximum rate at which a given number of control points can be supplied
     * to a given device within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device The device to get the maximum update rate for.
     * @param cp_count The number of control points to get the maximum update rate for.
     * @return The maximum update rate of the given device or an Error. */
    ULH_API result<float> getDeviceUpdateRateLimit(const Device& device, size_t cp_count) const;
    /** Gets the maximum rate at which a given number of control points can be supplied
     * to a device at a given index within this emitter.
     *
     * @see \rstref{UpdateRates.rst}
     *
     * @param device_index The index of the device to get the maximum update rate for.
     * @param cp_count The number of control points to get the maximum update rate for.
     * @return The maximum update rate of the device at the given index or an Error. */
    ULH_API result<float> getDeviceUpdateRateLimit(size_t device_index, size_t cp_count) const;

    ULH_API result<void> setExtendedOption(const char* key, const char* value);

    result<std::string> getExtendedOption(const char* key) const
    {
        auto r = _getExtendedOption(key);
        if (r.has_value())
            return r.value();
        else
            return make_unexpected(r.error());
    }

    ULH_API result<void> setDeviceExtendedOption(const Device& device, const char* key, const char* value);
    ULH_API result<void> setDeviceExtendedOption(size_t index, const char* key, const char* value);

    result<std::string> getDeviceExtendedOption(const Device& device, const char* key) const
    {
        auto r = _getDeviceExtendedOption(device, key);
        if (r.has_value())
            return r.value();
        else
            return make_unexpected(r.error());
    }

    result<std::string> getDeviceExtendedOption(size_t index, const char* key) const
    {
        auto r = _getDeviceExtendedOption(index, key);
        if (r.has_value())
            return r.value();
        else
            return make_unexpected(r.error());
    }

    /** Gets a list of devices attached to this emitter.
     *
     * @return A vector of Device objects or an Error.
     * @see Device */
    result<std::vector<Device>> getDevices() const
    {
        auto r = _getDevices();
        if (r.has_value())
            return r.value();
        else
            return make_unexpected(r.error());
    }

    /** Gets a device attached to this emitter with a given identifier.
     *
     * @param device_id Full identifier for the required device.
     * @return A Device object with the requested identifier or an Error.
     * @see Device */
    ULH_API result<Device> getDevice(const char* device_id) const;
    /** Gets the number of devices attached to this emitter.
     *
     * @return The number of devices attached to this emitter. */
    ULH_API size_t getDeviceCount() const;
    /** Gets a device attached to this emitter at a given index.
     *
     * @param device_index the index of the device within this emitter.
     * @return A Device object at the requested index or an Error.
     * @see Device */
    ULH_API result<Device> getDeviceAt(size_t device_index) const;

    /** Destroys the device and all its resources */
    ULH_API virtual ~Emitter();

    /** The default device selection mode applied to newly-created emitters */
    static constexpr DeviceSelectionMode DefaultDeviceSelectionMode = DeviceSelectionMode::Hybrid;

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API void* getStateEmitter();

    const EmitterImplementation* getImplementation() const;

    EmitterImplementation* getImplementation();

protected:
    Emitter(EmitterImplementation* iimpl);
    ULH_API Emitter(Emitter&& other) noexcept;
    ULH_API Emitter& operator=(Emitter&& other) noexcept;

    Emitter(const Emitter& other) = delete;
    Emitter& operator=(const Emitter& other) = delete;

    EmitterImplementation* impl;

    ULH_API result<uh::string_wrapper> _getExtendedOption(const char* key) const;
    ULH_API result<uh::string_wrapper> _getDeviceExtendedOption(const Device& device, const char* key) const;
    ULH_API result<uh::string_wrapper> _getDeviceExtendedOption(size_t index, const char* key) const;
    ULH_API result<uh::vector_wrapper<Device>> _getDevices() const;
    /** \endcond */
};

} // namespace Haptics
} // namespace Ultraleap
