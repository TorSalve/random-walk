#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/transform.hpp"
#include "ultraleap/haptics/kit_transforms.hpp"

namespace Ultraleap
{
namespace Haptics
{

/** DeviceTranform contains the default transform for Ultraleap Haptics products.
 *
 * Contains a list of default geometric tranformations which convert vectors
 * from the basis of the tracking system to the basis of the device to which the system is attached
 *
 * @see Transform
 * */
namespace DeviceTransform
{
static Vector3 convertToVector3(const float* values)
{
    return Vector3(values[0], values[1], values[2]);
}

static Matrix3x3 convertToMatrix3x3(const float* values)
{
    return Matrix3x3(
        values[0], values[1], values[2],
        values[3], values[4], values[5],
        values[6], values[7], values[8]);
}

static inline Transform UHDK5_Square()
{
    float scale = KitTransforms::UHDK5_SquareScale;
    Matrix3x3 rotation = convertToMatrix3x3(KitTransforms::UHDK5_SquareRotation);
    Vector3 translation = convertToVector3(KitTransforms::UHDK5_SquareTranslation);

    return { scale * rotation, translation };
}

static inline Transform UHDK5_Rectangular()
{
    float scale = KitTransforms::UHDK5_RectangularScale;
    Matrix3x3 rotation = convertToMatrix3x3(KitTransforms::UHDK5_RectangularRotation);
    Vector3 translation = convertToVector3(KitTransforms::UHDK5_RectangularTranslation);

    return { scale * rotation, translation };
}

static inline Transform UHDK5_Rectangular_Side()
{
    float scale = KitTransforms::UHDK5_Rectangular_SideScale;
    Matrix3x3 rotation = convertToMatrix3x3(KitTransforms::UHDK5_Rectangular_SideRotation);
    Vector3 translation = convertToVector3(KitTransforms::UHDK5_Rectangular_SideTranslation);

    return { scale * rotation, translation };
}

static inline Transform USX()
{
    float scale = KitTransforms::USXScale;
    Matrix3x3 rotation = convertToMatrix3x3(KitTransforms::USXRotation);
    Vector3 translation = convertToVector3(KitTransforms::USXTranslation);

    return { scale * rotation, translation };
}

static inline Transform USI()
{
    float scale = KitTransforms::USIScale;
    Matrix3x3 rotation = convertToMatrix3x3(KitTransforms::USIRotation);
    Vector3 translation = convertToVector3(KitTransforms::USITranslation);

    return { scale * rotation, translation };
}

} // namespace DeviceTransform
} // namespace Haptics
} // namespace Ultraleap
