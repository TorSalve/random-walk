#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/driver.hpp"
#include "ultraleap/haptics/device.hpp"
#include "ultraleap/haptics/local_time.hpp"
#include "ultraleap/haptics/device_time.hpp"
#include "ultraleap/haptics/library.hpp"
#include "ultraleap/haptics/state_interval.hpp"

#include <vector>

namespace Ultraleap
{
namespace Haptics
{
/** \cond EXCLUDE_FROM_DOCS */
/* Forward declarations of internal implementation classes */
class StateEmitterImplementation;

template <class StateType, class TimeType>
class SingleStateEmitterImplementation;

template <class StateType, class TimeType>
class StreamingStateEmitterImplementation;
/** \endcond */

class ULH_RESEARCH_API_CLASS StateEmitter
{
public:
    std::vector<Device> getDevices() const
    {
        return _getDevices();
    }

    ULH_RESEARCH_API bool hasDevice(const Device& d) const;
    ULH_RESEARCH_API result<Device> getDevice(const char* identifier) const;
    ULH_RESEARCH_API result<Device> getDeviceAt(size_t index) const;

    /** Get the driver device for a device we own with this identifier
     * Accessing the driver device in this way is unsafe but may be necessary for research purposes. Use with caution.
     * @see DriverDevice
     *
     * @return a shared pointer to a DriverDevice or an Error. */
    result<std::shared_ptr<DriverDevice>> getDriverDeviceUnsafe(const char* identifier)
    {
        DriverDevice* ddptr = nullptr;
        void* sentinel = nullptr;
        auto alloc_result = _getDriverDeviceUnsafe_alloc(&ddptr, &sentinel, identifier);
        if (alloc_result) {
            if (ddptr != nullptr) {
                return std::shared_ptr<DriverDevice>(
                    ddptr,
                    [sentinel](DriverDevice*) { _getDriverDeviceUnsafe_dealloc(sentinel); });
            } else {
                return nullptr;
            }
        } else {
            return make_unexpected(alloc_result);
        }
    }

    /** Get the driver device for a device we own with this device index
     * Accessing the driver device in this way is unsafe but may be necessary for research purposes. Use with caution.
     * @see DriverDevice
     *
     * @return a shared pointer to a DriverDevice or an Error. */
    result<std::shared_ptr<DriverDevice>> getDriverDeviceUnsafeAt(size_t device_index)
    {
        DriverDevice* ddptr = nullptr;
        void* sentinel = nullptr;
        auto alloc_result = _getDriverDeviceUnsafeAt_alloc(&ddptr, &sentinel, device_index);
        if (alloc_result) {
            if (ddptr != nullptr) {
                return std::shared_ptr<DriverDevice>(
                    ddptr,
                    [sentinel](DriverDevice*) { _getDriverDeviceUnsafe_dealloc(sentinel); });
            } else {
                return nullptr;
            }
        } else {
            return make_unexpected(alloc_result);
        }
    }

    ULH_RESEARCH_API size_t getDeviceCount() const;

    ULH_RESEARCH_API result<size_t> getMinimumStateSize(const Device& device) const;
    ULH_RESEARCH_API result<size_t> getMinimumStateSize(size_t device_index) const;

    /** @return The maximum size of states which may be sent via this state emitter */
    ULH_RESEARCH_API result<size_t> getCommonMaximumStateSize() const;
    ULH_RESEARCH_API result<size_t> getMaximumStateSize(const Device& device) const;
    ULH_RESEARCH_API result<size_t> getMaximumStateSize(size_t device_index) const;

    ULH_RESEARCH_API virtual LocalDuration getTargetBufferDuration() const { return {}; };
    ULH_RESEARCH_API virtual result<void> setTargetBufferDuration(LocalDuration new_duration) { return {}; };

    ULH_RESEARCH_API result<size_t> addDevice(Device device);
    ULH_RESEARCH_API result<size_t> addDevice(ModifiableDevice&& device);
    ULH_RESEARCH_API result<void> removeDevice(const Device& device);
    ULH_RESEARCH_API result<void> removeDevice(size_t index);
    ULH_RESEARCH_API result<ModifiableDevice> removeActiveDevice(const Device& device);
    ULH_RESEARCH_API result<ModifiableDevice> removeActiveDevice(size_t index);
    ULH_RESEARCH_API result<size_t> removeUnavailableDevices();
    ULH_RESEARCH_API result<void> clearDevices();

    ULH_RESEARCH_API result<void> enable();
    ULH_RESEARCH_API result<void> disable();
    ULH_RESEARCH_API bool isEnabled() const;

