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
#include <list>
#include <numeric>
#include <ultraleap/haptics/streaming.hpp>

#include "HandTracking.h"
#include "Entries.h"
#include "Utils.h"

using namespace Ultraleap::Haptics;

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

using namespace RandomWalk;

namespace RandomWalk::LeapTrackingParameterSpace
{
    // Structure for passing information on the type of point to create
    struct Config
    {
        // Hand data
        HandTracking::LeapListening hand;

        float intensity{ 1 };

        int frequency{ 256 };

        Ultrahaptics::Vector3 leap_offset;
        
        float sine(Seconds t) {
            // return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5;
            return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5 * intensity;
        }

        //Ultrahaptics::Vector3 position{ 0, 0, 0 };
        int position = 0;
    };

    // Callback function for filling out complete device output states through time
    void my_emitter_callback(const StreamingEmitter& emitter,
        OutputInterval& interval,
        const LocalTimePoint& submission_deadline,
        void* user_pointer)
    {
        // Cast the user pointer to the struct that describes the control point behaviour
        Config* config = static_cast<Config*>(user_pointer);

        // Get a copy of the hand data.
        HandTracking::LeapOutput leapOutput = config->hand.getLeapOutput();

        std::vector<Ultrahaptics::Vector3> positions = {
            leapOutput.palm_position,
            leapOutput.wrist_position,
            leapOutput.finger_root_thumb, 
            leapOutput.finger_root_index, 
            leapOutput.finger_root_middle, 
            leapOutput.finger_root_ring, 
            leapOutput.finger_root_pinky,
            leapOutput.finger_tip_thumb,
            leapOutput.finger_tip_index,
            leapOutput.finger_tip_middle,
            leapOutput.finger_tip_ring,
            leapOutput.finger_tip_pinky
        };

        // Loop through time, setting control point data
        for (TimePointOnOutputInterval& sample : interval)
        {
            if (!leapOutput.hand_present)
            {
                sample.controlPoint(0).setIntensity(0.0f);
                continue;
            }

            const Seconds t = sample - start_time;

            Ultrahaptics::Vector3 position = positions[config->position] + (config->leap_offset * (leapOutput.hand_is_left ? -1 : 1));

            // std::cout << position << std::endl;

            const float intensity = config->sine(t);
            
            // std::cout << position << std::endl;

            
            // fingers[0].bone(Leap::Bone::Type::TYPE_DISTAL).center();

            // Project the control point onto the palm
            sample.controlPoint(0).setPosition(position);

            // Set the intensity of the point using the waveform. If the hand is not present, intensity is 0.
            sample.controlPoint(0).setIntensity(intensity);

        }
    }

    int entry(int argc, char* argv[])
    {

#pragma region INIT_DEVICE
        // Create a Library object and connect it to a running service
        Library lib;
        auto found_library = lib.connect();
        
        /*lib.getLogging().setLogLevel(3);
        lib.getLogging().redirectLoggingToStdOut();*/

        if (!found_library)
        {
        
            std::cout << "Library failed to connect: " << found_library.error().message() << std::endl;
            return 1;
        }

        // Create a streaming emitter and add a suitable device to it
        StreamingEmitter emitter{ lib };
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
#pragma endregion INIT_DEVICE

#pragma region INIT_LEAP
        HandTracking::LeapController leap_control;

        // Create a structure containing our control point data and fill it in from the file.
        Config point;

        point.leap_offset = Vector3(-20.f, 0.f, 0.f);

        leap_control.addListener(point.hand);
#pragma endregion INIT_LEAP

#pragma region INIT_EMITTER
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
#pragma endregion INIT_EMITTER

        // Wait for enter key to be pressed.
        std::cout << "Hit ENTER to quit..." << std::endl;
        std::cout << "Hit 1 and 2 to regulate frequency" << std::endl;
        std::cout << "Hit 3 and 4 to regulate intensity" << std::endl;
        std::cout << "Hit 5 and 6 to regulate position" << std::endl;
        // std::cout << "Hit 7 and 8 to regulate position y" << std::endl;

        float intensity_step = 0.1f;
        float intensity_min = 0.f;
        float intensity_max = 1.f;
        float frequency_step = 1.f;
        float frequency_min = 1.f;
        float frequency_max = 10.f;

        int position_idx = 0;

        std::vector<std::string> positions = {
            "palm",
            "wrist",
            "thumb root",
            "index root",
            "middle root",
            "ring root",
            "pinky root",
            "thumb tip",
            "index tip",
            "middle tip",
            "ring tip",
            "pinky tip"
        };

        while (true)
        {
            std::string key;
            key = _getch();
            std::cout << key << std::endl;

            if (key == "\r")
            {
                break;
            }

            try
            {
                std::string action;
                float current;

                switch (Utils::hash(key.c_str()))
                {
                case Utils::hash("1"):
                    current = std::clamp(log2f((float)point.frequency) - frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency lowered";
                    break;
                case Utils::hash("2"):
                    current = std::clamp(log2f((float)point.frequency) + frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency upped";
                    break;
                case Utils::hash("3"):
                    point.intensity = std::clamp(point.intensity - intensity_step, intensity_min, intensity_max);
                    action = "intensity lowered";
                    break;
                case Utils::hash("4"):
                    point.intensity = std::clamp(point.intensity + intensity_step, intensity_min, intensity_max);
                    action = "intensity upped";
                    break;
                case Utils::hash("5"):
                    point.position = std::clamp(point.position - 1, 0, (int) positions.size() - 1);
                    action = "position lowered";
                    break;
                case Utils::hash("6"):
                    point.position = std::clamp(point.position + 1, 0, (int)positions.size() - 1);
                    action = "position upped";
                    break;
                case Utils::hash("q"):
                    point.leap_offset += Vector3(0.5f, 0.f, 0.f);
                    action = "+x";
                    break;
                case Utils::hash("w"):
                    point.leap_offset -= Vector3(0.5f, 0.f, 0.f);
                    action = "-x";
                    break;
                case Utils::hash("a"):
                    point.leap_offset += Vector3(0.f, 0.5f, 0.f);
                    action = "+y";
                    break;
                case Utils::hash("s"):
                    point.leap_offset -= Vector3(0.f, 0.5f, 0.f);
                    action = "-y";
                    break;
                case Utils::hash("z"):
                    point.leap_offset += Vector3(0.f, 0.f, 0.5f);
                    action = "+z";
                    break;
                case Utils::hash("x"):
                    point.leap_offset -= Vector3(0.f, 0.f, 0.5f);
                    action = "-z";
                    break;
                default:
                    std::cout << "Command unknown: " << key << std::endl;
                    break;
                }
                std::cout << "intensity: " << point.intensity << "\t | frequency: " << point.frequency << "\t | position: " << positions[point.position] << "\t | offset: " << point.leap_offset << std::endl;
            }
            catch (const std::invalid_argument& e)
            {
                std::cerr << e.what() << ": " << key << std::endl;
            }
        }

        // Stop the array
        emitter.stop();

        return 0;
    }
}