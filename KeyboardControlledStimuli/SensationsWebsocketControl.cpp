#include <conio.h>

#include <ultraleap/haptics/sensations.hpp>

#include "Entries.h"

namespace RandomWalk::Sensations {

using namespace Ultraleap::Haptics;

int entry(int argc, char* argv[]) {
  // Create a Library object and connect it to a running service
  Library lib;
  if (!lib.connect()) {
    std::cerr << "Could not connect to library" << std::endl;
    return 1;
  }

  // We need to acquire the transform from Tracking space to Ultraleap Haptics
  // space
  Ultraleap::Haptics::Transform tracking_transform;

  // Create a streaming emitter and add a device to it
  SensationEmitter emitter{lib};
  if (auto device = lib.findDevice(DeviceFeatures::StreamingHaptics)) {
    result<Transform> transform = device.value().getKitTransform();
    if (!transform) {
      std::cerr << "Unknown device transform" << std::endl;
      return 1;
    }
    tracking_transform = transform.value();

    emitter.addDevice(device.value(), Transform{});
  } else {
    std::cout << "Failed to find real device, falling back to mock device"
              << std::endl;
    if (auto mock_device = lib.getDevice("MockDevice:USX")) {
      emitter.addDevice(mock_device.value(), Transform{});
    } else {
      std::cout << "Failed to get mock device" << std::endl;
      return 1;
    }
  }

  auto lib_sensation = RandomWalk::Sensations::LibrarySensation(&lib, &emitter);

  auto available_sensations = RandomWalk::Sensations::allKnownSensations();
  auto it = available_sensations.begin();
  int idx = 0;

  RandomWalk::Sensations::KnownSensation s = *(std::next(it, idx));
  auto start = lib_sensation.start(s);
  if (start > 0) {
    return start;
  }

  std::cout << "Hit ENTER to quit..." << std::endl;
  std::cout << "Hit \x11 and \x10 to switch sensation" << std::endl;
  std::cout << idx << ") Now playing: "
            << RandomWalk::Sensations::knownSensationString(s) << std::endl;
  while (true) {
    std::string key;
    key = _getch();

    if (key == "\r") {
      break;
    }

    try {
      std::string action;
      int fkey = std::stoi(key);
      float current;

      switch (fkey) {
        case 4:  // left
          idx -= 1;
          break;
        case 6:  // right
          idx += 1;
          break;
        default:
          std::cout << "Command unknown: " << key << std::endl;
          break;
      }

      idx = idx >= available_sensations.size() ? 0 : idx;
      s = *(std::next(it, idx));

      lib_sensation.stop();
      start = lib_sensation.start(s);
      if (start > 0) {
        return start;
      }

      std::cout << idx << ") Now playing: "
                << RandomWalk::Sensations::knownSensationString(s) << std::endl;
    } catch (const std::invalid_argument& e) {
      std::cerr << e.what() << ": " << key << std::endl;
    }
  }

  return lib_sensation.stop();
}
}  // namespace RandomWalk::KeyboardControlledSensationLibrary
}