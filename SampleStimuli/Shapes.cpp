#include <chrono>

#include <Ultrahaptics.hpp>

#include "Shapes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using Seconds = std::chrono::duration<float>;

namespace Shapes {

    const Ultrahaptics::Vector3 Circle::evaluateAt(Seconds t) {
        // Calculate the x and y positions of the circle and set the height
        position.x = std::cos(2 * M_PI * frequency * t.count()) * radius;
        position.y = std::sin(2 * M_PI * frequency * t.count()) * radius;
        return position;
    };

    const Ultrahaptics::Vector3 Square::evaluateAt(Seconds t) {
        float fraction = t.count() * frequency;
        fraction -= (long)fraction;

        if (fraction <= 0.25) {
            position.x = -side_length / 2;
            position.y = (-side_length / 2) + (4 * fraction * side_length);
        }
        else if (fraction > 0.25 && fraction <= 0.5) {
            position.x = (-side_length / 2) + (4 * (fraction - 0.25) * side_length);
            position.y = side_length / 2;
        }
        else if (fraction > 0.5 && fraction <= 0.75) {
            position.x = side_length / 2;
            position.y = (side_length / 2) - (4 * (fraction - 0.5) * side_length);
        }
        else {
            position.x = (side_length / 2) - (4 * (fraction - 0.75) * side_length);
            position.y = -side_length / 2;
        }
        return position;
    }

}