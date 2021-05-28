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
#include "ultraleap/haptics/device_time.hpp"
#include "ultraleap/haptics/driver.hpp"
#include "ultraleap/haptics/local_time.hpp"
#include "ultraleap/haptics/stl_compat.hpp"
#include "ultraleap/haptics/transducers.hpp"

#include <array>
#include <cstdint>
#include <vector>

namespace Ultraleap
{
namespace Haptics
{

/** Valid modes a device could be operating in */
enum class DeviceMode : uint32_t {
    /** Primary mode, controlling operation */
    Primary = 0,
    /** Secondary mode, accepting input from a primary device */
    Secondary = 1
};

/** Transducer types which could be connected to a device */
enum class TransducerType : uint32_t {
    /** Type not yet identified (e.g. disconnected device) */
    Unknown = 0x0000,

    /** Murata MA40S4S transducer */
    MA40S4S = 0x0001,

    /** Unused */
    TestType0 = 0xFF00,
    /** Unused */
    TestType1 = 0xFF01,
    /** Unused */
    TestType2 = 0xFF02,
    /** Unused */
    TestType3 = 0xFF03,
    /** Unused */
    TestType4 = 0xFF04,
    /** Unused */
    TestType5 = 0xFF05,
    /** Unused */
    TestType6 = 0xFF06,
    /** Unused */
    TestType7 = 0xFF07
};

/** Transducer models a device could support */
enum class TransducerModel : uint32_t {
    /** Model not yet identified (e.g. disconnected device) */
    Unknown = 0x0000,

    /** Traditional directivity model */
    Directivity = 0x0001,
    /** Updated model for MA40S4S transducers */
    MA40S4Sv1 = 0x0002,

    /** Unused */
    TestModel0 = 0xFF00,
    /** Unused */
    TestModel1 = 0xFF01,
    /** Unused */
    TestModel2 = 0xFF02,
    /** Unused */
    TestModel3 = 0xFF03,
    /** Unused */
    TestModel4 = 0xFF04,
    /** Unused */
    TestModel5 = 0xFF05,
    /** Unused */
    TestModel6 = 0xFF06,
    /** Unused */
    TestModel7 = 0xFF07
};

/** Sensor types which could be present on a device */
enum class SensorType : uint32_t {
    /** Unknown sensor */
    Unknown,
    /** Temperature sensor */
    Temperature,
    /** Voltage sensor */
    Voltage,
    /** Current sensor */
    Current,
    /** Power (wattage) sensor */
    Power,
    /** Speed sensor */
    Speed,
    /** Invalid value */
    End
};

/** Units which could be returned by sensors */
enum class SensorUnit : uint32_t {
    /** Unknown units */
    Unknown,
    /** Temperature in Celsius */
    Celsius,
    /** Temperature in Kelvin */
    Kelvin,
    /** Voltage in Volts */
    Volts,
    /** Current in Amps */
    Amps,
    /** Power in Watts */
    Watts,
    /** Resistance in Ohms */
    Ohms,
    /** A Boolean (true/false) value */
    Boolean,
    /** Speed in Revolutions Per Minute */
    RPM,
    /** Invalid value */
    End
};

/** Broad locations which sensors could be present on a device */
enum class SensorLocation : uint32_t {
    /** Unknown location */
    Unknown,
    /** The device's logic board */
    LogicBoard,
    /** The device's transducer (array) board */
    TransducerBoard,
    /** The device's enclosure */
    Enclosure,
    /** Invalid value */
    End
};

/** Specific locations which sensors could be present on a device */
enum class SensorSubLocation : uint32_t {
    /** Unknown location */
    Unknown,
    /** Generic Power Supply */
    PowerSupply,
    /** Generic Logic */
    Logic,
    /** Power Supply for Logic */
    LogicPowerSupply,
    /** Transducers */
    Transducers,
    /** Power Supply for Transducers */
    TransducersPowerSupply,
    /** HPU */
    HPU,
    /** Power Supply for HPU */
    HPUPowerSupply,
    /** Microcontroller (control device) */
    Microcontroller,
    /** Power Supply for Microcontroller */
    MicrocontrollerPowerSupply,
    /** FPGA */
    FPGA,
    /** Power Supply for FPGA */
    FPGAPowerSupply,
    /** Shift Registers */
    ShiftRegisters,
    /** Power Supply for Shift Registers */
    ShiftRegistersPowerSupply,
    /** Top-left item (quadrant 1) */
    TopLeft,
    /** Top-right item (quadrant 2) */
    TopRight,
    /** Bottom-left item (quadrant 3) */
    BottomLeft,
    /** Bottom-right item (quadrant 4) */
    BottomRight,
    /** Generic system location */
    System,
    /** Invalid value */
    End
};

struct SensorDetails
{
    size_t index = 0;
    SensorType type = SensorType::Unknown;
    SensorUnit unit = SensorUnit::Unknown;
    int unit_power = 0;
    SensorLocation location = SensorLocation::Unknown;
    SensorSubLocation sublocation = SensorSubLocation::Unknown;
    float raw_value = 0.0f;

    float value() const { return raw_value * static_cast<float>(std::pow(10.0, unit_power)); }

    std::string to_string() const;
};

namespace v4
{
struct ControlPointConfiguration
{
    enum class SolveType {
        EnergyDotN = 0,
        VelocityDotN = 1,
        Energy = 2,
        Pressure = 3,
    };

    enum class WaveType {
        Converging = 0,
        Diverging = 1,
    };

    enum class VelocityDirection {
        Forwards = 0,
        Backwards = 1,
    };

    bool enabled = true;    //note: should always be true until we finalise 3.0 and allow sparse data sending?
    std::uint8_t group = 0; //0, 1, 2, or 3
    bool dynamic_ranging = false;
    SolveType solve_type = SolveType::EnergyDotN;
    WaveType wave_type = WaveType::Converging;
    VelocityDirection velocity_direction = VelocityDirection::Forwards;
};

//end
} // namespace v4

enum class DeviceTrigger {
    Automatic,
    Rise,
    Fall,
};

enum class DataDumpCategory {
    V4CP = 0,
    V4CPConfig = 1,
    V4TX = 2,
    Latency = 3,
    Clock = 4,
    USB = 5,
    CTRequest = 6,
    CTSend = 7,
    COUNT = 8,
};

class ULH_RESEARCH_API_CLASS DriverDeviceListener
{
public:
    ULH_RESEARCH_API virtual void onDeviceReady() {}
    ULH_RESEARCH_API virtual void onDeviceUnready() {}
    ULH_RESEARCH_API virtual void onDeviceErrorsChanged(uint32_t active_errors_bitfield, uint32_t changed_errors_bitfield);

protected:
    ULH_RESEARCH_API DriverDeviceListener() = default;
};

/** \brief Persistent device with driver loaded.
 *
 * Driver-level details on a device. Can exist without a backing device,
 * but once connected will keep attempting to connect to the first device seen. */
class ULH_RESEARCH_API_CLASS DriverDevice
{
    friend class DirectLibraryBackend;

public:
    /** Destructor. */
    ULH_RESEARCH_API ~DriverDevice();