    /** Stops emission
     *
     * @return SUCCESS if the device was instructed to stop emission, or an error code if not */
    ULH_RESEARCH_API result<void> stopEmission();
    ULH_RESEARCH_API result<void> stopEmission(const Device& device);
    ULH_RESEARCH_API result<void> stopEmission(size_t device_index);

    ULH_RESEARCH_API StateEmitter(const StateEmitter&) = delete;
    ULH_RESEARCH_API StateEmitter(StateEmitter&&) = delete;
    ULH_RESEARCH_API StateEmitter& operator=(const StateEmitter&) = delete;
    ULH_RESEARCH_API StateEmitter& operator=(StateEmitter&&) = delete;
    ULH_RESEARCH_API virtual ~StateEmitter();

    StateEmitterImplementation* getImplementation();

protected:
    /** \cond EXCLUDE_FROM_DOCS */
    StateEmitter(StateEmitterImplementation* impl);
    StateEmitterImplementation* impl;

    template <class StateType, class TimeType>
    StateInterval<StateType, TimeType> _createInterval(size_t nTimes) const
    {
        auto devices = getDevices();
        StateInterval<StateType, TimeType> interval(nullptr, nTimes, devices.data(), devices.size());
        for (size_t i = 0; i < devices.size(); ++i)
            interval.setDevice(i, devices[i]);
        return interval;
    }
    /** \endcond */
private:
    /** \cond EXCLUDE_FROM_DOCS */
    ULH_RESEARCH_API uh::vector_wrapper<Device> _getDevices() const;
    /** \endcond */

    ULH_RESEARCH_API result<std::shared_ptr<DriverDevice>> _getDriverDeviceUnsafe_alloc(DriverDevice** ddptr, void** sentinel, const char* identifier);
    ULH_RESEARCH_API result<std::shared_ptr<DriverDevice>> _getDriverDeviceUnsafeAt_alloc(DriverDevice** ddptr, void** sentinel, size_t device_index);
    ULH_RESEARCH_API static result<void> _getDriverDeviceUnsafe_dealloc(void* sentinel);
};

/** A state emitter, to which individual states representing a single point in time can be sent.
 *
 * Note that state emitters do not perform any connect/disconnect actions on underlying devices;
 * it is left to the user to keep track of this status themselves.
 *
 * This template has implementations for the following StateType%s:
 *
 *  * ControlPointStateWithFrequency
 *  * CyclingTransducersState
 *
 * @tparam StateType the type of state to emit to the device */
template <class StateType, class TimeType = LocalTimePoint>
class ULH_RESEARCH_API_CLASS SingleStateEmitter : public StateEmitter
{
public:
    /** Construct a new state emitter using the specified (already-connected) device */
    ULH_RESEARCH_API SingleStateEmitter(Library lib);
    ULH_RESEARCH_API SingleStateEmitter(const SingleStateEmitter<StateType, TimeType>&) = delete;
    ULH_RESEARCH_API SingleStateEmitter(SingleStateEmitter<StateType, TimeType>&&) noexcept;
    ULH_RESEARCH_API SingleStateEmitter& operator=(const SingleStateEmitter<StateType, TimeType>&) = delete;
    ULH_RESEARCH_API SingleStateEmitter& operator=(SingleStateEmitter<StateType, TimeType>&&) noexcept;
    /** Destroys the emitter
     *
     * Note that this does *not* disconnect the underlying devices (if any). */
    ULH_RESEARCH_API virtual ~SingleStateEmitter() = default;

    /** Sends one or more states to the devices
     *
     * @param states A pointer to an array of states (if states_size is 1, this can be a pointer to a state object)
     *
     * @return SUCCESS if the states were sent for emission, or an error code if not */
    ULH_RESEARCH_API result<void> emit(const StateInterval<StateType, TimeType>& states);

