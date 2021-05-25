#pragma once

#include <chrono>

#include <Ultrahaptics.hpp>

using Seconds = std::chrono::duration<float>;

namespace Shapes {
    struct Shape {
        Ultrahaptics::Vector3 position = Ultrahaptics::Vector3();
        float intensity{ 1 };
        float frequency{ 100 };
        const Ultrahaptics::Vector3 evaluateAt(Seconds t) {
            return Ultrahaptics::Vector3();
        };
    };

    struct Circle : Shape
    {
        float radius{ 2 * Ultrahaptics::Units::centimetres };
        const Ultrahaptics::Vector3 evaluateAt(Seconds t);
    };

    struct Square : Shape
    {
        float side_length{ 2 * Ultrahaptics::Units::centimetres };
        const Ultrahaptics::Vector3 evaluateAt(Seconds t);
    };
}