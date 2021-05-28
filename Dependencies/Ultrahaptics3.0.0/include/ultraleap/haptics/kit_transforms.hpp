#pragma once
#include <cstdint>

namespace Ultraleap {
namespace Haptics {

    /** All the default values for device transforms */
    namespace KitTransforms
    {
        /** Default Scale value for the UHDK5 Square Board */
        constexpr const float UHDK5_SquareScale = 0.001f;
        /** Default Rotation matrix for the UHDK5 Square Board */
        constexpr const float UHDK5_SquareRotation[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        /** Default Translation value for the UHDK5 Square Board */
        constexpr const float UHDK5_SquareTranslation[3] = { 0.0f, 0.1105f, 0.0f };
        /** Default Scale value for the UHDK5 Rectangular Board */
        constexpr const float UHDK5_RectangularScale = 0.001f;
        /** Default Rotation matrix for the UHDK5 Rectangular Board */
        constexpr const float UHDK5_RectangularRotation[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        /** Default Translation value for the UHDK5 Rectangular Board */
        constexpr const float UHDK5_RectangularTranslation[3] = { 0.0f, 0.163f, 0.0f };
        /** Default Scale value for the UHDK5 Rectangular Board side */
        constexpr const float UHDK5_Rectangular_SideScale = 0.001f;
        /** Default Rotation matrix for the UHDK5 Rectangular Board side */
        constexpr const float UHDK5_Rectangular_SideRotation[9] = { 0.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
        /** Default Translation value for the UHDK5 Rectangular Board side */
        constexpr const float UHDK5_Rectangular_SideTranslation[3] = { 0.0f, 0.163f, 0.0f };
        /** Default Scale value for the USX board */
        constexpr const float USXScale = 0.001f;
        /** Default Rotation matrix for the USX board */
        constexpr const float USXRotation[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        /** Default Translation value for the USX board */
        constexpr const float USXTranslation[3] = { 0.0f, 0.121f, 0.0f };
        /** Default Scale value for the USI board */
        constexpr const float USIScale = 0.001f;
        /** Default Rotation matrix for the USI board */
        constexpr const float USIRotation[9] = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };
        /** Default Translation value for the USI board */
        constexpr const float USITranslation[3] = { 0.0f, -0.089f, 0.0f };
    }

}
}