    StateInterval<StateType, TimeType> createInterval(size_t nTimes = 1) const { return _createInterval<StateType, TimeType>(nTimes); }

private:
    void changeFrontend();
};

/* Forward declaration of StreamingStateEmitter */
template <class StateType, class TimeType>
class StreamingStateEmitter;

/** Callback used to populate emission data in a state emitter
 *
 * @param state_emitter The state emitter this callback is registered to
 * @param interval The current interval this callback should populate data for
 * @param interval_begin The time indicating the start of this interval
 * @param interval_end The time indicating the end of this interval
 * @param submission_deadline The time by which this callback must have returned control to the SDK
 * @param user_pointer The user pointer passed to the SDK when this callback was registered
 *
 * @tparam StateType the type of state in use */
template <class StateType, class TimeType>
using StateEmissionCallback = void (*)(StreamingStateEmitter<StateType, TimeType>& state_emitter,
    StateInterval<StateType, TimeType>& interval,
    const TimeType& interval_begin,
    const TimeType& interval_end,
    const LocalTimePoint& submission_deadline,
    void* user_pointer);

/** Update mode that streaming state emitters can run in
 *
 * When in interpolated mode, any number of devices can be added to a state emitter,
 * and their individual update rates will be decoupled from the rate at which the emitter
 * itself updates. This is the mode used by all top-level emitters.
 *
 * When in device-linked mode, only one device can be added to a state emitter. The emitter's
 * update rate is then linked to match the update rate of the device itself, and no interpolation
 * is performed. This is more similar to the operation of state emitters in earlier SDK versions. */
enum class StreamingUpdateMode {
    /** Automatically interpolates between a stable emitter update rate, and disparate device update rates */
    Interpolated,
    /** Links the emitter update rate to the update rate of the single device in the emitter */
    DeviceLinked
};

enum class StreamingCallbackMode {
    Normal,
    Intensive
};

/** A streaming state emitter, used to send time-aware emission data to a device
 *
 * The emitter can be used in two ways: either using callbacks or directly emitting states.
 *
 * Callbacks are the preferred method of use, as this handles most timing and setup tasks automatically.
 * This allows you to register a callback function, which is called regularly while the emitter is started.
 * You can then populate data within the callback function, which is then emitted shortly afterwards.
 *
 * It is also possible to directly send data to be emitted on the device using the emit methods,
 * but it will be more difficult to ensure the timing and synchronisation is correct.
 *
 * Note that state emitters do not perform any connect/disconnect actions on underlying devices;
 * it is left to the user to keep track of this status themselves.
 *
 * This template has implementations for the following StateType%s:
 *
 *  * TransducersState
 *  * FocusPointState
 *
 * @tparam StateType the type of state in use */
template <class StateType, class TimeType = LocalTimePoint>
class ULH_RESEARCH_API_CLASS StreamingStateEmitter : public StateEmitter
{
public:
    /** Construct a new state emitter using the specified (already-connected) device
     *
     * @param lib the library to use for emission
     * @param update_mode the update mode to use when emitting */
    ULH_RESEARCH_API StreamingStateEmitter(Library lib, StreamingUpdateMode update_mode);
    ULH_RESEARCH_API StreamingStateEmitter(const StreamingStateEmitter<StateType, TimeType>&) = delete;
    ULH_RESEARCH_API StreamingStateEmitter(StreamingStateEmitter<StateType, TimeType>&&) noexcept;
    ULH_RESEARCH_API StreamingStateEmitter<StateType, TimeType>& operator=(const StreamingStateEmitter<StateType, TimeType>&) = delete;
    ULH_RESEARCH_API StreamingStateEmitter<StateType, TimeType>& operator=(StreamingStateEmitter<StateType, TimeType>&&) noexcept;
    /** Note that this does *not* disconnect the underlying device. */
    ULH_RESEARCH_API virtual ~StreamingStateEmitter() = default;

    using StateEmitter::addDevice;
    ULH_RESEARCH_API result<size_t> addDevice(Device device, size_t state_data_count);
    ULH_RESEARCH_API result<size_t> addDevice(ModifiableDevice&& device, size_t state_data_count);
    ULH_RESEARCH_API result<size_t> addDevice(Device device, size_t state_data_count, float rate);
    ULH_RESEARCH_API result<size_t> addDevice(ModifiableDevice&& device, size_t state_data_count, float rate);

    /** Emit the specified emission data to the device
     *
     * The times provided must be monotonically increasing; if they are not, the method will return BAD_QUERY.
     *
     * Please note that this method may not be called if the callback is active (i.e. isRunning() is true).
     * If this is attempted, the method will return FAILED.
     *
     * @param states      An array of length points_size containing the states to emit at the specified times
     *
     * @return SUCCESS if the points were sent to the device to be emitted, or an error code if not */
    ULH_RESEARCH_API result<void> emit(const StateInterval<StateType, TimeType>& states);
    /** Emit the specified emission data to the device
     *
     * The times provided must be monotonically increasing; if they are not, the method will return BAD_QUERY.
     *
     * Please note that this method may not be called if the callback is active (i.e. isRunning() is true).
     * If this is attempted, the method will return FAILED.
     *
     * @param states      An array of length points_size containing the states to emit at the specified times
     * @param time_control_left_user The time at which the user finished populating this data
     *
     * @return SUCCESS if the points were sent to the device to be emitted, FAILED if the callback is active, or an error code if not */
    ULH_RESEARCH_API result<void> emit(const StateInterval<StateType, TimeType>& states, TimeType time_control_left_user);

