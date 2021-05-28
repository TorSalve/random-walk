#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/states.hpp"

#include <cstdlib>

namespace Ultraleap
{
namespace Haptics
{

/** An enumerated type for each of the firmwares capabilities. */
enum DeviceCaps {
    /** The device has unknown and any capabilities. */
    DEVICE_CAPABILITY_ANY = 0,
    /** The device supports V2 focus point updates with timestamps */
    DEVICE_CAPABILITY_V2_FOCUS_POINT_STATE_WITH_TIME_POINT = (1 << 0),
    /** The device supports the amplitude modulation API. This is true if any of these
     * backend methods are supported (in order of preference):
     *
     *   DEVICE_CAPABILITY_CONTROL_POINT_STATE
     *   DEVICE_CAPABILITY_CYCLING_TRANSDUCER_STATE
     *   DEVICE_CAPABILITY_CONTROL_POINT_STATE_STREAMING
     *   DEVICE_CAPABILITY_V4_CONTROL_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_CONTROL_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_FOCUS_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V2_FOCUS_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_TRANSDUCERS_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V2_TRANSDUCERS_STATE_WITH_TIME_POINT
     */
    DEVICE_CAPABILITY_AMPLITUDE_MODULATION = (1 << 1),
    /** The device supports the time point streaming API. This is true if any of these
     * backend methods are supported (in order of preference):
     *
     *   DEVICE_CAPABILITY_CONTROL_POINT_STATE_STREAMING
     *   DEVICE_CAPABILITY_V4_CONTROL_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_CONTROL_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_FOCUS_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V2_FOCUS_POINT_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V3_TRANSDUCERS_STATE_WITH_TIME_POINT
     *   DEVICE_CAPABILITY_V2_TRANSDUCERS_STATE_WITH_TIME_POINT
     */
    DEVICE_CAPABILITY_TIMEPOINT_STREAMING = (1 << 3),
    /** The device supports V2 transducers state updates with timestamps */
    DEVICE_CAPABILITY_V2_TRANSDUCERS_STATE_WITH_TIME_POINT = (1 << 4),
    /** The device supports the hard reset command */
    DEVICE_CAPABILITY_HARD_RESET = (1 << 5),
    /** The device supports directly sending control point states with sine modulation.
     * (as in the AM API). */
    DEVICE_CAPABILITY_CONTROL_POINT_STATE = (1 << 6),
    /** The device supports receiving a heartbeat packet */
    DEVICE_CAPABILITY_HEARTBEAT = (1 << 8),
    /** The device supports being sent a timed V2 trigger signal */
    DEVICE_CAPABILITY_V2_TRIGGER_SIGNAL = (1 << 9),
    /** The device supports master/slave mode */
    DEVICE_CAPABILITY_MASTER_SLAVE = (1 << 10),
    /** The device supports cycling transducer states - sine modulation but where
     * the transducer coefficients are calculated on the host instead of the device. */
    DEVICE_CAPABILITY_CYCLING_TRANSDUCER_STATE = (1 << 11),
    /** The device supports sending control point states as a stream. */
    DEVICE_CAPABILITY_CONTROL_POINT_STATE_STREAMING = (1 << 12),
    /** The device supports having its output config reset to defaults */
    DEVICE_CAPABILITY_OUTPUT_CONFIG_RESET = (1 << 13),
    /** The device supports V3 control point updates with timestamps */
    DEVICE_CAPABILITY_V3_CONTROL_POINT_STATE_WITH_TIME_POINT = (1 << 14),
    /** The device supports V3 focus point updates with timestamps */
    DEVICE_CAPABILITY_V3_FOCUS_POINT_STATE_WITH_TIME_POINT = (1 << 15),
    /** The device supports V3 transducers state updates with timestamps */
    DEVICE_CAPABILITY_V3_TRANSDUCERS_STATE_WITH_TIME_POINT = (1 << 16),
    /** The device supports V3 uniform transducer state updates with timestamps */
    DEVICE_CAPABILITY_V3_COMMON_TRANSDUCER_STATE_WITH_TIME_POINT = (1 << 17),

    /** The device supports V4 control point configuration*/
    DEVICE_CAPABILITY_V4_CONTROL_POINT_CONFIGURATION = (1 << 18),
    /** The device supports V4 control point updates with timestamps */
    DEVICE_CAPABILITY_V4_CONTROL_POINT_STATE_WITH_TIME_POINT = (1 << 19),
    DEVICE_CAPABILITY_V4_TRANSDUCERS_STATE_WITH_TIME_POINT = (1 << 20),
    DEVICE_CAPABILITY_V4_COMMON_TRANSDUCER_STATE_WITH_TIME_POINT = (1 << 21),
    //DEVICE_CAPABILITY_V4_FOCUS_POINT_STATE_WITH_TIME_POINT = (1 << 22),
    /** The device supports being sent a V4 timed trigger signal */
    DEVICE_CAPABILITY_V4_TRIGGER_SIGNAL = (1 << 23),
};

} // namespace Haptics
} // namespace Ultraleap
