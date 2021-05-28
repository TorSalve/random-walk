#include <Leap.h>

#include "ultraleap/haptics/library.hpp"

namespace RandomWalk::HandTracking {

    // Structure to represent output from the Leap listener
    struct LeapOutput
    {
        LeapOutput() noexcept
        {}

        Ultraleap::Haptics::Vector3 palm_position;
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
                local_hand_data.palm_position = Ultraleap::Haptics::Vector3();
                local_hand_data.hand_present = false;
            }
            else {
                // Get the data for the first hand
                Leap::Hand hand = hands[0];

                // Translate the hand position from leap objects to Ultraleap Haptics objects.
                Leap::Vector leap_palm_position = hand.palmPosition();

                // Convert to Ultraleap Haptics vectors, normal is negated as leap normal points down.
                Ultraleap::Haptics::Vector3 ulh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);

                // Update the hand data with the current information
                local_hand_data.palm_position = ulh_palm_position;
                local_hand_data.hand_present = true;
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