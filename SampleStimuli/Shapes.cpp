#include <chrono>

#include <Ultrahaptics.hpp>

using Seconds = std::chrono::duration<float>;

struct Circle
{
    // The position of the control point
    Ultrahaptics::Vector3 position;

    // The intensity of the control point
    float intensity;

    // The radius of the circle
    float radius;

    // The frequency at which the control point goes around the circle
    float frequency;


    const Ultrahaptics::Vector3 evaluateAt(Seconds t) {
        // Calculate the x and y positions of the circle and set the height
        position.x = std::cos(2 * M_PI * frequency * t.count()) * radius;
        position.y = std::sin(2 * M_PI * frequency * t.count()) * radius;
        return position;
    }
};