#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/matrix4x4.hpp"
#include "ultraleap/haptics/matrix3x3.hpp"
#include "ultraleap/haptics/quaternion.hpp"
#include "ultraleap/haptics/vector3.hpp"

#include <cmath>
#include <sstream>

namespace Ultraleap
{
namespace Haptics
{

/** \brief A basis transformation. */
class Transform : public Matrix4x4
{
public:
    Transform(Matrix4x4 affine = Matrix4x4::identity())
        : Matrix4x4(affine)
    {}

    /** Element constructor */
    Transform(float a00, float a01, float a02, float a03,
        float a10, float a11, float a12, float a13,
        float a20, float a21, float a22, float a23,
        float a30, float a31, float a32, float a33)
        : Matrix4x4(a00, a01, a02, a03,
            a10, a11, a12, a13,
            a20, a21, a22, a23,
            a30, a31, a32, a33)
    {}

    /** basis & origin constructor */
    Transform(Matrix3x3 basis,
        Vector3 origin = {})
    {
        setBasis(basis);
        setOrigin(origin);
    }

    Transform(const Quaternion& q, Vector3 origin = {})
    {
        Matrix3x3 basis(q);
        setBasis(basis);
        setOrigin(origin);
    }

    /** Create a new translation transform
     *
     * @param v Translation vector
     *
     * @return New transform for translation
     */
    static Transform translation(Vector3 v)
    {
        return Transform(Matrix4x4::translation(v.x, v.y, v.z));
    }

    /** Create a new translation transform
     *
     * @param x Translation vector, x value
     * @param y Translation vector, y value
     * @param z Translation vector, z value
     *
     * @return New transform for translation
     */
    static Transform translation(float x, float y, float z)
    {
        return Transform(Matrix4x4::translation(x, y, z));
    }

    /** @return the basis */
    Matrix3x3 basis() const
    {
        return Matrix3x3(
            element[0], element[1], element[2],
            element[4], element[5], element[6],
            element[8], element[9], element[10]);
    }

    /** @param basis the basis */
    void setBasis(Matrix3x3 basis)
    {
        element[0] = basis.element[0];
        element[1] = basis.element[1];
        element[2] = basis.element[2];
        element[4] = basis.element[3];
        element[5] = basis.element[4];
        element[6] = basis.element[5];
        element[8] = basis.element[6];
        element[9] = basis.element[7];
        element[10] = basis.element[8];
    }

    /** @return the origin */
    Vector3 origin() const
    {
        return Vector3(element[3], element[7], element[11]);
    }

    /** @param origin the origin */
    void setOrigin(Vector3 origin)
    {
        element[3] = origin.x;
        element[7] = origin.y;
        element[11] = origin.z;
    }

    /** Inverse transform.
     *
     * @return the inverse transform
     */
    Transform inverse() const
    {
        Matrix3x3 inv_b = basis().inverse();
        Vector3 inv_origin = -inv_b * origin();

        return Transform(inv_b, inv_origin);
    }
};
} // namespace Haptics

} // namespace Ultraleap
