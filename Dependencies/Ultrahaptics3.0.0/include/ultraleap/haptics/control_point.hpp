
#pragma once

#include <cstring>
#include <cmath>
#include "ultraleap/haptics/vector3.hpp"
#include "ultraleap/haptics/common.hpp"

namespace Ultraleap
{
namespace Haptics
{
/** \brief Point at which the air pressure is controlled, with position and intensity.
 *
 * A control point is simply a point in space at which you can specify
 * the desired SPL (Sound Pressure Level) modulation. In almost all cases
 * the control point will coincide with the focal point of the ultrasound.
 *
 * The intensity is given in arbitrary units varying from 0 to 1.
 */
struct ControlPoint
{
    /** Control point constructor. */
    ControlPoint()
        : ControlPoint({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 0)
    {}

    /** Raw control point constructor with x, y, z, dx, dy, dz and intensity.
     * 
     * `dx`, `dy` and `dz` should represent a normalized vector.
     *
     * @param x The x position of the control point
     *
     * @param y The y position of the control point
     *
     * @param z The z position of the control point
     *
     * @param dx The x direction of the control point
     *
     * @param dy The y direction of the control point
     *
     * @param dz The z direction of the control point
     *
     * @param iintensity The intensity of the control point */
    ControlPoint(const float x, const float y, const float z,
        const float dx, const float dy, const float dz,
        const float iintensity)
        : ControlPoint({ x, y, z }, { dx, dy, dz }, iintensity)
    {}

    /** Raw control point constructor with x, y, z and intensity.
     *
     * @param x The x position of the control point
     *
     * @param y The y position of the control point
     *
     * @param z The z position of the control point
     *
     * @param iintensity The intensity of the control point
     */
    ControlPoint(const float x, const float y, const float z, const float iintensity)
        : ControlPoint({ x, y, z }, iintensity)
    {}

    /** Raw control point constructor with position and intensity.
     *
     * @param iposition The position of the control point
     *
     * @param iintensity The intensity of the control point
     */
    ControlPoint(const Vector3& iposition, const float iintensity)
        : ControlPoint(iposition, { 0.f, 0.f, 0.f }, iintensity)
    {}

    /** Raw control point constructor with position, direction and intensity.
     *
     * @param iposition The position of the control point
     *
     * @param idirection The direction vector of the control point
     *
     * @param iintensity The intensity of the control point
     */
    ControlPoint(const Vector3& iposition, const Vector3& idirection, const float iintensity)
    {
        setPosition(iposition);
        setDirection(idirection);
        setIntensity(iintensity);
    }

    /** @return Position of the control point. */
    Vector3 getPosition() const
    {
        return position;
    }

    /** Set position of the control point.
     *
     * @param iposition The position of the control point. */
    void setPosition(Vector3 iposition)
    {
        position = iposition;
    }

    /** @return Control point direction. */
    Vector3 getDirection() const
    {
        return direction;
    }

    /** Set the direction of the control point.
     *
     * @param idirection The direction of the control point. */
    void setDirection(Vector3 idirection)
    {
        direction = idirection.normalize();
    }

    /** @return Intensity of the control point. */
    float getIntensity() const
    {
        return intensity;
    }

    /** Set intensity of the control point.
     *
     * @param iintensity The intensity of the control point. */
    void setIntensity(const float iintensity)
    {
        intensity = fmin(fmax(0.0f, iintensity), 1.0f);
    }

    /** Get the group of this control point.
     *
     * In Streaming emission, control points can be assigned to groups. Control points
     * which are in the same group will be optimised internally to acheive maximum efficiency.
     *
     * For more information on using control point groups, see the \rstref{TutorialStreaming.rst}
     * page of this documentation.
     *
     * @return The current group of this control point. */
    uint8_t getGroup() const
    {
        return group;
    }
    /** Set the group of this control point.
     *
     * In Streaming emission, control points can be assigned to groups. Control points
     * which are in the same group will be optimised internally to acheive maximum efficiency.
     *
     * For more information on using control point groups, see the \rstref{TutorialStreaming.rst}
     * page of this documentation.
     *
     * @param igroup The group to set this control point to. */
    void setGroup(const uint8_t igroup)
    {
        group = igroup;
    }

private:
    /** Position of the control point */
    Vector3 position;
    /** Normalized direction of the control point */
    Vector3 direction;
    /** Intensity of the control point */
    float intensity = 0.0f;
    /** Group of the control point */
    uint8_t group = 0;
};

} // namespace Haptics

} // namespace Ultraleap
