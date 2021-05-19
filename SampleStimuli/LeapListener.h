#pragma once

#include <Leap.h>
#include <Ultrahaptics.hpp>

#ifndef LEAPLISTENER_H 
#define LEAPLISTENER_H
// Structure to represent output from the Leap listener
struct LeapOutput {
    LeapOutput() noexcept {};
    Ultrahaptics::Vector3 palm_position;
    bool hand_present = false;
};

// Leap listener class - tracking the hand position and creating data structure for use by Ultrahaptics API
class LeapListening : public Leap::Listener {
public:
	LeapListening(const Ultrahaptics::Alignment& align) : alignment(align) {  }

	~LeapListening() = default;

	LeapListening(const LeapListening & other) = delete;
	LeapListening& operator=(const LeapListening & other) = delete;
	
	void onFrame(const Leap::Controller& controller) override {
		// Get all the hand positions from the leap and position a focal point on each.
		Leap::Frame frame = controller.frame();
		Leap::HandList hands = frame.hands();

		LeapOutput local_hand_data;

		if (hands.isEmpty())
		{
			local_hand_data.palm_position = Ultrahaptics::Vector3();
			local_hand_data.hand_present = false;
		}
		else
		{
			// Get the data for the first hand
			Leap::Hand hand = hands[0];

			// Translate the hand position from leap objects to Ultrahaptics objects.
			Leap::Vector leap_palm_position = hand.palmPosition();

			// Convert to Ultrahaptics vectors, normal is negated as leap normal points down.
			Ultrahaptics::Vector3 uh_palm_position(leap_palm_position.x, leap_palm_position.y, leap_palm_position.z);

			// Convert to device space from leap space.
			Ultrahaptics::Vector3 device_palm_position = alignment.fromTrackingPositionToDevicePosition(uh_palm_position);

			// Update the hand data with the current information
			local_hand_data.palm_position = device_palm_position;
			local_hand_data.hand_present = true;
		}

		atomic_local_hand_data.store(local_hand_data);
	}
	LeapOutput getLeapOutput() {
		return atomic_local_hand_data.load();
	}

private:
    std::atomic <LeapOutput> atomic_local_hand_data;
    Ultrahaptics::Alignment alignment;
};

class ModulatedPoint {
public:
	ModulatedPoint(const Ultrahaptics::Alignment& align): hand(align), sample_number(0) { }
    LeapListening hand;
    size_t sample_number;
};
#endif