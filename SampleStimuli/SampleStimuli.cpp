// This example uses the Timepoint Streaming emitter to project a control point
// moving in a circle 20 cm above the centre of the array.

#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include <Leap.h>
#include <UltrahapticsTimePointStreaming.hpp>

#include "LeapListener.h"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

// Callback function for filling out complete device output states through time
void my_emitter_callback(const Ultrahaptics::TimePointStreaming::Emitter& timepoint_emitter,
    Ultrahaptics::TimePointStreaming::OutputInterval& interval,
    const Ultrahaptics::HostTimePoint& submission_deadline,
    void* user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint* my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Get a copy of the hand data.
    LeapOutput leapOutput = my_modulated_point->hand.getLeapOutput();
    std::cout << leapOutput.hand_present << " - " << my_modulated_point->sample_number << " - (" << leapOutput.palm_position.x << ", " << leapOutput.palm_position.y << ", " << leapOutput.palm_position.z << ")" << std::endl;

    // Loop through time, setting control point data
    for (auto& sample : interval)
    {
        if (!leapOutput.hand_present) {
            sample.persistentControlPoint(0).setIntensity(0.0f);
            continue;
        }
        // Project the control point onto the palm
        sample.persistentControlPoint(0).setPosition(leapOutput.palm_position);

        // Set the intensity of the point using the waveform. If the hand is not present, intensity is 0
        sample.persistentControlPoint(0).setIntensity(1.0f);

        // Increment sample count
        my_modulated_point->sample_number++;
    }
}

int main(int argc, char* argv[])
{

    // Create a time point streaming emitter
    Ultrahaptics::TimePointStreaming::Emitter emitter;
    Leap::Controller leap_control;

    ModulatedPoint point(emitter.getDeviceInfo().getDefaultAlignment());

    // Set the leap motion to listen for background frames.
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // Update our structure with data from our Leap listener
    leap_control.addListener(point.hand);

    // Get the achievable sample_rate for the number of control points we are producing
    unsigned int hardware_sample_rate = emitter.setMaximumControlPointCount(1);
    
    point.sample_number = 0;

    // Set the callback function to the callback written above
    emitter.setEmissionCallback(my_emitter_callback, &point);

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
