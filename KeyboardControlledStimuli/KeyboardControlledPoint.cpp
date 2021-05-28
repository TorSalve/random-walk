// This example uses the Streaming emitter and a Leap Motion Controller.
// It reads in a WAV file and projects a point focussed onto the height
// of the palm. The intensity of the point is modulated with the WAV data.

#include <atomic>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <ultraleap/haptics/streaming.hpp>

#include "HandTracking.h"
#include "Entries.h"

using namespace Ultraleap::Haptics;

using Seconds = std::chrono::duration<float>;
static auto start_time = std::chrono::steady_clock::now();

using namespace RandomWalk;

namespace RandomWalk::KeyboardControlledPoint
{
    // Structure for passing information on the type of point to create
    struct ModulatedPoint
    {
        // Hand data
        HandTracking::LeapListening hand;

        float intensity{1};

        int frequency{512};

        float last_freq{0};
    };

    // Callback function for filling out complete device output states through time
    void my_emitter_callback(const StreamingEmitter &emitter,
                             OutputInterval &interval,
                             const LocalTimePoint &submission_deadline,
                             void *user_pointer)
    {
        // Cast the user pointer to the struct that describes the control point behaviour
        ModulatedPoint *my_modulated_point = static_cast<ModulatedPoint *>(user_pointer);

        // Get a copy of the hand data.
        HandTracking::LeapOutput leapOutput = my_modulated_point->hand.getLeapOutput();
        float frac = 1.f / my_modulated_point->frequency;
        float last_frac = my_modulated_point->last_freq + frac;

        // Loop through time, setting control point data
        for (TimePointOnOutputInterval &sample : interval)
        {
            if (!leapOutput.hand_present)
            {
                sample.controlPoint(0).setIntensity(0.0f);
                continue;
            }
            const Seconds t = sample - start_time;

            if (last_frac >= t.count())
            {
                sample.controlPoint(0).setIntensity(0.0f);
                continue;
            }

            // std::cout << t.count() << " " << frac << " " << my_modulated_point->last_freq << " " << frac + my_modulated_point->last_freq << std::endl;
            //std::cout << t.count() << " | " << my_modulated_point->intensity << std::endl;

            // Project the control point onto the palm
            sample.controlPoint(0).setPosition(leapOutput.palm_position);

            // Set the intensity of the point using the waveform. If the hand is not present, intensity is 0.
            sample.controlPoint(0).setIntensity(my_modulated_point->intensity);

            my_modulated_point->last_freq = t.count();
        }
    }

    int entry(int argc, char *argv[])
    {

        // Create a Library object and connect it to a running service
        Library lib;
        auto found_library = lib.connect();
        if (!found_library)
        {
            std::cout << "Library failed to connect: " << found_library.error().message() << std::endl;
            return 1;
        }

        // Create a streaming emitter and add a suitable device to it
        StreamingEmitter emitter{lib};
        auto device = lib.findDevice(DeviceFeatures::StreamingHaptics);
        if (!device)
        {
            std::cout << "Failed to find device: " << device.error().message() << std::endl;
            return 1;
        }

        // If we found a device, get the default transform from Leap to Haptics device space
        auto transform = device.value().getKitTransform();
        if (!transform)
        {
            std::cerr << "Unknown device transform: " << transform.error().message() << std::endl;
            return 1;
        }

        auto add_res = emitter.addDevice(device.value(), transform.value());
        if (!add_res)
        {
            std::cout << "Failed to add device: " << add_res.error().message() << std::endl;
            return 1;
        }

        HandTracking::LeapController leap_control;

        // Create a structure containing our control point data and fill it in from the file.
        ModulatedPoint point;

        leap_control.addListener(point.hand);

        // Get the achievable sample_rate for the number of control points we are producing
        auto cp_res = emitter.setControlPointCount(1, AdjustRate::All);
        if (!cp_res)
        {
            std::cout << "Failed to setControlPointCount: " << cp_res.error().message() << std::endl;
            return 1;
        }

        // Set the callback function to the callback written above
        auto ec_res = emitter.setEmissionCallback(my_emitter_callback, &point);
        if (!ec_res)
        {
            std::cout << "Failed to setEmissionCallback: " << ec_res.error().message() << std::endl;
            return 1;
        }

        // Start the array
        emitter.start();

        // Wait for enter key to be pressed.
        std::cout << "Hit ENTER to quit..." << std::endl;
        std::cout << "Hit \x1e and \x1f to regulate intensity, hit \x11 and \x10 to regulate frequency" << std::endl;

        float intensity_step = 0.1f;
        float intensity_min = 0.f;
        float intensity_max = 1.f;
        float frequency_step = 1.f;
        float frequency_min = 1.f;
        float frequency_max = 15.f;

        while (true)
        {
            std::string key;
            key = _getch();

            if (key == "\r")
            {
                break;
            }

            try
            {
                std::string action;
                int fkey = std::stoi(key);
                float current;

                switch (fkey)
                {
                case 4: // left
                    current = std::clamp(log2f((float)point.frequency) - frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency lowered";
                    break;
                case 6: // right
                    current = std::clamp(log2f((float)point.frequency) + frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency upped";
                    break;
                case 2: // down
                    point.intensity = std::clamp(point.intensity - intensity_step, intensity_min, intensity_max);
                    action = "intensity lowered";
                    break;
                case 8: // up
                    point.intensity = std::clamp(point.intensity + intensity_step, intensity_min, intensity_max);
                    action = "intensity upped";
                    break;
                default:
                    std::cout << "Command unknown: " << key << std::endl;
                    break;
                }
                std::cout << "intensity: " << point.intensity << "\t\t | frequency: " << point.frequency << "\t\t | " << action << std::endl;
            }
            catch (const std::invalid_argument &e)
            {
                std::cerr << e.what() << ": " << key << std::endl;
            }
        }

        // Stop the array
        emitter.stop();

        return 0;
    }
}