#pragma once

#include <cmath>
#include <cstdint>
#include <chrono>
#include "ultraleap/haptics/common.hpp"

namespace Ultraleap
{
namespace Haptics
{
/** \brief A period of time. The difference between two LocalTimePoint%s. */
using LocalDuration = std::chrono::duration<int64_t, std::nano>;

/** \brief A clock. */
using LocalTimeClock = std::chrono::steady_clock;

/** \brief A point in time. */
using LocalTimePoint = std::chrono::time_point<LocalTimeClock, LocalDuration>;

/** Convert a LocalDuration to a number of seconds.
   *
   * @param duration The LocalDuration to be converted.
   *
   * @return The duration in seconds. */
inline double duration_to_sec(LocalDuration duration)
{
    return std::chrono::duration<double>(duration).count();
}

/** Convert a number of seconds to a LocalDuration.
   *
   * @param sec The number of seconds to be converted.
   *
   * @return The number of seconds as a LocalDuration. */
inline LocalDuration duration_from_sec(double sec)
{
    return std::chrono::duration_cast<LocalDuration>(std::chrono::duration<double>(sec));
}
} // namespace Haptics

} // namespace Ultraleap