    ULH_RESEARCH_API result<LocalTimePoint> getLastEmitterUpdateTime() const;
    /** The emission time for which an update was last sent to the specified device */
    ULH_RESEARCH_API result<TimeType> getLastDeviceUpdateTime(const Device& device) const;
    ULH_RESEARCH_API result<TimeType> getLastDeviceUpdateTime(size_t device_index) const;

    ULH_RESEARCH_API float getCallbackRate() const;
    ULH_RESEARCH_API LocalDuration getCallbackInterval() const;
    ULH_RESEARCH_API result<void> setCallbackRate(float rate);

    ULH_RESEARCH_API LocalDuration getTargetBufferDuration() const final;
    ULH_RESEARCH_API result<void> setTargetBufferDuration(LocalDuration new_duration) final;

    ULH_RESEARCH_API float getEmitterUpdateRate() const;
    ULH_RESEARCH_API LocalDuration getEmitterUpdateInterval() const;
    ULH_RESEARCH_API result<void> setEmitterUpdateRate(float rate);

    ULH_RESEARCH_API float getCommonDeviceUpdateRate() const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRate(const Device& device) const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRate(size_t device_index) const;
    ULH_RESEARCH_API result<LocalDuration> getDeviceUpdateInterval(const Device& device) const;
    ULH_RESEARCH_API result<LocalDuration> getDeviceUpdateInterval(size_t device_index) const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRateLimit(const Device& device) const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRateLimit(size_t device_index) const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRateLimit(const Device& device, size_t cp_count) const;
    ULH_RESEARCH_API result<float> getDeviceUpdateRateLimit(size_t device_index, size_t cp_count) const;
    ULH_RESEARCH_API result<float> getCommonDeviceUpdateRateLimit(size_t cp_count) const;

    ULH_RESEARCH_API result<void> setDeviceUpdateConfig(const Device& device, size_t state_size, float rate);
    ULH_RESEARCH_API result<void> setDeviceUpdateConfig(size_t device_index, size_t state_size, float rate);
    ULH_RESEARCH_API result<void> setCommonDeviceUpdateConfig(size_t state_size, float rate);

    ULH_RESEARCH_API result<typename TimeType::duration> getEmissionLatency() const;

    /** @return True if some mode of emission is currently active, or false if not */
    ULH_RESEARCH_API bool isRunning() const;

    /** Gets the number of times the callback has been called since start
     *
     * @return the number of calls. */
    ULH_RESEARCH_API size_t getCallbackIterations() const;
    /** Gets the number of times the callback has been missed since start
     *
     * @return the number of missed calls. */
    ULH_RESEARCH_API size_t getMissedCallbackIterations() const;

    /** Start the emitter's event loop, enabling the emission callback to be used to populate data.
     *
     * @return True if the emitter was started or already running, or false if it could not be started. */
    ULH_RESEARCH_API result<bool> startCallback();
    /** Stop the emitter's event loop and halt all output from the device.
     *
     * @return True if the emitter is now stopped (including if it was already stopped), or false if the stop action failed. */
    ULH_RESEARCH_API result<bool> stopCallback();

    ULH_RESEARCH_API bool isCallbackRunning() const;

    ULH_RESEARCH_API bool isOnCallbackThread() const;

    ULH_RESEARCH_API result<bool> startManualEmission();

    ULH_RESEARCH_API result<bool> stopManualEmission();

    ULH_RESEARCH_API bool isManualEmissionRunning() const;

    ULH_RESEARCH_API StreamingCallbackMode getEmissionCallbackMode() const;
    ULH_RESEARCH_API result<void> setEmissionCallbackMode(StreamingCallbackMode mode);

    /** Set the callback used to send emission data to the device
     *
     * @param callback_function The function to be queried for data to send to the device. May be null, which will stop all output.
     * @param set_user_pointer  A user-provided pointer which will be passed to the callback function when called. May be null. */
    ULH_RESEARCH_API result<void> setEmissionCallback(StateEmissionCallback<StateType, TimeType> callback_function, void* set_user_pointer);

    ULH_RESEARCH_API size_t getCommonStateSize() const;

    /** Get the current number of elements within each individual state */
    ULH_RESEARCH_API result<size_t> getStateSize(size_t device_index) const;
    ULH_RESEARCH_API result<size_t> getStateSize(const Device& device) const;

    StateInterval<StateType, TimeType> createInterval(size_t nTimes) const { return _createInterval<StateType, TimeType>(nTimes); }
};
} // namespace Haptics

} // namespace Ultraleap
