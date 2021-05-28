#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/transducers.hpp"

namespace Ultraleap
{
namespace Haptics
{
/** \cond EXCLUDE_FROM_DOCS */
class DeviceInfoImplementation;
/** \endcond */

/** Flags indicating features which devices may support */
enum class DeviceFeatures : unsigned int {
    None = 0,
    BasicHaptics = (1 << 0),
    StreamingHaptics = (1 << 1)
};

/** \cond EXCLUDE_FROM_DOCS */
/** Bitwise "or" operator for device features enum */
ULH_API DeviceFeatures operator|(DeviceFeatures lhs, DeviceFeatures rhs);
/** Bitwise "and" operator for device features enum */
ULH_API DeviceFeatures operator&(DeviceFeatures lhs, DeviceFeatures rhs);
/** Bitwise "xor" operator for device features enum */
ULH_API DeviceFeatures operator^(DeviceFeatures lhs, DeviceFeatures rhs);
/** Bitwise "not" operator for device features enum */
ULH_API DeviceFeatures operator~(DeviceFeatures lhs);
/** Bitwise "or" assignment operator for device features enum */
ULH_API DeviceFeatures& operator|=(DeviceFeatures& lhs, DeviceFeatures rhs);
/** Bitwise "and" assignment operator for device features enum */
ULH_API DeviceFeatures& operator&=(DeviceFeatures& lhs, DeviceFeatures rhs);
/** Bitwise "xor" assignment operator for device features enum */
ULH_API DeviceFeatures& operator^=(DeviceFeatures& lhs, DeviceFeatures rhs);
/** \endcond */

/** This represents information about a device at the time it was queried.
   *
   * No persistent link to the device is maintained via this class, so there is no guarantee
   * that the device has not been disconnected or altered since the information was gathered.
   *
   * This class is copyable and movable.
   *
   * A DeviceInfo object can be obtain from a Device object. */
class ULH_API_CLASS DeviceInfo
{
    friend class DeviceBackend;

public:
    /** \cond EXCLUDE_FROM_DOCS */
    /** Copy constructor */
    ULH_API DeviceInfo(const DeviceInfo& other);
    /** Copy assignment operator */
    ULH_API DeviceInfo& operator=(const DeviceInfo& other);
    /** Move constructor */
    ULH_API DeviceInfo(DeviceInfo&& other);
    /** Move assignment operator */
    ULH_API DeviceInfo& operator=(DeviceInfo&& other);
    /** \endcond */
    /** Destroys this device info object and all its resources */
    ULH_API ~DeviceInfo();

    /** Gets the device identifier for this device e.g. 'USX:00000001'.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return the full identifier for this device. */
    ULH_API const char* getIdentifier() const;

    /** Gets the model name of this device e.g. 'USX'.
     *
     * @return the model name for this device. */
    ULH_API const char* getModelName() const;
    /** Gets the model description of this device.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return a description of the model type of this device. */
    ULH_API const char* getModelDescription() const;
    /** Gets the serial number of this device e.g. '00000001'.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return the serial number of this device. */
    ULH_API const char* getSerialNumber() const;
    /** Gets the name of the transducer layout of this device.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return the name of the transducer layout of this device. */
    ULH_API const char* getTransducerLayoutName() const;
    /** Gets the firmware version of this device.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return the firmware verison of this device. */
    ULH_API const char* getFirmwareVersion() const;
    /** Gets the date on which the firmware of this device was built.
     *
     * Please note that this returns a pointer to a string held within the DeviceInfo object.
     *
     * @return the date on which the firmware of this device was built*/
    ULH_API const char* getFirmwareBuildDate() const;

    /** Get the transducer configuration of this device.
     *
     * Gets a TransducerContainer containing the layout
     * and other information regarding the transducers of this device.
     *
     * @return a TransducerContainer describing the transducers of this device. */
    ULH_API TransducerContainer getTransducers() const;

    /** @return The number of transducers making up this device */
    ULH_API size_t getTransducerCount() const;

    /** @return The nominal operating frequency of transducers in this device, in Hz */
    ULH_API float getTransducerFrequency() const;

    /** @return The minimum distance this device requires two control points to be apart, in metres */
    ULH_API float getMinimumPointSeparation() const;

    /** Get a bitfield representing the set of features supported by this device.
     *
     * Note that the value returned is a set of flags (a bitfield), so it
     * may not match any individual value of the DeviceFeatures enumeration.
     *
     * @see DeviceFeatures
     *
     * @return The set of features supported by this device */
    ULH_API DeviceFeatures getSupportedFeatures() const;

    /** Checks whether this device supports a specific feature set
     *
     * The argument may be a single feature, or a combination
     * of features specified by using the bitwise OR operator.
     *
     * @see DeviceFeatures
     *
     * @param features The feature(s) to check for support for
     *
     * @return True if the device supports this feature set, false if not */
    ULH_API bool hasSupportFor(DeviceFeatures features) const;

private:
    /** \cond EXCLUDE_FROM_DOCS */
    DeviceInfo(DeviceInfoImplementation* impl);
    DeviceInfoImplementation* impl;
    /** \endcond */
};
} // namespace Haptics
} // namespace Ultraleap
