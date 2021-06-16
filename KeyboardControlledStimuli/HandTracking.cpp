#include "HandTracking.h"

namespace RandomWalk::HandTracking {
    std::vector<std::vector<Ultrahaptics::Vector3>> translate_finger_output(LeapOutput* output) {
        return {
            // thumb
            { output->finger_root_thumb, output->finger_proximal_thumb, output->finger_intermediate_thumb, output->finger_tip_thumb },
            // index
            { output->finger_root_index, output->finger_proximal_index, output->finger_intermediate_index, output->finger_tip_index },
            // middle
            { output->finger_root_middle, output->finger_proximal_middle, output->finger_intermediate_middle, output->finger_tip_middle },
            // ring
            { output->finger_root_ring, output->finger_proximal_ring, output->finger_intermediate_ring, output->finger_tip_ring },
            // pinky
            { output->finger_root_pinky, output->finger_proximal_pinky, output->finger_intermediate_pinky, output->finger_tip_pinky },
        };
    }
}