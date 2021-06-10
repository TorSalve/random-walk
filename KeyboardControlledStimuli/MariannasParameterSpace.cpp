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

namespace RandomWalk::MariannasParameterSpace
{
    // Structure for passing information on the type of point to create
    struct Config
    {
        // Hand data
        HandTracking::LeapListening hand;

        float intensity{ 1 };

        int frequency{ 256 };
        
        float sine(Seconds t) {
            // return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5;
            return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5 * intensity;
        }

        Ultrahaptics::Vector3 position{ 0, 0, 0 };
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

        // Loop through time, setting control point data
        for (TimePointOnOutputInterval& sample : interval)
        {
            if (!leapOutput.hand_present)
            {
                sample.controlPoint(0).setIntensity(0.0f);
                continue;
            }

            const Seconds t = sample - start_time;
            const Ultrahaptics::Vector3 position = leapOutput.palm_position + config->position;
            //const Ultrahaptics::Vector3 position = Ultrahaptics::Vector3(0 + rand() % 10,250 + rand() % 10,100 + rand() % 10);
            //const Ultrahaptics::Vector3 position = Ultrahaptics::Vector3(0, 250, 100);
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

        // Create a Library object and connect it to a running service
        Library lib;
        auto found_library = lib.connect();
        if (!found_library)
        {
            std::cout << "Library failed to connect: " << found_library.error().message() << std::endl;
            return 1;
        }

        //lib.getLogging().setLogLevel(3);
        //lib.getLogging().redirectLoggingToStdOut();

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

        HandTracking::LeapController leap_control;

        // Create a structure containing our control point data and fill it in from the file.
        Config point;

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
        std::cout << "Hit 1 and 2 to regulate frequency" << std::endl;
        std::cout << "Hit 3 and 4 to regulate intensity" << std::endl;
        std::cout << "Hit 5 and 6 to regulate position x" << std::endl;
        std::cout << "Hit 7 and 8 to regulate position y" << std::endl;

        float intensity_step = 0.1f;
        float intensity_min = 0.f;
        float intensity_max = 1.f;
        float frequency_step = 1.f;
        float frequency_min = 1.f;
        float frequency_max = 10.f;

        const int position_m_x = 4;
        const int position_m_y = 4;
        float position_x_step = 35.f; // in mm
        float position_y_step = 50.f; // in mm
        float position_x_relative_min = -1.5;
        float position_x_relative_max = 1.5;
        float position_y_relative_min = -1;
        float position_y_relative_max = 1;

        std::vector<double> position_x = Utils::linspace(position_x_relative_min, position_x_relative_max, position_m_x);
        Utils::print_vector(position_x);

        std::vector<double> position_y = Utils::linspace(position_y_relative_min, position_y_relative_max, position_m_y);
        Utils::print_vector(position_y);


        Ultrahaptics::Vector3 position_matrix[position_m_x][position_m_y];
        
        for (int i = 0; i < position_m_x; i++)
        {
            for (int j = 0; j < position_m_y; j++)
            {
                float x = *std::next(position_x.begin(), i);
                float y = *std::next(position_y.begin(), j);
                position_matrix[i][j] = Ultrahaptics::Vector3(x, y, 0);
            }
        }

        int position_idx_x = 0;
        int position_idx_y = 0;

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
                case 1:
                    current = std::clamp(log2f((float)point.frequency) - frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency lowered";
                    break;
                case 2:
                    current = std::clamp(log2f((float)point.frequency) + frequency_step, frequency_min, frequency_max);
                    point.frequency = (int)powf(2, current);
                    action = "frequency upped";
                    break;
                case 3:
                    point.intensity = std::clamp(point.intensity - intensity_step, intensity_min, intensity_max);
                    action = "intensity lowered";
                    break;
                case 4:
                    point.intensity = std::clamp(point.intensity + intensity_step, intensity_min, intensity_max);
                    action = "intensity upped";
                    break;
                case 5:
                    position_idx_x = std::clamp(position_idx_x - 1, 0, 3);
                    point.position = position_matrix[position_idx_x][position_idx_y] * position_x_step;
                    action = "position x lowered";
                    break;
                case 6:
                    position_idx_x = std::clamp(position_idx_x + 1, 0, 3);
                    point.position = position_matrix[position_idx_x][position_idx_y] * position_x_step;
                    action = "position x upped";
                    break;
                case 7:
                    position_idx_y = std::clamp(position_idx_y - 1, 0, 3);
                    point.position = position_matrix[position_idx_x][position_idx_y] * position_y_step;
                    action = "position y lowered";
                    break;
                case 8:
                    position_idx_y = std::clamp(position_idx_y + 1, 0, 3);
                    point.position = position_matrix[position_idx_x][position_idx_y] * position_y_step;
                    action = "position y upped";
                    break;
                default:
                    std::cout << "Command unknown: " << key << std::endl;
                    break;
                }
                std::cout << "intensity: " << point.intensity << "\t | frequency: " << point.frequency << "\t | position: " << point.position << ", [" << position_idx_x << "," << position_idx_y << "]\t | " << std::endl;
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