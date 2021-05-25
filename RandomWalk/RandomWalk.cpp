// This example uses the Timepoint Streaming emitter to project a control point
// moving in a circle 20 cm above the centre of the array.

#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include "UltrahapticsTimePointStreaming.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

// Structure for passing information on the type of point to create
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

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter& timepoint_emitter,
    Ultrahaptics::TimePointStreaming::OutputInterval& interval,
    const Ultrahaptics::HostTimePoint& submission_deadline,
    void* user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    Circle* circle = static_cast<Circle*>(user_pointer);

    // Loop through the samples in this interval
    for (auto& sample : interval)
    {
        const Seconds t = sample - start_time;
        const Ultrahaptics::Vector3 position = circle->evaluateAt(t);

        // Set the position and intensity of the persistent control point to that of the modulated wave at this point in time.
        sample.persistentControlPoint(0).setPosition(position);
        sample.persistentControlPoint(0).setIntensity(circle->intensity);
    }
}

int main(int argc, char* argv[])
{
    // Create a time point streaming emitter
    Ultrahaptics::TimePointStreaming::Emitter emitter;

    // Set the maximum control point count
    emitter.setMaximumControlPointCount(1);

    // Create a structure containing our control point data and fill it in
    Circle circle;

    // Set control point 20cm above the centre of the array at the radius
    circle.position = Ultrahaptics::Vector3(2.0f * Ultrahaptics::Units::centimetres, 0.0f, 20.0f * Ultrahaptics::Units::centimetres);
    // Set the amplitude of the modulation of the wave to one (full modulation depth)
    circle.intensity = 1.0f;
    // Set the radius of the circle that the point is traversing
    circle.radius = 2.0f * Ultrahaptics::Units::centimetres;
    // Set how many times the point traverses the circle every second
    circle.frequency = 100.0f;

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &circle);

    // Start the array
    emitter.start();

    // Wait for enter key to be pressed.
    std::cout << "Hit ENTER to quit..." << std::endl;
    std::string line;
    std::getline(std::cin, line);

    // Stop the array
    emitter.stop();

    return 0;
}
