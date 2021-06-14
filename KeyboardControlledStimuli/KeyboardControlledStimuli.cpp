#include <conio.h>
#include <ultraleap/haptics/sensations.hpp>

#include "Entries.h"

int main(int argc, char *argv[])
{

    //return RandomWalk::KeyboardControlledSensationLibrary::entry(argc, argv);

    //return RandomWalk::KeyboardControlledPoint::entry(argc, argv);

    //return RandomWalk::LeapTrackingParameterSpace::entry(argc, argv);
    
    return RandomWalk::MariannasParameterSpace::entry(argc, argv);
}