    /** Deleted copy constructor */
    DriverDevice(const DriverDevice& other) = delete;
    /** Deleted copy assignment operator */
    DriverDevice& operator=(const DriverDevice& other) = delete;

    /**
     * Destroy a previously-created device object.
     *
     * @param idevice The device object. It is permitted for this argument to be nullptr, in which case no action is taken.
     */
    ULH_RESEARCH_API static void destroyDevice(DriverDevice* idevice);

    /** Flush updates to device. */
    ULH_RESEARCH_API result<void> flushUpdates();

    /** Disconnect the driver layer from the device.
     *
     * @return True if successfully disconnected, false if already disconnected */
    ULH_RESEARCH_API bool disconnect();

    /** @return True if the device is connected */
    ULH_RESEARCH_API bool isConnected() const;

    ULH_RESEARCH_API void addDriverDeviceListener(DriverDeviceListener&);
    ULH_RESEARCH_API void removeDriverDeviceListener(DriverDeviceListener&);

    /** @return True if the device is in the halted state and it cannot accept new data */
    ULH_RESEARCH_API bool isHalted() const;

    /** @return Array type name */
    ULH_RESEARCH_API const char* getArrayTypeName() const;

    /** @return Array identifier */
    ULH_RESEARCH_API const char* getArrayIdentifier() const;

    /** @return Firmware version */
    ULH_RESEARCH_API result<const char*> getFirmwareVersion() const;

    /** @return Firmware build date */
    ULH_RESEARCH_API result<const char*> getFirmwareBuildDate() const;

    /** @return Array serial number */
    ULH_RESEARCH_API result<const char*> getArraySerial() const;

    /** @return Array layout name */
    ULH_RESEARCH_API result<const char*> getArrayLayoutName() const;

    /** @return The wavelength of the transducers on the device, in metres */
    ULH_RESEARCH_API result<float> getTransducerWavelength() const;

    /** @return The frequency of the transducers on the device, in Hz */
    ULH_RESEARCH_API result<float> getTransducerFrequency() const;

    /** Get minimum separation between control points.
     *
     * @return The minimum separation between control points for the device, in metres */
    ULH_RESEARCH_API result<float> getMinimumSeparation() const;

    /** @return Bitmask of driver capabilities advertised by the driver
     * implementation for this device */
    ULH_RESEARCH_API result<int> getCapabilitiesBitmask() const;

    /** @return A vector of transducer positions, directions and individual phase
     * biases for the transducer at each index */
    ULH_RESEARCH_API result<TransducerContainer> getTransducers() const;

    /** @param input A vector of transducer positions, directions and individual phase
     * biases for the transducer at each index */
    ULH_RESEARCH_API result<void> setTransducers(TransducerContainer& input);

    /** Set the transducer positions, directions and individual phase
     * biases, back to their default values for the transducer at each index */
    ULH_RESEARCH_API result<void> setDefaultTransducers();

    /*********** Capping Level ************/

    /** Gets the current capping level reported by pre-thor devices.
     *
     * @return the capping level of the device or an error */
    ULH_RESEARCH_API result<float> getCappingLevel() const;

    /** Sets the capping level used by the device's solver for pre-thor devices.
     *
     * This will also set the host capping level
     *
     * @param cap the capping level to send to the device
     * @return void, or an error */
    ULH_RESEARCH_API result<void> setCappingLevel(float cap);

    /** Sets the default range value used by thor. Can be overriden per-cp via the ControlPointState
     * Can be set during emission
     * Capping unit is in Pascals squared (acoustic pressure) for the default energy solve type.*/
    ULH_RESEARCH_API result<void> setCappingRange(float cap, std::uint8_t cp_index);

    /** Sets the average capping value used by thor. 
     * Cannot be set during emission
     * Capping unit is in Pascals (acoustic pressure). */
    ULH_RESEARCH_API result<void> setCappingAverage(float cap, std::uint8_t cp_index);

    /** Sets the peak capping value used by thor. 
     * Cannot be set during emission
     * Capping unit is in Pascals (acoustic pressure). */
    ULH_RESEARCH_API result<void> setCappingPeak(float cap, std::uint8_t cp_index);

    ULH_RESEARCH_API result<float> getCappingRange(std::uint8_t cp_index) const;
    ULH_RESEARCH_API result<float> getCappingAverage(std::uint8_t cp_index) const;
    ULH_RESEARCH_API result<float> getCappingPeak(std::uint8_t cp_index) const;

    /** Gets the default capping level reported by the pre-thor device.
     *
     * @return the default capping level of the device or an error */
    ULH_RESEARCH_API result<float> getDefaultDeviceCappingLevel() const;

    /* Thor */
    ULH_RESEARCH_API bool hasSupportForCappingPeak() const;
    /* Thor */
    ULH_RESEARCH_API bool hasSupportForCappingAverage() const;
    /* Thor */
    ULH_RESEARCH_API bool hasSupportForCappingRange() const;
    /* Pre-Thor */
    ULH_RESEARCH_API bool hasSupportForCappingLevel() const;

    /*********** Control Point State Updates ************/

    /** Queues a set of control point states to be sent to the device; typically only one state would be sent.
     *
     * @param cp_states an array of state objects
     * @param states_count the number of states in the array
     * @param add_header whether to add a packet header to the sent packet
     * @return SUCCESS if the update was queued, FAILED or DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support control point state updates */
    ULH_RESEARCH_API result<void> controlPointStateUpdates(
        const AMControlPointState* cp_states,
        size_t states_count,
        bool add_header = true);

    /** Gets the maximum number of control points this device can accept in one state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getControlPointCountLimitInState() const;

    /** @return SUCCESS if the device supports control point state updates, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForControlPointStateUpdates() const;

    /*********** Streamed Control Point States ************/

