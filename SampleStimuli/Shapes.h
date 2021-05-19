#pragma once

#include <chrono>

#include <Ultrahaptics.hpp>

using Seconds = std::chrono::duration<float>;

struct Circle
{
    Ultrahaptics::Vector3 position;
    float intensity {0};
    float radius{0};
    float frequency{0};
    const Ultrahaptics::Vector3 evaluateAt(Seconds t);
};