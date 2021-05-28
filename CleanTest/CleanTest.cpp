/**
 * Sensation_TrackedDial.cpp
 *
 * A 'Dial' sensation is played while tracking a hand.
 *
 * This example shows how to use hand-tracked sensations, loaded from a .ssp file.
 *
 * This shows how to play a sensation, and change the sensation parameters during playback.
 * In this case, the 'hand' property of the sensation is set every time a new Leap Frame
 * is provided.
 *
 * This shows use of the LeapHandConverter class to convert 'Leap.Hand's into a format
 * which can be used as an Element SimpleHand.
 */

#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include <cstdint>

#include <Leap.h>

#include "ultraleap/haptics/sensations.hpp"
#include "ultraleap/haptics/streaming.hpp"

#include "LeapHandConverter.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using namespace Ultraleap::Haptics;

class FrameListener2 : public Leap::Listener
{
public:
    FrameListener2(std::function<void(const Leap::Controller& controller)> callback)
        : callback(callback)
    {}
    void onFrame(const Leap::Controller& controller) override
    {
        callback(controller);
    }
    std::function<void(const Leap::Controller& controller)> callback;
};


int main()
{
    // Create a Library object and connect it to a running service
    Library lib;
    if (!lib.connect()) {
        std::cerr << "Could not connect to library" << std::endl;
        return 1;
    }

    // We need to acquire the transform from Tracking space to Ultraleap Haptics space
    Transform tracking_transform;

    // Create a streaming emitter and add a device to it
    SensationEmitter emitter{ lib };
    if (auto device = lib.findDevice(DeviceFeatures::StreamingHaptics)) {

        result<Transform> transform = device.value().getKitTransform();
        if (!transform) {
            std::cerr << "Unknown device transform" << std::endl;
            return 1;
        }
        tracking_transform = transform.value();

        // Start a device but do not add a Transform for Sensations
        emitter.addDevice(device.value(), Transform{});
    }
    else {
        std::cout << "Failed to find real device, falling back to mock device" << std::endl;
        if (auto mock_device = lib.getDevice("MockDevice:USX")) {
            emitter.addDevice(mock_device.value(), Transform{});
        }
        else {
            std::cout << "Failed to get mock device" << std::endl;
            return 1;
        }
    }

    // Load the sensation package
    const auto sensation_package_result = SensationPackage::loadFromFile(lib, "StandardSensations.ssp");
    if (!sensation_package_result)
        return 1;
    const auto& sensation_package = sensation_package_result.value();

    // Load the data of the named Sensation
    const auto hand_tracked_sensation = sensation_package.sensation("Hand.Dial");
    if (!hand_tracked_sensation)
        return 1;

    // Created an instance of the Sensation
    auto sensation_instance = SensationInstance(hand_tracked_sensation.value());

    // Set the instance on the emitter
    const auto set_result = emitter.setSensation(sensation_instance);
    if (!set_result)
        return 2;

    // Set up Leap
    Leap::Controller leap_control;
    leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // Set up the Leap Frame callback
    LeapHandConverter hand_converter(tracking_transform);
    auto on_frame_callback = [&](const Leap::Controller& controller) {
        const Leap::HandList hands = leap_control.frame().hands();
        if (!hands.isEmpty()) {
            Leap::Hand hand = hands[0];
            sensation_instance.set("hand", hand_converter.toElementSimpleHand(hand));
        }
        else {
            sensation_instance.set("hand", invalidElementSimpleHand());
        }
        emitter.updateSensationArguments(sensation_instance);
    };
    FrameListener2 frame_listener = FrameListener2(on_frame_callback);
    leap_control.addListener(frame_listener);

    // Start the emitter
    const auto emitter_result = emitter.start();
    if (!emitter_result)
        return 2;

    std::cout << "Hit ENTER to quit..." << std::endl;
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty())
            break;
    }

    // Stop the array
    emitter.stop();

    return 0;
}