    /** Queues a message to be sent to the device to send a set of control point states.
     *
     * @param cp_states an array of states for the device to output
     * @param states_count the number of states provided
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> controlPointStateStreamingUpdates(
        const ControlPointState* cp_states,
        size_t states_count);

    /** Gets the maximum number of streamed control points which the device supports in a single state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getControlPointCountLimitInStreamingState() const;

    /** Gets the maximum update rate for control point updates supported by the device
     *
     * @param maximum_number_of_control_points_in_state the maximum focus point count to get the update rate for
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForControlPointStreamingStates(size_t maximum_number_of_control_points_in_state) const;

    /** Get the current update rate for streamed updates
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<float> getStreamingUpdateRate() const;

    /** Set the update rate for streamed updates (using the new streaming method, rather than Time Point Streaming).
     *
     * @param update_rate The new rate in Hz.
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<void> setStreamingUpdateRate(float update_rate) const;

    /** @return SUCCESS if the device supports this mode of operation, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForControlPointStreamingStateUpdates() const;

    /*********** Cycling Transducer State Updates ************/

    /** Queues to be sent a set of transducer states to be cycled between
     *
     * @param transducers_states an array of transducer state objects
     * @param states_count the number of state objects in the array
     * @param mod_freq the frequency with which to cycle between the states
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> cyclingTransducersStateUpdates(
        const TransducersState* transducers_states,
        size_t states_count,
        float mod_freq);

    /** @return SUCCESS if the device supports this mode of operation, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForCyclingTransducersStateUpdates() const;

    /*********** Device Trigger ************/

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the local time at which the device should fall
     * @param trigger_type the type of trigger message. Automatic for default pulse duration (V2/V4), Fall and Rise to control the signal manually (V4)
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const LocalTimePoint& time, DeviceTrigger trigger_type = DeviceTrigger::Automatic);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fall
     * @param trigger_type the type of trigger message. Automatic for default pulse duration (V2/V4), Fall and Rise to control the signal manually (V4)
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const DeviceTimePoint& time, DeviceTrigger trigger_type = DeviceTrigger::Automatic);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param data the data to send with the trigger signal, or nullptr if no data is required
     * @param data_len the length of the data to send; must be 0 if data is nullptr
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const LocalTimePoint& time, const void* data, size_t data_len);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param data the data to send with the trigger signal, or nullptr if no data is required
     * @param data_len the length of the data to send; must be 0 if data is nullptr
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const DeviceTimePoint& time, const void* data, size_t data_len);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param pulse_duration the duration of the trigger in host time
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const LocalTimePoint& time, const LocalDuration& pulse_duration);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param pulse_duration the duration of the trigger in device time
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const LocalTimePoint& time, const DeviceDuration& pulse_duration);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param pulse_duration the duration of the trigger in host time
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const DeviceTimePoint& time, const LocalDuration& pulse_duration);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param pulse_duration the duration of the trigger in device time
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendTriggerSignal(const DeviceTimePoint& time, const DeviceDuration& pulse_duration);

    /** @return SUCCESS if this device has support for sending trigger signals, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForDeviceTrigger() const;

    /** @return SUCCESS if this device has support for sending v4 trigger signals, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForV4DeviceTrigger() const;

    /** @return SUCCESS if this device has support for sending v2 trigger signals, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForV2DeviceTrigger() const;

    /*********** V4 Transducers State At Time Points ************/

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param tx_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param local_time_focus_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v4transducersStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const TransducersState* tx_states,
        size_t states_count,
        LocalTimePoint local_time_focus_left_user);

    ULH_RESEARCH_API result<void> v4transducersStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const TransducersState* tx_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum update rate supported by this device for transducer state updates
     *
     * @return SUCCESS if the rate limit was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV4TransducersStatesAtTimePoints() const;

    /** Gets the FPGA buffer depth for transducers states
     *
     * @return the maximum number of transducers states which can be queued by the state interpolator at
     * one time, or an error */
    ULH_RESEARCH_API result<uint16_t> getBufferDepthForV4TransducersStatesAtTimePoints() const;

    /** Gets the number of transducers states currently queued
     *
     * @return the number of transducers states which are currently queued in the state interpolator, or
     * an error */
    ULH_RESEARCH_API result<uint16_t> getQueueLengthForV4TransducersStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV4TransducersStatesAtTimePoints() const;

    /*********** Transducers State At Time Points ************/

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param tx_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param local_time_focus_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v3transducersStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const TransducersState* tx_states,
        size_t states_count,
        LocalTimePoint local_time_focus_left_user);

    ULH_RESEARCH_API result<void> v3transducersStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const TransducersState* tx_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum update rate supported by this device for transducer state updates
     *
     * @return SUCCESS if the rate limit was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV3TransducersStatesAtTimePoints() const;

    /** Gets the FPGA buffer depth for transducers states
     *
     * @return the maximum number of transducers states which can be queued by the state interpolator at
     * one time, or an error */
    ULH_RESEARCH_API result<uint16_t> getBufferDepthForV3TransducersStatesAtTimePoints() const;

    /** Gets the number of transducers states currently queued
     *
     * @return the number of transducers states which are currently queued in the state interpolator, or
     * an error */
    ULH_RESEARCH_API result<uint16_t> getQueueLengthForV3TransducersStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV3TransducersStatesAtTimePoints() const;

    /*********** Focus Point State At Time Points ************/

    /** Queues a message to be sent to the device to send a set of focus point states
     *  with associated time points for the device to emit.
     *
     * @param local_time_points an array of host-side time points to emit states at
     * @param fp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @param time_control_left_user the time at which the user returned this data
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v3focusPointStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const FocusPointState* fp_states,
        size_t states_count,
        LocalTimePoint time_control_left_user);

    ULH_RESEARCH_API result<void> v3focusPointStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const FocusPointState* fp_states,
        size_t states_count,
        DeviceTimePoint time_control_left_user);

    /** Gets the maximum number of focus points which the device supports in a single state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getV3FocusPointCountLimitInTimePointState() const;

    /** Gets the maximum update rate for focus point updates supported by the device
     *
     * @param maximum_number_of_focus_points_in_state the maximum focus point count to get the update rate for
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV3FocusPointStatesAtTimePoints(size_t maximum_number_of_focus_points_in_state) const;

    /** Gets the FPGA buffer depth for focus point states
     *
     * @return the maximum number of focus point states which can be queued by the state interpolator at
     * one time, or an error */
    ULH_RESEARCH_API result<uint16_t> getBufferDepthForV3FocusPointStatesAtTimePoints() const;

    /** Gets the number of focus point states currently queued
     *
     * @return the number of focus point states which are currently queued in the state interpolator, or
     * an error */
    ULH_RESEARCH_API result<uint16_t> getQueueLengthForV3FocusPointStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV3FocusPointStatesAtTimePoints() const;

    /*********** V2 Focus Point State At Time Points ************/

    /** Queues a message to be sent to the device to send a set of focus point states
     *  with associated time points for the device to emit.
     *
     * @param local_time_points an array of host-side time points to emit states at
     * @param fp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @param time_control_left_user the time at which the user returned this data
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2focusPointStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const FocusPointState* fp_states,
        size_t states_count,
        LocalTimePoint time_control_left_user);

    /** Queues a message to be sent to the device to send a set of focus point states
     *  with associated time points for the device to emit.
     *
     * @param local_time_points an array of host-side time points to emit states at
     * @param fp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2focusPointStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const FocusPointState* fp_states,
        size_t states_count);

    /** Queues a message to be sent to the device to send a set of focus point states
     *  with associated time points for the device to emit.
     *
     * @param device_time_points an array of host-side time points to emit states at
     * @param fp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @param time_control_left_user the time at which the user returned this data
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2focusPointStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const FocusPointState* fp_states,
        size_t states_count,
        DeviceTimePoint time_control_left_user);

    /** Queues a message to be sent to the device to send a set of focus point states
     *  with associated time points for the device to emit.
     *
     * @param device_time_points an array of device-side time points to emit states at
     * @param fp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2focusPointStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const FocusPointState* fp_states,
        size_t states_count);

    /** Gets the maximum number of focus points which the device supports in a single state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getV2FocusPointCountLimitInTimePointState() const;

    /** Gets the maximum update rate for focus point updates supported by the device
     *
     * @param maximum_number_of_focus_points_in_state the maximum focus point count to get the update rate for
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV2FocusPointStatesAtTimePoints(size_t maximum_number_of_focus_points_in_state) const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV2FocusPointStatesAtTimePoints() const;

    /*********** Control Point State Update At Time Points ************/

    /** Queues a message to be sent to the device to send a set of control point states
     *  with associated time points for the device to emit.
     *
     * @param local_time_points an array of host-side time points to emit states at
     * @param cp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @param local_time_control_left_user the time at which the user returned this data
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v3controlPointStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const ControlPointState* cp_states,
        size_t states_count,
        LocalTimePoint local_time_control_left_user);

    ULH_RESEARCH_API result<void> v3controlPointStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const ControlPointState* cp_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum number of control points which the device supports in a single state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getV3ControlPointCountLimitInTimePointState() const;

    /** Gets the maximum update rate for control point updates supported by the device
     *
     * @param maximum_number_of_control_points_in_state the maximum control point count to get the update rate for
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV3ControlPointStatesAtTimePoints(size_t maximum_number_of_control_points_in_state) const;

    /** Gets the FPGA buffer depth for control point states
     *
     * @return the maximum number of control point states which can be queued by the state interpolator at
     * one time, or an error */
    ULH_RESEARCH_API result<uint16_t> getBufferDepthForV3ControlPointStatesAtTimePoints() const;

    /** Gets the number of control point states currently queued
     *
     * @return the number of control point states which are currently queued in the state interpolator, or
     * an error */
    ULH_RESEARCH_API result<uint16_t> getQueueLengthForV3ControlPointStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV3ControlPointStatesAtTimePoints() const;

    /*********** Control Point State Update At Time Points V4 ************/

    /** Queues a message to be sent to the device to send a set of control point states
     *  with associated time points for the device to emit.
     *
     * @param local_time_points an array of host-side time points to emit states at
     * @param cp_states an array of states for the device to output
     * @param states_count the number of states (and time points) provided
     * @param local_time_control_left_user the time at which the user returned this data
     * @return SUCCESS if the message was queued, FAILED or DISCONNECTED if the device was
     *         unavailable, or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v4controlPointStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const ControlPointState* cp_states,
        size_t states_count,
        LocalTimePoint local_time_control_left_user);

    ULH_RESEARCH_API result<void> v4controlPointStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const ControlPointState* cp_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum number of control points which the device supports in a single state
     *
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getV4ControlPointCountLimitInTimePointState() const;

    /** Gets the maximum update rate for control point updates supported by the device
     *
     * @param maximum_number_of_control_points_in_state the maximum control point count to get the update rate for
     * @return SUCCESS if the query succeeded, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV4ControlPointStatesAtTimePoints(size_t maximum_number_of_control_points_in_state) const;

    /** Gets the FPGA buffer depth for control point states
     *
     * @return the maximum number of control point states which can be queued by the state interpolator at
     * one time, or an error */
    ULH_RESEARCH_API result<uint16_t> getBufferDepthForV4ControlPointStatesAtTimePoints() const;

    /** Gets the number of control point states currently queued
     *
     * @return the number of control point states which are currently queued in the state interpolator, or
     * an error */
    ULH_RESEARCH_API result<uint16_t> getQueueLengthForV4ControlPointStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV4ControlPointStatesAtTimePoints() const;

    /*********** Control Point Configuration ************/

    ULH_RESEARCH_API inline result<void> v4updateControlPointConfiguration(std::vector<v4::ControlPointConfiguration> configs) const
    {
        return _v4updateControlPointConfiguration(std::move(configs));
    }

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForControlPointConfiguration() const;

    /*********** V4 Transducers State At Time Points ************/

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param tx_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param local_time_focus_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v4commonTransducerStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const CommonTransducerState* tx_states,
        size_t states_count,
        LocalTimePoint local_time_focus_left_user);

    ULH_RESEARCH_API result<void> v4commonTransducerStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const CommonTransducerState* tx_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum update rate supported by this device for transducer state updates
     *
     * @return SUCCESS if the rate limit was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV4CommonTransducerStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV4CommonTransducerStatesAtTimePoints() const;

    /*********** Transducers State At Time Points ************/

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param tx_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param local_time_focus_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v3commonTransducerStatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const CommonTransducerState* tx_states,
        size_t states_count,
        LocalTimePoint local_time_focus_left_user);

    ULH_RESEARCH_API result<void> v3commonTransducerStatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const CommonTransducerState* tx_states,
        size_t states_count,
        DeviceTimePoint frame_device_time);

    /** Gets the maximum update rate supported by this device for transducer state updates
     *
     * @return SUCCESS if the rate limit was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV3CommonTransducerStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV3CommonTransducerStatesAtTimePoints() const;

    /*********** Hard Reset ************/

    /** Hard resets the device. The device may choose to enter a cooldown period in which
     *  it claims to be disconnected whilst in the process of restarting.
     *
     * @return SUCCESS if the reset command was sent to the device, or FEATURE_UNSUPPORTED if the device does not support this action */
    ULH_RESEARCH_API result<void> hardResetDevice();
    /** Hard resets the device to a specific mode. The device may choose to enter a
     *  cooldown period in which it claims to be disconnected whilst in the process of restarting.
     *
     * @param mode the mode to reset the device to
     * @return SUCCESS if the reset command was sent to the device, or FEATURE_UNSUPPORTED if the device does not support this action */
    ULH_RESEARCH_API result<void> hardResetDeviceToMode(DeviceMode mode);

    /** @return SUCCESS if the device supports the reset command, or FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForHardReset() const;

    /** @return SUCCESS if the device supports resetting to a specific mode, or FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForHardResetToMode() const;

    /*********** Heartbeat ************/

    /** @return SUCCESS if the device uses a heartbeat function, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForHeartbeat() const;

    /*********** Latency Monitor ************/

    /** Estimate the latency between host and device to match the current time
     *
     * @return SUCCESS if the action succeeded, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query. */
    ULH_RESEARCH_API result<DeviceDuration> getEstimatedHostToDeviceLatency();

    /** @return SUCCESS if the device supports latency monitoring, or FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForLatencyMonitor() const;

    /*********** Mode Settings ************/

    /** @return true if the device was instructed to reset and apply changes to the modes of operation, false if not */
    ULH_RESEARCH_API result<void> v2resetDeviceAndApplyChanges();
    /** @return A list of modes available for querying and setting */
    ULH_RESEARCH_API result<const char*> getV2Modes() const;
    /** Gets the type of data a mode setting contains (e.g. bool, string)
     *
     * @param mode_name the name of the mode to query
     * @return The type of the requested mode setting. May be nullptr if an invalid mode name is specified. */
    ULH_RESEARCH_API result<const char*> getV2ModeSettingType(const char* mode_name) const;
    /** Gets the current setting of a specified operating mode
     *
     * @param mode_name the name of the mode to query
     * @return The current setting of the requested mode. May be nullptr if an invalid mode name is specified. */
    ULH_RESEARCH_API result<const char*> getCurrentV2ModeSetting(const char* mode_name) const;
    /** Set a mode setting to a new value
     *
     * @param mode_name the name of the mode to set
     * @param mode_setting the new value to set it to
     * @return SUCCESS if the mode was set, FAILED if the operation was attempted but failed,
     *         BAD_QUERY if the mode name was invalid, or FEATURE_UNSUPPORTED if this is not supported by the device */
    ULH_RESEARCH_API result<void> setV2Mode(const char* mode_name, const char* mode_setting);

    /** @return SUCCESS if this device supports mode settings, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForV2ModeSettings() const;

    /*********** Query Custom Sensors ************/

    /** Gets the most recent value for the specified sensor
     *
     * @param sensor_id the ID of the sensor to query
     * @return SUCCESS if the value was queried, BAD_QUERY if the sensor ID was invalid,
     *         DISCONNECTED if the device was unavailable, or FEATURE_UNSUPPORTED if it does not support querying sensors */
    ULH_RESEARCH_API result<int32_t> getCustomSensorValue(short sensor_id) const;

    /** @return SUCCESS if the device supports querying sensors, or FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForQueryingCustomSensors() const;

    /*********** Soft Reset ***********/
    /** Instructs the device to change its output configuration modes back to their defaults
     *
     * @return SUCCESS if the command was sent, DISCONNECTED if the device was unavailable, or FEATURE_UNSUPPORTED if support is not present */
    ULH_RESEARCH_API result<void> resetDeviceOutputConfigToDefaults();

    /** @return SUCCESS if the device supports sending soft reset commands, or FEATURE_UNSUPPORTED if not */
    ULH_RESEARCH_API bool hasSupportForSoftReset() const;

    /*********** Stop Command ************/

    /** Sends a message to the device to stop emission.
     *
     * @return SUCCESS if the message was sent to the device, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support the stop command. */
    ULH_RESEARCH_API result<void> haltEmission();

    /** @return SUCCESS if the device supports the stop command, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForStopCommand() const;

    /*********** Timer Monitor ************/

    /** Gets an estimate of the device time on this specific device which correlates to the provided host time
     *
     * @param local_time_point the host-side time to estimate the device time for
     * @return SUCCESS if the time was estimated, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support device time estimation */
    ULH_RESEARCH_API result<DeviceTimePoint> getEstimatedDeviceTime(LocalTimePoint local_time_point) const;

    /** Gets an estimate of the device time on this specific device which correlates to the provided host time
     *
     * @param local_time_in_future the host-side duration in the future to estimate the device time for
     * @return SUCCESS if the time was estimated, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support device time estimation */
    ULH_RESEARCH_API result<DeviceTimePoint> getEstimatedDeviceTime(LocalDuration local_time_in_future) const;

    /** Gets an estimate of the current device time on this specific device
     *
     * @return SUCCESS if the time was estimated, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support device time estimation */
    ULH_RESEARCH_API result<DeviceTimePoint> getEstimatedDeviceTime() const;

    /** Gets an estimate of the host time represented by the specified device time
     *
     * @param device_time_point the device time to convert
     * @return SUCCESS if the conversion succeeded, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support device time estimation */
    ULH_RESEARCH_API result<LocalTimePoint> getEstimatedHostTime(DeviceTimePoint device_time_point) const;

    /** Gets an estimate of the host time represented by the specified device time
     *
     * @param device_time_in_future the device duration in the future to convert
     * @return SUCCESS if the conversion succeeded, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support device time estimation */
    ULH_RESEARCH_API result<LocalTimePoint> getEstimatedHostTime(DeviceDuration device_time_in_future) const;

    /** Gets an estimate of the host duration represented by a device duration
     *
     * @param device_duration the device duration to convert
     * @return the corresponding DeviceDuration object or an error */
    ULH_RESEARCH_API result<LocalDuration> getEstimatedLocalDuration(DeviceDuration device_duration) const;

    /** Gets an estimate of the device duration represented by the specified local duration
     *
     * @param local_duration the local duration to convert
     * @return the corresponding DeviceDuration object or an error */
    ULH_RESEARCH_API result<DeviceDuration> getEstimatedDeviceDuration(LocalDuration local_duration) const;

    /** @return The number of device time ticks for this device in one second of local time */
    ULH_RESEARCH_API result<int64_t> getDeviceClockRate() const;

    /** @return SUCCESS if the device supports timer monitoring, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForTimerMonitor() const;

    /*********** Transducer Type Info ************/

    /** @return SUCCESS if the device supports transducer model information, FEATURE_UNSUPPORTED if it does not */
    ULH_RESEARCH_API bool hasSupportForTransducerTypeInfo() const;

    /** Discovers whether or not the current device has reported its transducer type information
     *
     * @return SUCCESS if the query succeeded, FEATURE_UNSUPPORTED if the device does not support the query,
               or DISCONNECTED if the device is unavailable */
    ULH_RESEARCH_API bool hasDeviceReportedTransducerTypeInfo() const;

    /** Get the type of transducer on the transducer board connected to this device
     *
     * @return SUCCESS if the query succeeded, FEATURE_UNSUPPORTED if the device does not support the query,
               or DISCONNECTED if the device is unavailable */
    ULH_RESEARCH_API result<TransducerType> getTransducerType() const;

    /** Get the currently available transducer model for this device
     *
     * @return SUCCESS if the query succeeded, FEATURE_UNSUPPORTED if the device does not support the query,
               or DISCONNECTED if the device is unavailable */
    ULH_RESEARCH_API result<TransducerModel> getSupportedTransducerModel() const;

    /** Get a string representing the specified transducer type
     *
     * @param type the transducer type to get the name of
     * @return a static string containing the name */
    ULH_RESEARCH_API static const char* getTransducerTypeName(TransducerType type);

    /** Get a string representing the specified transducer model
     *
     * @param model the transducer model to get the name of
     * @return a static string containing the name */
    ULH_RESEARCH_API static const char* getTransducerModelName(TransducerModel model);

    /*********** Transducers State updates At Time Points ************/

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param transducers_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param time_control_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
         or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2transducersStateUpdatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const TransducersState* transducers_states,
        size_t states_count,
        LocalTimePoint time_control_left_user);

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param local_time_points an array of host-side time points to emit the associated states at
     * @param transducers_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
               or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2transducersStateUpdatesAtTimePoints(
        const LocalTimePoint* local_time_points,
        const TransducersState* transducers_states,
        size_t states_count);

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param device_time_points an array of device-side time points to emit the associated states at
     * @param transducers_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @param time_control_left_user the time at which control returned from the user to the SDK
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
               or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2transducersStateUpdatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const TransducersState* transducers_states,
        size_t states_count,
        DeviceTimePoint time_control_left_user);

    /** Send a set of transducer state objects with associated emission times to the device for emission
     *
     * @param device_time_points an array of device-side time points to emit the associated states at
     * @param transducers_states an array of states containing sets of transducer activations
     * @param states_count the number of entries in the time points and states arrays
     * @return SUCCESS if the message was queued for sending, FAILED if the message could not be sent,
               or FEATURE_UNSUPPORTED if the device does not support this mode of operation */
    ULH_RESEARCH_API result<void> v2transducersStateUpdatesAtTimePoints(
        const DeviceTimePoint* device_time_points,
        const TransducersState* transducers_states,
        size_t states_count);

    /** Gets the maximum update rate supported by this device for transducer state updates
     *
     * @return SUCCESS if the rate limit was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getUpdateRateLimitForV2TransducersStatesAtTimePoints() const;

    /** @return true if the device supports this mode of operation, or false if not */
    ULH_RESEARCH_API bool hasSupportForV2TransducersStateUpdateAtTimePoints() const;

    /*********** Dynamic Range Control ************/

    /** Determines whether dynamic range is currently enabled on this device.
     *
     * @return SUCCESS if the dynamic range status was queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<bool> isDynamicRangeEnabled() const;

    /** Enables or disables dynamic range on this device.
     *
     * @param is_enabled whether to enable or disable dynamic range
     * @return SUCCESS if the dynamic range status was set, DISCONNECTED if the device was unavailable,
     *         FAILED if the device did not accept the change, or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<void> setDynamicRangeEnabled(bool is_enabled);

    /** Determines whether this device supports dynamic range query and modification
     *
     * @return SUCCESS if dynamic range control is supported, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this mechanism */
    ULH_RESEARCH_API bool hasSupportForDynamicRangeControl() const;

    /*********** Filter Control ************/

    /** Get filter upper cut-off frequencies on this device.
     *
     * @param frequencyX the cut-off frequency to filter the X coordinate, or zero if disabled
     * @param frequencyY the cut-off frequency to filter the Y coordinate, or zero if disabled
     * @param frequencyZ the cut-off frequency to filter the Z coordinate, or zero if disabled
     * @param frequencyA the cut-off frequency to filter the intensity, or zero if disabled
     * @return SUCCESS if the filters were queried, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<void> getFilterFrequencies(float& frequencyX, float& frequencyY, float& frequencyZ, float& frequencyA) const;

    /** Set filter upper cut-off frequencies and filter enable/disable on this device.
     *
     * @param frequencyX the cut-off frequency to filter the X coordinate, but if the frequency given is zero or less, the filter is disabled
     * @param frequencyY the cut-off frequency to filter the Y coordinate, but if the frequency given is zero or less, the filter is disabled
     * @param frequencyZ the cut-off frequency to filter the Z coordinate, but if the frequency given is zero or less, the filter is disabled
     * @param frequencyA the cut-off frequency to filter the amplitude, but if the frequency given is zero or less, the filter is disabled
     * @return SUCCESS if the filters were set, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<void> setFilterFrequencies(float frequencyX, float frequencyY, float frequencyZ, float frequencyA);

    /** Determines whether this device supports filter queries and modification
     *
     * @return SUCCESS if filter control is supported, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this mechanism */
    ULH_RESEARCH_API bool hasSupportForFilterControl() const;

    /*********** Repeat Generator Period Control ************/

    /** Queries the current repeat generator period on this device.
     *
     * @return the current repeat generator period of this device in device ticks, or an error */
    ULH_RESEARCH_API result<DeviceDuration> getRepeatGeneratorPeriod() const;

    /** Set the repeat generator period on this device.
     *
     * @param period the repeat generator period in device ticks to set
     *
     * @return void if the repeat generator period was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setRepeatGeneratorPeriod(DeviceDuration period);

    /** Determines whether this device supports getting and setting the repeat generator period
     *
     * @return true if this device supports repeat generator period control, false if not */
    ULH_RESEARCH_API bool hasSupportForRepeatGeneratorPeriodControl() const;

    /*********** Sustain Counter Period Control ************/

    /** Queries the current sustain counter period on this device.
     *
     * @return the current sustain counter period of this device in device ticks, or an error */
    ULH_RESEARCH_API result<DeviceDuration> getSustainCounterPeriod() const;

    /** Set the sustain counter period on this device.
     *
     * @param period the sustain counter period in device ticks to set
     *
     * @return void if the sustain counter period was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setSustainCounterPeriod(DeviceDuration period);

    /** Determines whether this device supports getting and setting the sustain counter period
     *
     * @return true if this device supports sustain counter period control, false if not */
    ULH_RESEARCH_API bool hasSupportForSustainCounterPeriodControl() const;

    /*********** Timestamp Cycle Offset Control ************/

    /** Queries the current timestamp cycle offset on a given channel.
     *
     * @param channel the channel for which to query the timestamp cycle offset
     *
     * @return the current timestamp cycle offset of a given channel, or an error */
    ULH_RESEARCH_API result<size_t> getTimestampCycleOffset(size_t channel) const;

    /** Set the timestamp cycle offset for a given channel.
     *
     * @param offset the number of timestamps to go back when recycling states
     *
     * @param channel the channel for which to set the timestamp cycle offset
     *
     * @return void if the timestamp cycle offset was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setTimestampCycleOffset(size_t offset, size_t channel);

    /** Determines whether this device supports getting and setting timestamp cycle offsets
     *
     * @return true if this device supports timestamp cycle offset control, false if not */
    ULH_RESEARCH_API bool hasSupportForTimestampCycleOffsetControl() const;

    /*********** Point Cycle Offset Control ************/

    /** Queries the current point cycle offset on a given channel.
     *
     * @param channel the channel for which to query the point cycle offset
     *
     * @return the current point cycle offset of a given channel, or an error */
    ULH_RESEARCH_API result<size_t> getPointCycleOffset(size_t channel) const;

    /** Set the point cycle offset for a given channel.
     *
     * @param offset the number of states to go back when recycling states
     *
     * @param channel the channel for which to set the point cycle offset
     *
     * @return void if the point cycle offset was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setPointCycleOffset(size_t offset, size_t channel);

    /** Determines whether this device supports getting and setting point cycle offsets
     *
     * @return true if this device supports point cycle offset control, false if not */
    ULH_RESEARCH_API bool hasSupportForPointCycleOffsetControl() const;

    /*********** Sample And Hold Control ************/

    /** Determines whether sample and hold is currently enabled on this device.
     *
     * @return true if sample and hold is enabled, false if not or an error */
    ULH_RESEARCH_API result<bool> isSampleAndHoldEnabled() const;

    /** Enables or disables dynamic range on this device.
     *
     * @param is_enabled whether to enable or disable sample and hold
     *
     * @return void if sample and hold was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setSampleAndHoldEnabled(bool is_enabled);

    /** Determines whether this device supports sample and hold query and modification
     *
     * @return true if this device supports sample and hold, false if not */
    ULH_RESEARCH_API bool hasSupportForSampleAndHoldControl() const;

    /*********** State Generator Period Control ************/

    /** Queries the current state generator period on this device.
     *
     * @return the current state generator period of this device in device ticks, or an error */
    ULH_RESEARCH_API result<DeviceDuration> getStateGeneratorPeriod() const;

    /** Queries the current state generator period on this device.
     *
     * @param period the state generator period in device ticks to set
     *
     * @return void if the state generator period was set correctly or an error if not */
    ULH_RESEARCH_API result<void> setStateGeneratorPeriod(DeviceDuration period);

    /** Determines whether this device supports getting and setting the state generator period
     *
     * @return true if this device supports state generator period control, false if not */
    ULH_RESEARCH_API bool hasSupportForStateGeneratorPeriodControl() const;

