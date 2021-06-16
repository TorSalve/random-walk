#pragma once
/**
 * A collection of utilities to convert Leap Hand data into formats used by Sensations
 *
 * Contains ways to convert Leap Hand into ElementSimpleHand.
 */

#include <vector>
#include <Leap.h>
#include "ultraleap/haptics/streaming.hpp"

using namespace Ultraleap::Haptics;

std::vector<float> invalidElementSimpleHand()
{
    std::vector<float> invalid_hand(86, 0);
    return invalid_hand;
}

class LeapHandConverter
{
public:
    LeapHandConverter() {}
    explicit LeapHandConverter(const Transform& transform)
        : _transform(transform)
    {}

    std::vector<float> toElementSimpleHand(const Leap::Hand& hand) const
    {
        if (!hand.isValid()) {
            return invalidElementSimpleHand();
        }
        std::vector<float> element_hand;
        element_hand.reserve(86);
        element_hand.push_back(hand.isValid()); // isValid  :   1 float
        element_hand.push_back(hand.isRight()); // isRight  :   1 float

        appendToVector(element_hand, hand.palmPosition(), true); // position: 3 floats
        appendToVector(element_hand, hand.direction(), false);   // direction: 3 floats
        appendToVector(element_hand, hand.palmNormal(), false);  // normal: 3 floats

        for (const auto& finger : hand.fingers()) {
            appendToVector(element_hand, finger); //  Each finger has 5 * 3 = 15 floats
        }                                         // 5 fingers * 15 floats = 75 float

        return element_hand;
    }

private:
    // Append the Leap::Vector to the std::vector<float>
    //
    //  If is_position = True, interpret vector as a position
    //  If is_position = False, interpret vector as a direction
    //
    void appendToVector(std::vector<float>& current_vector, const Leap::Vector& v, bool is_position) const
    {
        const Vector3 original_vector(v.x, v.y, v.z);

        Vector3 transformed_vector;
        if (is_position) {
            transformed_vector = _transform.transformPosition(original_vector);
        }
        else {
            transformed_vector = _transform.transformDirection(original_vector);
            // All direction vectors in the hand need to be normalised
            transformed_vector = transformed_vector.normalize();
        }

        current_vector.push_back(transformed_vector.x);
        current_vector.push_back(transformed_vector.y);
        current_vector.push_back(transformed_vector.z);
    }

    void appendToVector(std::vector<float>& current_vector, const Leap::Finger& f) const
    {
        auto metacarpal = static_cast<Leap::Bone::Type>(0);
        appendToVector(current_vector, f.bone(metacarpal).prevJoint(), true);

        for (int i = 0; i < 4; ++i) {
            auto bone_type = static_cast<Leap::Bone::Type>(i);
            appendToVector(current_vector, f.bone(bone_type).nextJoint(), true);
        }
    }

    Transform _transform;
};
