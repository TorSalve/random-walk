#include <cmath>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <Leap.h>
#include "LeapHandConverter.hpp"
#include <ultraleap/haptics/sensations.hpp>

#include "ExploringSensationLibrary.h"

namespace RandomWalk::Sensations {

    std::string knownSensationString(KnownSensation sensation) {
        switch (sensation)
        {
        case RandomWalk::Sensations::KnownSensation::CircleWithFixedSpeed:
            return "CircleWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::CircleWithFixedFrequency:
            return "CircleWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::Line:
            return "Line";
        case RandomWalk::Sensations::KnownSensation::Dial:
            return "Dial";
        case RandomWalk::Sensations::KnownSensation::RotorWithFixedSpeed:
            return "RotorWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::RotorWithFixedFrequency:
            return "RotorWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::Scan:
            return "Scan";
        case RandomWalk::Sensations::KnownSensation::OpenCloseWithFixedSpeed:
            return "OpenCloseWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::OpenCloseWithFixedFrequency:
            return "OpenCloseWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::Lissajous:
            return "Lissajous";
        case RandomWalk::Sensations::KnownSensation::Ripple:
            return "Ripple";
        case RandomWalk::Sensations::KnownSensation::AmplitudeModulatedPoint:
            return "AmplitudeModulatedPoint";
        case RandomWalk::Sensations::KnownSensation::HandCircleWithFixedSpeed:
            return "Hand.CircleWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::HandCircleWithFixedFrequency:
            return "Hand.CircleWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::HandLissajous:
            return "Hand.Lissajous";
        case RandomWalk::Sensations::KnownSensation::HandDial:
            return "Hand.Dial";
        case RandomWalk::Sensations::KnownSensation::HandRotorWithFixedSpeed:
            return "Hand.RotorWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::HandRotorWithFixedFrequency:
            return "Hand.RotorWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::HandScan:
            return "Hand.Scan";
        case RandomWalk::Sensations::KnownSensation::HandOpenCloseWithFixedSpeed:
            return "Hand.OpenCloseWithFixedSpeed";
        case RandomWalk::Sensations::KnownSensation::HandOpenCloseWithFixedFrequency:
            return "Hand.OpenCloseWithFixedFrequency";
        case RandomWalk::Sensations::KnownSensation::HandLightning:
            return "Hand.Lightning";
        case RandomWalk::Sensations::KnownSensation::HandRipple:
            return "Hand.Ripple";
        default:
            return "";
        }
    }

    std::list<KnownSensation> allKnownSensations() {
        return {
            KnownSensation::CircleWithFixedSpeed,
            KnownSensation::CircleWithFixedFrequency,
            KnownSensation::Line,
            KnownSensation::Dial,
            KnownSensation::RotorWithFixedSpeed,
            KnownSensation::RotorWithFixedFrequency,
            KnownSensation::Scan,
            KnownSensation::OpenCloseWithFixedSpeed,
            KnownSensation::OpenCloseWithFixedFrequency,
            KnownSensation::Lissajous,
            KnownSensation::Ripple,
            KnownSensation::AmplitudeModulatedPoint,
        };
    }

    class FrameListener : public Leap::Listener
    {
    public:
        FrameListener(std::function<void(const Leap::Controller& controller)> callback)
            : callback(callback)
        {}
        void onFrame(const Leap::Controller& controller) override
        {
            callback(controller);
        }
        std::function<void(const Leap::Controller& controller)> callback;
    };


    Ultraleap::Haptics::result<Ultraleap::Haptics::SensationPackage> LibrarySensation::loadSensationPackage(std::string package) {
        const auto sensation_package_result = Ultraleap::Haptics::SensationPackage::loadFromFile(*lib, package);
        if (!sensation_package_result) {
            std::cerr << "Failed to load from file" << std::endl;
        }
        return sensation_package_result.value();
    }

    int LibrarySensation::start(KnownSensation sensation)
    {
        std::string sensation_name = knownSensationString(sensation);
        auto pattern = sensation_package->sensation(sensation_name);

        if (!pattern) {
            std::cerr << "Failed to get " << sensation_name << " sensation" << std::endl;
            return 1;
        }

        auto sensation_i = Ultraleap::Haptics::SensationInstance(pattern.value());
        sensation_instance = &sensation_i;
        auto set_result = (*emitter).setSensation(*sensation_instance);

        if (sensation_name.rfind("Hand.", 0) == 0) {

            // Set up Leap
            Leap::Controller leap_control;
            leap_control.setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

            // Set up the Leap Frame callback
            LeapHandConverter hand_converter(tracking_transform);
            auto on_frame_callback = [&](const Leap::Controller& controller) {
                const Leap::HandList hands = leap_control.frame().hands();
                if (!hands.isEmpty()) {
                    Leap::Hand hand = hands[0];
                    sensation_i.set("hand", hand_converter.toElementSimpleHand(hand));
                }
                else {
                    (*sensation_instance).set("hand", invalidElementSimpleHand());
                }
                (*emitter).updateSensationArguments(*sensation_instance);
            };
            FrameListener frame_listener = FrameListener(on_frame_callback);
            leap_control.addListener(frame_listener);
        }

        auto emitter_result = (*emitter).start();

        if (!set_result || !emitter_result) {
            std::cerr << "Failed to set " << sensation_name << " sensation or to start the sensation emitter" << std::endl;
            return 2;
        }
        return 0;
    }

    int LibrarySensation::stop()
    {
        // Stop the array
        (*emitter).stop();
        return 0;
    }

    int LibrarySensation::sensationLoop() {
        // Wait for enter key to be pressed.
        std::cout << "Hit ENTER to quit..." << std::endl;
        while (true) {
            std::cout << "Enter radius (mm): ";
            std::string line;
            std::getline(std::cin, line);
            if (line.empty())
                break;
            try {
                int radius_in_mm;
                Ultraleap::Haptics::result<void> result;

                switch (current_sensation)
                {
                case RandomWalk::Sensations::KnownSensation::CircleWithFixedSpeed:
                case RandomWalk::Sensations::KnownSensation::CircleWithFixedFrequency:
                case RandomWalk::Sensations::KnownSensation::HandCircleWithFixedSpeed:
                case RandomWalk::Sensations::KnownSensation::HandCircleWithFixedFrequency:
                    radius_in_mm = std::stof(line);
                    result = (*sensation_instance).set("radius", radius_in_mm * 0.001f);
                    if (result.has_value()) {
                        auto update_result = (*emitter).updateSensationArguments(*sensation_instance);
                        if (!update_result)
                            return 3;

                        std::cout << std::endl << "Updated succeeded: Radius" << std::endl;
                    }
                    else {
                        std::cout << std::endl << "Update failed: is Radius a valid named parameter?" << std::endl;
                    }
                    break;
                default:
                    break;
                }
            }
            catch (const std::invalid_argument& e) {
                std::cerr << e.what() << std::endl;
            }
        }
        return 0;
    }
}