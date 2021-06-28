// This example uses the Streaming emitter and a Leap Motion Controller.
// It reads in a WAV file and projects a point focussed onto the height
// of the palm. The intensity of the point is modulated with the WAV data.

#include <conio.h>
#include <atomic>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <ultraleap/haptics/streaming.hpp>
#include <vector>

#include "Entries.h"
#include "HandTracking.h"
#include "Utils.hpp"

using namespace Ultraleap::Haptics;

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

using namespace RandomWalk;

namespace RandomWalk::MariannasParameterSpace {
const int cells = 4;

enum class RenderMode { STATIC, DYNAMIC };

class Pattern {
 public:
  Pattern() : pattern({{0, 0}}), duration(200), m_name("Pattern") {}

 protected:
  std::vector<std::tuple<int, int>> pattern;
  // pattern cycle in ms
  float duration;
  std::string m_name;

 public:
  void set_duration(int new_duration) { duration = (float)new_duration; }
  int get_duration() { return (int)duration; }
  std::string name() { return m_name; }
  Ultrahaptics::Vector3 evaluate_at(
      Seconds t,
      Ultrahaptics::Vector3 offsets[cells][cells]) {
    float s = t.count();
    float ms = s * 1000;
    int len = pattern.size();
    int idx = (int)floor(ms / duration) % len;
    auto p = pattern[idx];
    // std::cout << std::get<0>(p) << std::get<1>(p) << std::endl;
    return offsets[std::get<0>(p)][std::get<1>(p)];
  }
};

class ULtBR : public Pattern {
 public:
  ULtBR() {
    pattern = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    duration = 200;
    m_name = "ULtBR";
  };
};

class BLtBR : public Pattern {
 public:
  BLtBR() {
    pattern = {{3, 0}, {3, 1}, {3, 2}, {3, 3}};
    duration = 200;
    m_name = "BLtBR";
  };
};

class ULtUR : public Pattern {
 public:
  ULtUR() {
    pattern = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    duration = 200;
    m_name = "ULtUR";
  };
};

class BLtUR : public Pattern {
 public:
  BLtUR() {
    pattern = {{3, 0}, {2, 1}, {1, 2}, {0, 3}};
    duration = 200;
    m_name = "BLtUR";
  };
};

class Circ : public Pattern {
 public:
  Circ() {
    pattern = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3},
               {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}};
    duration = 200;
    m_name = "Circ";
  };
};

class CircSm : public Pattern {
 public:
  CircSm() {
    pattern = {
        {1, 1},
        {1, 2},
        {2, 2},
        {2, 1},
    };
    duration = 200;
    m_name = "CircSm";
  };
};

class CircGrow : public Pattern {
 public:
  CircGrow() {
    pattern = {{1, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 0}, {0, 1}, {0, 2}, {0, 3},
               {1, 3}, {2, 3}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}};
    duration = 200;
    m_name = "CircGrow";
  };
};

// Structure for passing information on the type of point to create
class Config {
 public:
  // Hand data
  HandTracking::LeapListening hand;

  float intensity{1};

  int frequency{256};

  Ultrahaptics::Vector3 leap_offset;

  float sine(Seconds t) {
    // return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5;
    return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5 * intensity;
  }

  // Ultrahaptics::Vector3 position{ 0, 0, 0 };
  int position = 0;

  RenderMode mode = RenderMode::DYNAMIC;
  Pattern pattern;

 private:
  const float cell_size_x = 15.f;  // in decimeter 20 = 2cm
  float cell_offset_x = cell_size_x / 2;
  float cell_max_x = cell_size_x + cell_offset_x;

  const float cell_size_z = 15.f;  // in decimeter 20 = 2cm
  float cell_offset_z = cell_size_z / 2;
  float cell_max_z = cell_size_z + cell_offset_z;

 public:
  Ultrahaptics::Vector3 offsets[cells][cells] = {
      {Vector3(-cell_max_x, 0.f, -cell_max_z),
       Vector3(-cell_offset_x, 0.f, -cell_max_z),
       Vector3(cell_offset_x, 0.f, -cell_max_z),
       Vector3(cell_max_x, 0.f, -cell_max_z)},
      {Vector3(-cell_max_x, 0.f, -cell_offset_z),
       Vector3(-cell_offset_x, 0.f, -cell_offset_z),
       Vector3(cell_offset_x, 0.f, -cell_offset_z),
       Vector3(cell_max_x, 0.f, -cell_offset_z)},
      {Vector3(-cell_max_x, 0.f, cell_offset_z),
       Vector3(-cell_offset_x, 0.f, cell_offset_z),
       Vector3(cell_offset_x, 0.f, cell_offset_z),
       Vector3(cell_max_x, 0.f, cell_offset_z)},
      {Vector3(-cell_max_x, 0.f, cell_max_z),
       Vector3(-cell_offset_x, 0.f, cell_max_z),
       Vector3(cell_offset_x, 0.f, cell_max_z),
       Vector3(cell_max_x, 0.f, cell_max_z)},
  };
};

