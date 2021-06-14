// This example uses the Streaming emitter and a Leap Motion Controller.
// It reads in a WAV file and projects a point focussed onto the height
// of the palm. The intensity of the point is modulated with the WAV data.

#include <atomic>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <Leap.h>

#include "ultraleap/haptics/streaming.hpp"

using namespace Ultraleap::Haptics;

// Structure to represent output from the Leap listener
struct LeapOutput
{
    LeapOutput() noexcept
    {}

    Vector3 palm_position;
    Vector3 x_axis;
    bool hand_present = false;
};

// Leap listener class - tracking the hand position and creating data structure for use by Ultraleap Haptics API
class LeapListening : public Leap::Listener
{
public:
    LeapListening() = default;
    ~LeapListening() = default;

    LeapListening(const LeapListening& other) = delete;
    LeapListening& operator=(const LeapListening& other) = delete;

    void onFrame(const Leap::Controller& controller) override
    {
        // Get all the hand positions from the leap and position a focal point on the first
        Leap::Frame frame = controller.frame();
        Leap::HandList hands = frame.hands();

        LeapOutput local_hand_data;

        if (hands.isEmpty()) {
            local_hand_data.palm_position = Vector3();
            local_hand_data.hand_present = false;
        } else {
            // Get the data for the first hand
            Leap::Hand hand = hands[0];

            Leap::Vector leap_palm_position = hand.palmPosition();
            Leap::Vector leap_palm_direction = hand.direction();
            Leap::Vector leap_palm_normal= hand.palmNormal();

            // Convert to Ultraleap Haptics vectors, normal is negated as leap normal points down.
            Ultrahaptics::Vector3 ulh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
            Ultrahaptics::Vector3 ulh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);
            Ultrahaptics::Vector3 ulh_palm_normal(leap_palm_normal.x, leap_palm_normal.y, leap_palm_normal.z);

            // Check palm position is within 14cm either side of the centre of array.
            if (ulh_palm_position.x >= -140.f && ulh_palm_position.x <= 140.f) {
                // These can then be converted to be a unit axis on the palm of the hand.
                Vector3 ulh_palm_z = ulh_palm_normal;                          // Unit Z direction.
                Vector3 ulh_palm_y = ulh_palm_direction;                       // Unit Y direction.
                Vector3 ulh_palm_x = ulh_palm_y.cross(ulh_palm_z).normalize(); // Unit X direction.

                local_hand_data.palm_position = ulh_palm_position;
                local_hand_data.x_axis = ulh_palm_x;
                local_hand_data.hand_present = true;
            }
        }

        atomic_local_hand_data.store(local_hand_data);
    }

    LeapOutput getLeapOutput()
    {
        return atomic_local_hand_data.load();
    }

private:
    std::atomic<LeapOutput> atomic_local_hand_data;
};

// Structure for passing information on the type of point to create
struct ModulatedPoint
{
    // Hand data
    LeapListening hand;

    // The position of the control point
    Vector3 position;

    // The position of the leap
    Vector3 leap_offset;

    // The width of the forcefield
    float forcefield_width;

    // The frequency at the forcefield is created
    float forcefield_frequency;

    // The offset of the control point at the last sample time
    float offset = 0.0;

    // This allows us to easily reverse the direction of the point.
    // It is 1 or -1.
    int direction = 1;
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const StreamingEmitter& emitter,
    OutputInterval& interval,
    const LocalTimePoint& submission_deadline,
    void* user_pointer)
{
    // Cast the user pointer to the struct that describes the control point behaviour
    ModulatedPoint* my_modulated_point = static_cast<ModulatedPoint*>(user_pointer);

    // Set interval offset between control point samples
    float interval_offset = my_modulated_point->forcefield_width * my_modulated_point->forcefield_frequency / 40000.0;

    // Get a copy of the hand data.
    LeapOutput leapOutput = my_modulated_point->hand.getLeapOutput();

    // Loop through time, setting control point data
    for (TimePointOnOutputInterval& sample : interval) {
        if (!leapOutput.hand_present) {
            sample.controlPoint(0).setIntensity(0.0f);
            continue;
        }
        double os = my_modulated_point->offset;
        // Point moves back and forth along the x-axis of the palm, forming a very thin rectangle with a small y-axis variation
        my_modulated_point->position.x = leapOutput.palm_position.x + (my_modulated_point->direction * os);
        my_modulated_point->position.y = (os * leapOutput.x_axis.y) + leapOutput.palm_position.y;
        my_modulated_point->position.z = (my_modulated_point->direction * 5.f);

        my_modulated_point->position += my_modulated_point->leap_offset;

        sample.controlPoint(0).setPosition(my_modulated_point->position);
        sample.controlPoint(0).setIntensity(1.0f);

        // Update previous sample offset for next loop iteration
        my_modulated_point->offset += interval_offset * my_modulated_point->direction;

        // Check if we reached the edge of the forcefield projection width, and reverse direction if so
        if (fabs(my_modulated_point->offset) > my_modulated_point->forcefield_width / 2) {
            // Toggle the direction.
            my_modulated_point->direction = -my_modulated_point->direction;
        }
    }
}

int main(int argc, char* argv[])
{
    // Create a Library object and connect it to a running service
    Library lib;
    auto found_library = lib.connect();
    if (!found_library) {
        std::cout << "Library failed to connect: " << found_library.error().message() << std::endl;
        return 1;
    }

    // Create a streaming emitter and add a suitable device to it
    StreamingEmitter emitter{ lib };
    auto device = lib.findDevice(DeviceFeatures::StreamingHaptics);
    if (!device) {
        std::cout << "Failed to find device: " << device.error().message() << std::endl;
        return 1;
    }

    // If we found a device, get the default transform from Leap to Haptics device space
    auto transform = device.value().getKitTransform();
    if (!transform) {
        std::cerr << "Unknown device transform: " << transform.error().message() << std::endl;
        return 1;
    }

    auto add_res = emitter.addDevice(device.value(), transform.value());
    if (!add_res) {
        std::cout << "Failed to add device: " << add_res.error().message() << std::endl;
        return 1;
    }

    Leap::Controller leap_control;

    // Create a structure containing our control point data and fill it in from the file.
    ModulatedPoint point;

    
    // Set the leap motion to listen for background frames.
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // Update our structure with data from our Leap listener
    leap_control.addListener(point.hand);

    // Get the achievable sample_rate for the number of control points we are producing
    auto cp_res = emitter.setControlPointCount(1, AdjustRate::All);
    if (!cp_res) {
        std::cout << "Failed to setControlPointCount: " << cp_res.error().message() << std::endl;
        return 1;
    }

    // Set the position of the leap relative to the array
    point.leap_offset = Vector3(0.f, 0.f, 121.f);

    // Set the width of the forcefield that the point is traversing
    point.forcefield_width = 100.f;

    // Set how many times the point traverses the forcefield every second
    point.forcefield_frequency = 100.0;

    // Set the callback function to the callback written above
    auto ec_res = emitter.setEmissionCallback(my_emitter_callback, &point);
    if (!ec_res) {
        std::cout << "Failed to setEmissionCallback: " << ec_res.error().message() << std::endl;
        return 1;
    }

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
