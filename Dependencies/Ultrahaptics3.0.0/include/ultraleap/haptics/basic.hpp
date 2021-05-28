#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/control_point.hpp"
#include "ultraleap/haptics/emitter.hpp"
#include "ultraleap/haptics/device_info.hpp"
#include "ultraleap/haptics/local_time.hpp"
#include "ultraleap/haptics/transform.hpp"
#include "ultraleap/haptics/library.hpp"

#include <cstdint>

namespace Ultraleap
{
namespace Haptics
{

/** \brief Emit haptic points using automatic sine-wave amplitude modulation.
 * \rst
.. warning::
    This class is deprecated and will be removed in a future release.
    Amplitude modulation can be achieved using Ultraleap::Haptics::SensationEmitter
    and loading an amplitude modulation sensation.
\endrst
 */
class ULH_API_CLASS BasicEmitter : public Emitter
{
public:
    /** Constructs an amplitude modulation emitter.
     *
     * @see Library.
     *
     * This class is movable.
     *
     * @param ulh_lib an Library object. */
    ULH_API explicit BasicEmitter(Library ulh_lib);

    /** \cond EXCLUDE_FROM_DOCS */
    /** Move constructor */
    ULH_API BasicEmitter(BasicEmitter&& other);
    /** Move assignment operator */
    ULH_API BasicEmitter& operator=(BasicEmitter&& other);
    /** Deleted copy constructor. */
    ULH_API BasicEmitter(const BasicEmitter& other) = delete;
    /** Deleted copy assignment operator. */
    ULH_API BasicEmitter& operator=(const BasicEmitter& other) = delete;
    /** \endcond */

    /** Destroys the emitter and all its resources */
    ULH_API virtual ~BasicEmitter();

    /** Send a list of control points to be outputted.
     *
     * Sends a list of amplitude modulated control points at a given frequency,
     * which will be outputted until update is called again or stop is called.
     *
     * @param controlpoint a pointer to an array of control points.
     * @see ControlPoint
     *
     * @param controlpointcount number of control points in the array.
     *
     * @param frequency the frequency in Hertz at which the
     * amplitude of the control point(s) will be modulated. */
    ULH_API result<void> update(const ControlPoint* controlpoint, const size_t controlpointcount, const float frequency);

    /** Send a control point to be outputted.
     *
     * Sends an amplitude modulated control point at a given frequency,
     * which will be outputted until update is called again or stop is called.
     *
     * @param controlpoint a ControlPoint reference.
     * @see ControlPoint
     *
     * @param frequency the frequency in Hertz at which the
     * amplitude of the control point will be modulated. */
    ULH_API result<void> update(const ControlPoint& controlpoint, const float frequency);

    /** Set a geometric transform for a given device.
     * @see Emitter */
    ULH_API result<void> setDeviceTransform(const Device& device, Transform transform, ReEmitPoints reemit = ReEmitPoints::Yes);

    /** Set a geometric transform for the device at a given index.
     * @see Emitter */
    ULH_API result<void> setDeviceTransform(size_t device_index, Transform transform, ReEmitPoints reemit = ReEmitPoints::Yes);
};
} // namespace Haptics
} // namespace Ultraleap