// Callback function for filling out complete device output states through time
void my_emitter_callback(const StreamingEmitter& emitter,
                         OutputInterval& interval,
                         const LocalTimePoint& submission_deadline,
                         void* user_pointer) {
  // Cast the user pointer to the struct that describes the control point
  // behaviour
  Config* config = static_cast<Config*>(user_pointer);

  // Get a copy of the hand data.
  HandTracking::LeapOutput leapOutput = config->hand.getLeapOutput();

  // Loop through time, setting control point data
  for (TimePointOnOutputInterval& sample : interval) {
    if (!leapOutput.hand_present) {
      sample.controlPoint(0).setIntensity(0.0f);
      continue;
    }

    const Seconds t = sample - start_time;
    Ultrahaptics::Vector3 offset;
    if (config->mode == RenderMode::STATIC) {
      offset = config->leap_offset;
    } else if (config->mode == RenderMode::DYNAMIC) {
      offset = config->pattern.evaluate_at(t, config->offsets) +
               Vector3(-20.f, 0.f, 0.f);
    }
    Ultrahaptics::Vector3 position =
        leapOutput.palm_position +
        (offset * (leapOutput.hand_is_left ? -1 : 1));

    const float intensity = config->sine(t);

    // std::cout << position << std::endl;

    // fingers[0].bone(Leap::Bone::Type::TYPE_DISTAL).center();

    // Project the control point onto the palm
    sample.controlPoint(0).setPosition(position);

    // Set the intensity of the point using the waveform. If the hand is not
    // present, intensity is 0.
    sample.controlPoint(0).setIntensity(intensity);
  }
}

