#pragma once

#include <Leap.h>
#include <list>

#include "ultraleap/haptics/library.hpp"

namespace RandomWalk::HandTracking {

    // Structure to represent output from the Leap listener
    struct LeapOutput
    {
        LeapOutput() noexcept
        {}

        Ultrahaptics::Vector3 palm_position;
        Ultrahaptics::Vector3 palm_direction;
        Ultrahaptics::Vector3 wrist_position;
        //std::vector<Ultrahaptics::Vector3> finger_roots;
        //std::vector<Ultrahaptics::Vector3> finger_tips;
        Ultrahaptics::Vector3 finger_root_thumb;
        Ultrahaptics::Vector3 finger_root_index;
        Ultrahaptics::Vector3 finger_root_middle;
        Ultrahaptics::Vector3 finger_root_ring;
        Ultrahaptics::Vector3 finger_root_pinky;

        Ultrahaptics::Vector3 finger_tip_thumb;
        Ultrahaptics::Vector3 finger_tip_index;
        Ultrahaptics::Vector3 finger_tip_middle;
        Ultrahaptics::Vector3 finger_tip_ring;
        Ultrahaptics::Vector3 finger_tip_pinky;
        
        //Leap::FingerList fingers;
        bool hand_present = false;
        bool hand_is_left = false;
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
                local_hand_data.palm_position = Ultraleap::Haptics::Vector3();
                local_hand_data.hand_present = false;
            }
            else {
                // Get the data for the first hand
                Leap::Hand hand = hands[0];

                // Translate the hand position from leap objects to Ultraleap Haptics objects.
                Leap::Vector leap_palm_position = hand.palmPosition();
                Leap::Vector leap_palm_direction = hand.direction();
                Leap::Vector leap_wrist_position = hand.wristPosition();

                // hand.fingers()
                std::vector<Ultrahaptics::Vector3> finger_roots;
                finger_roots.reserve(5);
                for (auto& finger : hand.fingers())
                {
                    auto finger_root = finger.bone(Leap::Bone::Type::TYPE_METACARPAL).center();
                    finger_roots.emplace_back(finger_root.x, finger_root.y, finger_root.z);
                }

                std::vector<Ultrahaptics::Vector3> finger_tips;
                finger_tips.reserve(5);
                for (auto& finger : hand.fingers())
                {
                    auto finger_tip = finger.bone(Leap::Bone::Type::TYPE_DISTAL).center();
                    finger_tips.emplace_back(finger_tip.x, finger_tip.y, finger_tip.z);
                }

                // Convert to Ultraleap Haptics vectors, normal is negated as leap normal points down.
                Ultrahaptics::Vector3 ulh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);
                Ultrahaptics::Vector3 ulh_palm_direction(leap_palm_direction.x, leap_palm_direction.y, leap_palm_direction.z);
                Ultrahaptics::Vector3 ulh_wrist_position(leap_wrist_position.x, leap_wrist_position.y, leap_wrist_position.z);

                // Update the hand data with the current information
                local_hand_data.palm_position = ulh_palm_position;
                local_hand_data.palm_direction= ulh_palm_direction;
                local_hand_data.wrist_position = ulh_wrist_position;

                local_hand_data.finger_root_thumb = finger_roots[0];
                local_hand_data.finger_root_index = finger_roots[1];
                local_hand_data.finger_root_middle = finger_roots[2];
                local_hand_data.finger_root_ring = finger_roots[3];
                local_hand_data.finger_root_pinky = finger_roots[4];

                local_hand_data.finger_tip_thumb = finger_tips[0];
                local_hand_data.finger_tip_index = finger_tips[1];
                local_hand_data.finger_tip_middle = finger_tips[2];
                local_hand_data.finger_tip_ring = finger_tips[3];
                local_hand_data.finger_tip_pinky = finger_tips[4];


                //local_hand_data.finger_tips = finger_tips;
                //local_hand_data.fingers = hand.fingers();
                local_hand_data.hand_present = true;
                local_hand_data.hand_is_left = hand.isLeft();
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

    class LeapController: public Leap::Controller {
    public:
        LeapController() : Leap::Controller () {
            // Set the leap motion to listen for background frames.
            setPolicyFlags(Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);
        }
    };

}