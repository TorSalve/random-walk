#include <conio.h>

#include "Entries.h"

int main(int argc, char* argv[]) {
  // return RandomWalk::KeyboardControlledSensationLibrary::entry(argc, argv);

  // return RandomWalk::KeyboardControlledPoint::entry(argc, argv);

  // return RandomWalk::LeapTrackingParameterSpace::entry(argc, argv);

  // return RandomWalk::MariannasParameterSpace::entry(argc, argv);

  // return RandomWalk::Parameters::entry(argc, argv);

  // return RandomWalk::Parameters::Websockets::entry(argc, argv);

  return RandomWalk::Sensations::Websockets::entry(argc, argv);
}