int entry(int argc, char* argv[]) {
#pragma region INIT_DEVICE
  // Create a Library object and connect it to a running service
  Library lib;
  auto found_library = lib.connect();

  /*lib.getLogging().setLogLevel(3);
  lib.getLogging().redirectLoggingToStdOut();*/

  if (!found_library) {
    std::cout << "Library failed to connect: "
              << found_library.error().message() << std::endl;
    return 1;
  }

  // Create a streaming emitter and add a suitable device to it
  StreamingEmitter emitter{lib};
  auto device = lib.findDevice(DeviceFeatures::StreamingHaptics);
  if (!device) {
    std::cout << "Failed to find device: " << device.error().message()
              << std::endl;
    return 1;
  }

  // If we found a device, get the default transform from Leap to Haptics device
  // space
  auto transform = device.value().getKitTransform();
  if (!transform) {
    std::cerr << "Unknown device transform: " << transform.error().message()
              << std::endl;
    return 1;
  }

  auto add_res = emitter.addDevice(device.value(), transform.value());
  if (!add_res) {
    std::cout << "Failed to add device: " << add_res.error().message()
              << std::endl;
    return 1;
  }
#pragma endregion INIT_DEVICE

#pragma region INIT_LEAP
  HandTracking::LeapController leap_control;

  // Create a structure containing our control point data and fill it in from
  // the file.
  Config point;

  point.leap_offset = Vector3(-20.f, 0.f, 0.f);

  leap_control.addListener(point.hand);
#pragma endregion INIT_LEAP

#pragma region INIT_EMITTER
  // Get the achievable sample_rate for the number of control points we are
  // producing
  auto cp_res = emitter.setControlPointCount(1, AdjustRate::All);
  if (!cp_res) {
    std::cout << "Failed to setControlPointCount: " << cp_res.error().message()
              << std::endl;
    return 1;
  }

  // Set the callback function to the callback written above
  auto ec_res = emitter.setEmissionCallback(my_emitter_callback, &point);
  if (!ec_res) {
    std::cout << "Failed to setEmissionCallback: " << ec_res.error().message()
              << std::endl;
    return 1;
  }

  // Start the array
  emitter.start();
#pragma endregion INIT_EMITTER

  // Wait for enter key to be pressed.
  std::cout << "Hit ENTER to quit..." << std::endl;
  std::cout << "Hit 5 and 6 to regulate frequency" << std::endl;
  std::cout << "Hit 7 and 8 to regulate intensity" << std::endl;
  // std::cout << "Hit 5 and 6 to regulate position" << std::endl;
  // std::cout << "Hit 7 and 8 to regulate position y" << std::endl;

  float intensity_step = 0.1f;
  float intensity_min = 0.f;
  float intensity_max = 1.f;
  float frequency_step = 1.f;
  float frequency_min = 1.f;
  float frequency_max = 10.f;

  int duration_step = 1;

  int pattern_current = 0;
  std::vector<Pattern> available_patterns = {
      Circ(), CircSm(), CircGrow(), ULtBR(), BLtBR(), BLtUR(), ULtUR()};

  while (true) {
    std::string key;
    key = _getch();
    // std::cout << key << std::endl;

    if (key == "\r") {
      break;
    }

    try {
      std::string action;
      float current;

      switch (Utils::hash(key.c_str())) {
        case Utils::hash("5"):
          current = std::clamp(log2f((float)point.frequency) - frequency_step,
                               frequency_min, frequency_max);
          point.frequency = (int)powf(2, current);
          action = "frequency lowered";
          break;
        case Utils::hash("6"):
          current = std::clamp(log2f((float)point.frequency) + frequency_step,
                               frequency_min, frequency_max);
          point.frequency = (int)powf(2, current);
          action = "frequency upped";
          break;
        case Utils::hash("7"):
          point.intensity = std::clamp(point.intensity - intensity_step,
                                       intensity_min, intensity_max);
          action = "intensity lowered";
          break;
        case Utils::hash("8"):
          point.intensity = std::clamp(point.intensity + intensity_step,
                                       intensity_min, intensity_max);
          action = "intensity upped";
          break;

        case Utils::hash("9"):
          point.mode = RenderMode::STATIC;
          action = "mode: static";
          break;
        case Utils::hash("0"):
          point.mode = RenderMode::DYNAMIC;
          action = "mode: dynamic";
          break;

        case Utils::hash("1"):
          point.leap_offset = point.offsets[0][0];
          action = key;
          break;
        case Utils::hash("2"):
          point.leap_offset = point.offsets[0][1];
          action = key;
          break;
        case Utils::hash("3"):
          point.leap_offset = point.offsets[0][2];
          action = key;
          break;
        case Utils::hash("4"):
          point.leap_offset = point.offsets[0][3];
          action = key;
          break;

        case Utils::hash("q"):
          point.leap_offset = point.offsets[1][0];
          action = key;
          break;
        case Utils::hash("w"):
          point.leap_offset = point.offsets[1][1];
          action = key;
          break;
        case Utils::hash("e"):
          point.leap_offset = point.offsets[1][2];
          action = key;
          break;
        case Utils::hash("r"):
          point.leap_offset = point.offsets[1][3];
          action = key;
          break;

        case Utils::hash("a"):
          point.leap_offset = point.offsets[2][0];
          action = key;
          break;
        case Utils::hash("s"):
          point.leap_offset = point.offsets[2][1];
          action = key;
          break;
        case Utils::hash("d"):
          point.leap_offset = point.offsets[2][2];
          action = key;
          break;
        case Utils::hash("f"):
          point.leap_offset = point.offsets[2][3];
          action = key;
          break;

        case Utils::hash("z"):
          point.leap_offset = point.offsets[3][0];
          action = key;
          break;
        case Utils::hash("x"):
          point.leap_offset = point.offsets[3][1];
          action = key;
          break;
        case Utils::hash("c"):
          point.leap_offset = point.offsets[3][2];
          action = key;
          break;
        case Utils::hash("v"):
          point.leap_offset = point.offsets[3][3];
          action = key;
          break;

        case Utils::hash("i"):
          pattern_current = std::clamp(pattern_current - 1, 0,
                                       (int)available_patterns.size() - 1);
          point.pattern = available_patterns[pattern_current];
          action = point.pattern.name();
          break;
        case Utils::hash("o"):
          pattern_current = std::clamp(pattern_current + 1, 0,
                                       (int)available_patterns.size() - 1);
          point.pattern = available_patterns[pattern_current];
          action = point.pattern.name();
          break;

        case Utils::hash("k"):
          current = (float)std::clamp(
              (int)log2(point.pattern.get_duration()) - duration_step,
              duration_step, 10);
          point.pattern.set_duration((int)powf(2, current));
          action = "duration -";
          break;
        case Utils::hash("l"):
          current = (float)std::clamp(
              (int)log2(point.pattern.get_duration()) + duration_step,
              duration_step, 10);
          point.pattern.set_duration((int)powf(2, current));
          action = "duration +";
          break;
        default:
          std::cout << "Command unknown: " << key << std::endl;
          break;
      }
      std::cout << "int: " << point.intensity << " | freq: " << point.frequency
                << " | off: " << point.leap_offset
                << " | dur: " << point.pattern.get_duration()
                << " | action: " << action << std::endl;
    } catch (const std::invalid_argument& e) {
      std::cerr << e.what() << ": " << key << std::endl;
    }
  }

  // Stop the array
  emitter.stop();

  return 0;
}
}  // namespace RandomWalk::MariannasParameterSpace