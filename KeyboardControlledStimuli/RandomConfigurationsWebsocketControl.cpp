#include <conio.h>
#include <iostream>
#include <map>
#include <string>
#include <ultraleap/haptics/streaming.hpp>
#include <utility>
#include <vector>

#include "easywsclient.hpp"

#include "Parameters.h"
#include "Utils.hpp"

#include "Configurations.h"

using namespace Ultraleap::Haptics;

using Seconds = std::chrono::duration<float>;

static auto start_time = std::chrono::steady_clock::now();

static easywsclient::WebSocket::pointer ws = NULL;

namespace RandomWalk::Parameters::Websockets {
// Callback function for filling out complete device output states through time
void emitter_callback(const StreamingEmitter& emitter,
                      OutputInterval& interval,
                      const LocalTimePoint& submission_deadline,
                      void* user_pointer) {
  // Cast the user pointer to the struct that describes the control point
  // behaviour
  Configuration* config = static_cast<Configuration*>(user_pointer);

  // Get a copy of the hand data.
  HandTracking::LeapOutput leapOutput = config->hand.getLeapOutput();
  config->pre_hook(&leapOutput, emitter.getCallbackRate());

  // Loop through time, setting control point data
  for (TimePointOnOutputInterval& sample : interval) {
    if (!leapOutput.hand_present) {
      sample.controlPoint(0).setIntensity(0.0f);
      config->reset_playtime();
      continue;
    }

    const Seconds t = sample - start_time;
    Ultrahaptics::Vector3 offset = config->evaluate_position(t, &leapOutput);
    Ultrahaptics::Vector3 position;
    if (config->palm_position()) {
      position = leapOutput.palm_position;
    }
    position += (offset * (leapOutput.hand_is_left ? -1.f : 1.f));

    const float intensity = config->evaluate_intensity(t);

    // Project the control point onto the palm
    sample.controlPoint(0).setPosition(position);

    // Set the intensity of the point using the waveform. If the hand is not
    // present, intensity is 0.
    sample.controlPoint(0).setIntensity(intensity);
  }
}

void print_message(const std::string& message) {
  printf(">>> %s\n", message.c_str());
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

  // Get the achievable sample_rate for the number of control points we are
  // producing
  auto cp_res = emitter.setControlPointCount(1, AdjustRate::All);
  if (!cp_res) {
    std::cout << "Failed to setControlPointCount: " << cp_res.error().message()
              << std::endl;
    return 1;
  }
#pragma endregion

#pragma region INIT_WS
  const std::string ws_url = "ws://localhost:8081/";
  ws = easywsclient::WebSocket::from_url(ws_url);
#pragma endregion

  std::cout << "Hit ENTER to quit..." << std::endl;
  // std::cout << "Hit 5 and 6 to regulate frequency" << std::endl;
  // std::cout << "Hit 7 and 8 to regulate intensity" << std::endl;
  // std::cout << "Hit 5 and 6 to regulate position" << std::endl;
  // std::cout << "Hit 7 and 8 to regulate position y" << std::endl;

  HandTracking::LeapController leap_control;
  Configurations::Configurations configurations;

  std::vector<std::string> already_applied = {};
  already_applied.reserve(configurations.size());

  Configuration* point;

  auto next_configuration = [configurations, &already_applied, leap_control,
                             &emitter, point]() mutable {
    // Create a structure containing our control point data and fill it in from
    // the file.
    std::tuple<std::string, Configuration*> config =
        configurations.get_random(already_applied);
    auto m_key = std::get<std::string>(config);
    already_applied.push_back(m_key);
    std::cout << "Now playing: " << m_key << std::endl;
    point = std::get<Configuration*>(config);

    leap_control.addListener(point->hand);

    // Set the callback function to the callback written above
    auto ec_res = emitter.setEmissionCallback(emitter_callback, point);
    if (!ec_res) {
      std::cout << "Failed to setEmissionCallback: " << ec_res.error().message()
                << std::endl;
      return 1;
    }

    std::cout << point->to_json() << std::endl;
    Utils::print_element(already_applied);
    ws->send("stm" + point->to_json());

    if (configurations.size() == already_applied.size()) {
      already_applied = {};
      std::cout << "Restart from the beginning --------------- " << std::endl;
    }
    return 0;
  };
  int ok = next_configuration();
  if (ok > 0) {
    return ok;
  }

  // Start the array
  emitter.start();

  // Wait for enter key to be pressed.
  while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
    ws->poll();
    ws->dispatch(
        [next_configuration, &emitter](const std::string& message) mutable {
          if (message == "\"stmnext\"") {
            emitter.pause();

            int ok = next_configuration();
            if (ok > 0) {
              return ok;
            }
            emitter.resume();
          }
        });
  }

  // Stop the array
  emitter.stop();

  return 0;
}
}  // namespace RandomWalk::Parameters::Websockets