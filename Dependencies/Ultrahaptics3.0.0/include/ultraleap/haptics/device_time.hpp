#pragma once

#include <cstdint>

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/driver.hpp"

namespace Ultraleap
{
namespace Haptics
{
typedef uint32_t timestamp32_t;
typedef int32_t timediff32_t;

/** Represents a duration of time, in clock cycles on the device of Global Tick Counter.
 * This is a counter that is linked to the ultrasound cycles (40 kHz) by some power of 2 (e.g. 256).
 *
 * That factor is TICKS_PER_CYCLE in the firmware.
 *
 * The duration is signed so it can be negative.
 *
 * \brief Time duration class. */
class ULH_RESEARCH_API_CLASS DeviceDuration
{
    friend class DeviceTimePoint;
    friend class DeviceClock;
    friend class DeviceLatency;

public:
    using rep = timediff32_t;

    /** Create a duration of 0 seconds. */
    ULH_RESEARCH_API DeviceDuration();

    /** Constructor for device time difference. */
    ULH_RESEARCH_API DeviceDuration(rep itimer_difference);

    /** @return The internal representation counter for this value. */
    ULH_RESEARCH_API rep count() const;

    /** @return The internal representation counter for this value. */
    ULH_RESEARCH_API rep value() const;

    /** Overload of addition operator.
     *
     * @param rhs The right hand side duration in the addition
     *
     * @return The sum of the two durations */
    ULH_RESEARCH_API DeviceDuration operator+(const DeviceDuration& rhs) const;

    /** Overload of add-accumulate operator.
     *
     * @param rhs The right hand side duration to add to this
     *
     * @return The updated sum */
    ULH_RESEARCH_API DeviceDuration& operator+=(const DeviceDuration& rhs);

    /** Overload of subtraction operator.
     *
     * @param rhs The right hand side duration in the addition
     *
     * @return The sum of the two durations */
    ULH_RESEARCH_API DeviceDuration operator-(const DeviceDuration& rhs) const;

    /** Overload of subtract-accumulate operator.
     *
     * @param rhs The right hand side duration to subtract from this
     *
     * @return The updated subtraction */
    ULH_RESEARCH_API DeviceDuration& operator-=(const DeviceDuration& rhs);

    /** Overload of multiply accumulate.
     *
     * @param rhs The scaling factor for the duration
     *
     * @return The new scaled duration */
    ULH_RESEARCH_API DeviceDuration& operator*=(const uint32_t rhs);

    /** Overload of multiplication.
     *
     * @param rhs The scaling factor for the duration
     *
     * @return The new scaled duration */
    ULH_RESEARCH_API DeviceDuration operator*(const uint32_t rhs) const;

    /** Overload of divide accumulate.
     *
     * @param rhs The divisor factor for the duration
     *
     * @return The new divided duration */
    ULH_RESEARCH_API DeviceDuration& operator/=(const uint32_t rhs);

    /** Overload of division.
     *
     * @param rhs The divisor factor for the duration
     *
     * @return The new divided duration */
    ULH_RESEARCH_API DeviceDuration operator/(const uint32_t rhs) const;

    /** Overload of division.
     *
     * @param rhs The duration to divide by
     *
     * @return The divisor scale */
    ULH_RESEARCH_API float operator/(const DeviceDuration& rhs) const;

    /** Overload of less than operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is less than the other */
    ULH_RESEARCH_API bool operator<(const DeviceDuration& rhs) const;

    /** Overload of less than or equal to operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is less than or equal to the other */
    ULH_RESEARCH_API bool operator<=(const DeviceDuration& rhs) const;

    /** Overload of greater than operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is greater than the other */
    ULH_RESEARCH_API bool operator>(const DeviceDuration& rhs) const;

    /** Overload of greater than or equal to operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is greater than or equal to the other */
    ULH_RESEARCH_API bool operator>=(const DeviceDuration& rhs) const;

    /** Overload of equal to operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is equal to the other */
    ULH_RESEARCH_API bool operator==(const DeviceDuration& rhs) const;

    /** Overload of not equal to operator for duration comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this value is not equal to the other */
    ULH_RESEARCH_API bool operator!=(const DeviceDuration& rhs) const;

protected:
    /** 32-bit device specific timestamp difference. */
    rep timer_difference;
};

/** Represents a point in time, in clock cycles on the device of Global Tick Counter.
 * This is a counter that is linked to the ultrasound cycles (40 kHz) by some power of 2 (e.g. 256).
 *
 * That factor is TICKS_PER_CYCLE in the firmware.
 *
 * \brief Time point class. */
class ULH_RESEARCH_API_CLASS DeviceTimePoint
{
    friend class DeviceClock;
    friend class DriverDevice;

public:
    using duration = DeviceDuration;
    using rep = timestamp32_t;

    /** Construct zero device time. */
    ULH_RESEARCH_API DeviceTimePoint();

    /** Construct device time from value */
    ULH_RESEARCH_API DeviceTimePoint(rep itimer_count);

    /** @return the internal timestamp representation of this time point */
    ULH_RESEARCH_API rep value() const;

    /** Overload of addition operator.
     *
     * @param rhs The right hand side duration in the addition
     *
     * @return The point in time with the duration added */
    ULH_RESEARCH_API DeviceTimePoint operator+(const DeviceDuration& rhs) const;

    /** Overload of add-accumulate operator.
     *
     * @param rhs The right hand side duration to add to this
     *
     * @return This point in time with the duration added */
    ULH_RESEARCH_API DeviceTimePoint& operator+=(const DeviceDuration& rhs);

    /** Overload of subtraction operator.
     *
     * @param rhs The right hand side point in time to subtract
     *
     * @return The duration between this point in time or the other point in time */
    ULH_RESEARCH_API DeviceDuration operator-(const DeviceTimePoint& rhs) const;

    /** Overload of subtraction operator.
     *
     * @param rhs The right hand side duration in the subtraction
     *
     * @return The point in time with the duration subtracted */
    ULH_RESEARCH_API DeviceTimePoint operator-(const DeviceDuration& rhs) const;

    /** Overload of subtract-accumulate operator.
     *
     * @param rhs The right hand side duration to subtract from this
     *
     * @return This point in time with the duration subtracted */
    ULH_RESEARCH_API DeviceTimePoint& operator-=(const DeviceDuration& rhs);

    /** Overload of less than operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time is earlier than the other */
    ULH_RESEARCH_API bool operator<(const DeviceTimePoint& rhs) const;

    /** Overload of less than or equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time occurs earlier than or at the same time as the other */
    ULH_RESEARCH_API bool operator<=(const DeviceTimePoint& rhs) const;

    /** Overload of greater than operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time comes later than the other */
    ULH_RESEARCH_API bool operator>(const DeviceTimePoint& rhs) const;

    /** Overload of greater than or equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time is the same or comes later than the other */
    ULH_RESEARCH_API bool operator>=(const DeviceTimePoint& rhs) const;

    /** Overload of equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if the two points in time are the same */
    ULH_RESEARCH_API bool operator==(const DeviceTimePoint& rhs) const;

    /** Overload of not equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if the two points in time are not the same */
    ULH_RESEARCH_API bool operator!=(const DeviceTimePoint& rhs) const;

protected:
    /** 32-bit device specific timestamp. */
    rep timer_count;
};
} // namespace Haptics
} // namespace Ultraleap