private:
    ULH_RESEARCH_API result<void> _v4updateControlPointConfiguration(uh::vector_wrapper<v4::ControlPointConfiguration> configs) const;

    /*********** Waveform Tables ************/
    ULH_RESEARCH_API result<uh::vector_wrapper<uint16_t>> _getWaveformTable(uint16_t table_select) const;
    ULH_RESEARCH_API result<void> _setWaveformTable(uint16_t table_select, uh::vector_wrapper<uint16_t> data);

public:
    /** Gets the data held by the waveform table at a given index
     *
     * @param table_select the index of the table to get
     *
     * @return the waveform table at the index specified or an error */
    inline result<std::vector<uint16_t>> getWaveformTable(uint16_t table_select) const
    {
        auto result = _getWaveformTable(table_select);
        if (result)
            return result.value();
        else
            return make_unexpected(result.error());
    }

    /** Sets the data for the waveform table at a given index
     *
     * @param table_select the index of the table to set
     *
     * @param data the waveform table data to set
     *
     * @return void or an error */
    inline result<void> setWaveformTable(uint16_t table_select, std::vector<uint16_t> data)
    {
        return _setWaveformTable(table_select, std::move(data));
    }

    /** Gets the index of the currently active waveform table
     *
     * @return the index of the currently active waveform table or an error */
    ULH_RESEARCH_API result<uint16_t> getActiveWaveformTable() const;

    /** Sets the index of the currently active waveform table
     *
     * @param table_select the index of the table to make activeh
     *
     * @return void or an error */
    ULH_RESEARCH_API result<void> setActiveWaveformTable(uint16_t table_select);

    /** Determines whether this device supports getting and setting waveform tables
     *
     * @return true if this device supports waveform tables, alse if not */
    ULH_RESEARCH_API bool hasSupportForWaveformTables() const;

    /*********** Sensor Querying ************/

    /** Attempts to query the value of a sensor of a specified type
     *
     * @param type the type of sensor to be queried
     * @return SUCCESS if the value was successfully queried, FAILED if no suitable sensor could be found,
     *         DISCONNECTED if the device was unavailable or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<SensorDetails> getSensorValue(SensorType type) const;

    /** Attempts to query the value of a sensor of a specified type and location
     *
     * @param type the type of sensor to be queried
     * @param loc the required location of the sensor to be queried
     * @return SUCCESS if the value was successfully queried, FAILED if no suitable sensor could be found,
     *         DISCONNECTED if the device was unavailable or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<SensorDetails> getSensorValue(SensorType type, SensorLocation loc) const;

    /** Attempts to query the value of a sensor of a specified type and specific location
     *
     * @param type the type of sensor to be queried
     * @param loc the required location of the sensor to be queried
     * @param sloc the required extended location of the sensor to be queried
     * @return SUCCESS if the value was successfully queried, FAILED if no suitable sensor could be found,
     *         DISCONNECTED if the device was unavailable or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<SensorDetails> getSensorValue(SensorType type, SensorLocation loc, SensorSubLocation sloc) const;

    /** Query the value of a sensor on the device by index
     *
     * Note that sensor indexes begin from 1; valid indexes are from 1 to sensor_count,
     * rather than 0 to sensor_count-1.
     *
     * @param index the 1-indexed number of the sensor to query
     * @return SUCCESS if the query succeeded, FAILED if the index was not valid,
     *         DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<SensorDetails> getSensorValue(size_t index) const;

    /** Query the number of sensors present on the device
     *
     * @return SUCCESS if the count was populated, DISCONNECTED if the device was unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support this query */
    ULH_RESEARCH_API result<size_t> getSensorCount() const;

    /** @return SUCCESS if sensor querying is supported, DISCONNECTED if the device was unavailable,
     *          or FEATURE_UNSUPPORTED if the device has no sensors or does not support querying them */
    ULH_RESEARCH_API bool hasSupportForQueryingSensors() const;

    /*********** Device Status ************/

    ULH_RESEARCH_API bool hasSupportForDeviceStatus() const;

    // ULH_RESEARCH_API Status getDeviceStatus(DeviceStatus& status) const;

    ULH_RESEARCH_API bool isReadyToEmit() const;

    /*********** Query Error API ************/

    ULH_RESEARCH_API bool hasSupportForQueryingErrors() const;

    ULH_RESEARCH_API result<uint32_t> getDeviceErrorVersion() const;

    ULH_RESEARCH_API result<uint32_t> getDeviceActiveErrorCount() const;

    ULH_RESEARCH_API result<uint32_t> getDeviceTotalErrorCount() const;

    ULH_RESEARCH_API result<uint32_t> getActiveErrorBitfield() const;

    /*********** Device Trigger ************/

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param end_time the device time at which the trigger signal should end
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignal(const DeviceTimePoint& time, const DeviceTimePoint& end_time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param end_time the device time at which the trigger signal should end
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignal(const LocalTimePoint& time, const DeviceTimePoint& end_time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param end_time the host time at which the trigger signal should end
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignal(const DeviceTimePoint& time, const LocalTimePoint& end_time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param end_time the host time at which the trigger signal should end
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignal(const LocalTimePoint& time, const LocalTimePoint& end_time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time by rising
     *
     * @param time the host time at which the device should rise
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignalRise(const LocalTimePoint& time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time by falling
     *
     * @param time the host time at which the device should fall
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignalFall(const LocalTimePoint& time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time by rising
     *
     * @param time the device time at which the device should rise
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignalRise(const DeviceTimePoint& time);

    /** Queue a message to the device to send a v4 trigger signal to be executed at the specified time by falling
     *
     * @param time the device time at which the device should fall
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV4TriggerSignalFall(const DeviceTimePoint& time);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the host time at which the device should fire the trigger signal
     * @param data the data to send with the trigger signal, or nullptr if no data is required
     * @param data_len the length of the data to send; must be 0 if data is nullptr
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV2TriggerSignal(const LocalTimePoint& time, const void* data = nullptr, size_t data_len = 0);

    /** Queue a message to the device to send a trigger signal to be executed at the specified time
     *
     * @param time the device time at which the device should fire the trigger signal
     * @param data the data to send with the trigger signal, or nullptr if no data is required
     * @param data_len the length of the data to send; must be 0 if data is nullptr
     * @return SUCCESS if the message was queued, DISCONNECTED if the device is unavailable,
     *         or FEATURE_UNSUPPORTED if the device does not support trigger signals */
    ULH_RESEARCH_API result<void> sendV2TriggerSignal(const DeviceTimePoint& time, const void* data = nullptr, size_t data_len = 0);

    ULH_RESEARCH_API void setDataDumpEnabled(bool enabled) const;
    ULH_RESEARCH_API void setDataDumpEnabled(DataDumpCategory category, bool enabled) const;
    ULH_RESEARCH_API void setDataDumpFlushEveryXWrites(DataDumpCategory category, int flush_every_x_writes) const;
    ULH_RESEARCH_API bool getDataDumpEnabled() const;
    ULH_RESEARCH_API bool getDataDumpEnabled(DataDumpCategory category) const;
    ULH_RESEARCH_API int getDataDumpFlushEveryXWrites(DataDumpCategory category) const;

    ULH_RESEARCH_API void setPhaseOnlyTransducersEnabled(bool enabled) const;

    /** Send a get control transfer request with specific values, the request value is equal to DEVICE_EXPERIMENTAL_CT (0xF0)
     *
     * @param wValue the request value
     * @param wIndex the request index
     * @return Requested data, if it was received, zero length string otherwise.
     */
    ULH_RESEARCH_API std::string getExperimentalControlTransfer(const uint16_t wValue, const uint16_t wIndex) const;

    /** Send a control transfer with specific values, the request value is equal to DEVICE_EXPERIMENTAL_CT (0xF0)
     *
     * @param wValue the request value
     * @param wIndex the index value
     * @param data the actual data sent with the control transfer
     * @return SUCCESS if the control transfer was sent successfully, DISCONNECTED if the device is unavailable,
     *          or COMMUNICATION_ERROR if the control transfer was unsuccessful. */
    ULH_RESEARCH_API result<void> sendExperimentalControlTransfer(const uint16_t wValue, const uint16_t wIndex, const std::string& data) const;

    /** \cond EXCLUDE_FROM_DOCS */
    class Implementation;
    Implementation* getImplementation();
    const Implementation* getImplementation() const;
    /** \endcond */
private:
    /** \cond EXCLUDE_FROM_DOCS */
    Implementation* impl;
    /** \endcond */

    /** Private constructor called internally. */
    DriverDevice(Implementation* iimpl);
};
} // namespace Haptics

} // namespace Ultraleap
