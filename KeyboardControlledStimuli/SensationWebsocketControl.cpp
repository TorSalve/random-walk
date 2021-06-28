
#include <conio.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "easywsclient.hpp"
#include "json.hpp"

#include <cstdint>

#include <Leap.h>

#include "ultraleap/haptics/sensations.hpp"
#include "ultraleap/haptics/streaming.hpp"

#include "LeapHandConverter.hpp"

#include "Utils.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using json = nlohmann::json;

namespace RandomWalk::Sensations::Websockets {
using namespace Ultraleap::Haptics;
static easywsclient::WebSocket::pointer ws = NULL;

class FrameListener : public Leap::Listener {
 public:
  FrameListener(
      std::function<void(const Leap::Controller& controller)> callback)
      : callback(callback) {}
  void onFrame(const Leap::Controller& controller) override {
    callback(controller);
  }
  std::function<void(const Leap::Controller& controller)> callback;
};

int getch_noblock() {
  if (_kbhit())
    return _getch();
  else
    return -1;
}

std::vector<int> durations = {512, 1024, 2048, 4098};
std::vector<int> frequencies = {32, 128, 256, 512};
std::vector<float> intensities = {0.4, 0.6, 0.8, 1};

std::map<std::string, std::map<std::string, std::string>> translation = {
    {"RW.AmplitudeModulatedPoint",
     {{"frequency", "frequency"}, {"intensity", "maxIntensity"}}},
    {"RW.Brush", {{"frequency", "scanFrequency"}, {"intensity", "intensity"}}},
    {"RW.Ripple", {{"frequency", "intensity"}, {"intensity", "intensity"}}}};

int entry(int argc, char* argv[]) {
#pragma region START_DEVICE

  // Create a Library object and connect it to a running service
  Library lib;
  if (!lib.connect()) {
    std::cerr << "Could not connect to library" << std::endl;
    return 1;
  }

  // We need to acquire the transform from Tracking space to Ultraleap space
  Ultrahaptics::Transform tracking_transform;

  // Create a streaming emitter and add a device to it
  SensationEmitter emitter{lib};
  if (auto device = lib.findDevice(DeviceFeatures::StreamingHaptics)) {
    result<Transform> transform = device.value().getKitTransform();
    if (!transform) {
      std::cerr << "Unknown device transform" << std::endl;
      return 1;
    }
    tracking_transform = transform.value();

    // Start a device but do not add a Transform for Sensations
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
#pragma endregion

#pragma region INIT_WS
  const std::string ws_url = "ws://localhost:8081/";
  ws = easywsclient::WebSocket::from_url(ws_url);
#pragma endregion INIT_WS

  // Load the sensation package
  const auto sensation_package_result =
      SensationPackage::loadFromFile(lib, "StandardSensations.ssp");
  if (!sensation_package_result) {
    std::cerr << "Unknown package" << std::endl;
    return 1;
  }
  const auto& sensation_package = sensation_package_result.value();

  typedef std::map<std::string, float> parameters;
  typedef std::map<std::string, std::tuple<std::string, parameters>> sensations;
  sensations _sensations;

  std::ifstream fj("Sensations.json");
  json jsensations;
  fj >> jsensations;

  std::string sensations_key = "sensations";
  std::string shared_params_key = "shared_params";
  std::string sensation_name_key = "sensation";
  std::string sensation_shared_params_key = "shared_params";
  std::string sensation_params_key = "params";

  std::map<std::string, std::map<std::string, std::vector<float>>> iterators;
  if (jsensations.contains(sensations_key)) {
    for (auto& jsensation : jsensations[sensations_key]) {
      parameters parameters;

      std::string sensation_name = jsensation[sensation_name_key];
      Utils::replace(sensation_name, "\"", "");

      // insert shared parameters
      if (jsensation.contains(sensation_shared_params_key)) {
        auto curr_sensation_it = iterators.find(jsensation[sensation_name_key]);
        std::map<std::string, std::vector<float>> curr_sensation = {};
        if (curr_sensation_it != iterators.end()) {
          curr_sensation = curr_sensation_it->second;
        }

        auto o = jsensation[sensation_shared_params_key];
        for (json::iterator it = o.begin(); it != o.end(); ++it) {
          if (jsensations[shared_params_key].contains(it.key())) {
            auto value = it.value().dump();
            if (it.value().is_string()) {
              Utils::replace(value, "\"", "");
            } else {
              continue;
            }
            auto curr_param_it = curr_sensation.find(value);
            std::vector<float> curr_params;
            if (curr_param_it != curr_sensation.end()) {
              curr_params = curr_param_it->second;
            }

            auto values = jsensations[shared_params_key][it.key()];
            curr_params.insert(curr_params.end(), values.begin(), values.end());
            curr_sensation.insert({value, curr_params});
          }
        }
        iterators[sensation_name] = curr_sensation;
      }

      if (jsensation.contains(sensation_params_key)) {
        auto curr_sensation_it = iterators.find(jsensation[sensation_name_key]);
        std::map<std::string, std::vector<float>> curr_sensation = {};
        if (curr_sensation_it != iterators.end()) {
          curr_sensation = curr_sensation_it->second;
        }

        auto o = jsensation[sensation_params_key];
        for (json::iterator it = o.begin(); it != o.end(); ++it) {
          auto curr_param_it = curr_sensation.find(it.key());
          std::vector<float> curr_params;
          if (curr_param_it != curr_sensation.end()) {
            curr_params = curr_param_it->second;
          }
          auto values = it.value();
          if (!values.is_array()) {
            values = std::vector<float>{values};
          }

          curr_params.insert(curr_params.end(), values.begin(), values.end());
          curr_sensation.insert({it.key(), curr_params});
        }
        iterators[sensation_name] = curr_sensation;
      }
    }
    for (auto sensation : iterators) {
      std::string sensation_name = sensation.first;

      auto keys = Utils::map_get_keys(sensation.second);
      auto values = Utils::map_get_values(sensation.second);

      // number of arrays
      int n = sensation.second.size();

      // to keep track of next element in each of
      // the n arrays
      int* indices = new int[n];

      // initialize with first element's index
      for (int i = 0; i < n; i++)
        indices[i] = 0;

      while (1) {
        // print current combination
        std::string k = sensation_name;
        parameters params;
        for (int i = 0; i < n; i++) {
          auto key = keys[i];
          auto value = values[i][indices[i]];
          k += "_" + key[0] + std::to_string(value);
          params.insert({key, value});
        }
        _sensations.insert({k, {sensation_name, params}});

        // find the rightmost array that has more
        // elements left after the current element
        // in that array
        int next = n - 1;
        while (next >= 0 && (indices[next] + 1 >= values[next].size()))
          next--;

        // no such array is found so no more
        // combinations left
        if (next < 0)
          break;

        // if found move to next element in that
        // array
        indices[next]++;

        // for all arrays to the right of this
        // array current index again points to
        // first element
        for (int i = next + 1; i < n; i++)
          indices[i] = 0;
      }
    }
  }
  return 0;

  //// for (auto& d : durations) {
  // for (auto& i : intensities) {
  //  for (auto& f : frequencies) {
  //    for (std::string sn : {"RW.AmplitudeModulatedPoint", "RW.Ripple"}) {
  //      std::string k = sn + "_" + std::to_string(i) + "_" +
  //      std::to_string(f);
  //      // + "_" + std::to_string(d);
  //      parameters params;
  //      auto fn = translation[sn]["frequency"];
  //      auto in = translation[sn]["intensity"];
  //      params.insert({fn, f});
  //      params.insert({in, i});
  //      // params.insert({"duration", f});
  //      sensations.insert({k, {sn, params}});
  //    }

  //    std::string sn = "RW.Large";
  //    std::string k = sn + "_" + std::to_string(i) + "_" + std::to_string(f);
  //    parameters all_over_params = {
  //        {"scanLength", 0.075f},
  //        //{"duration", d},
  //        {"lineLength", 0.125f},
  //        {"lineFrequency", 125},
  //        {"scanFrequency", 125},
  //        {"intensity", i},
  //        {"frequency", f},
  //    };
  //    sensations.insert({k, {sn, all_over_params}});
  //  }

  //  for (auto& f : {0.5f, 1.f, 2.f, 4.f}) {
  //    std::string sn = "RW.Brush";
  //    std::string k = sn + "_" + std::to_string(i) + "_" + std::to_string(f);
  //    // + "_" + std::to_string(d);
  //    parameters params;
  //    auto fn = translation[sn]["frequency"];
  //    auto in = translation[sn]["intensity"];
  //    params.insert({fn, f});
  //    params.insert({in, i});
  //    // params.insert({"duration", f});
  //    sensations.insert({k, {sn, params}});
  //  }
  //}

  //}

  auto setSensation = [&](std::string s_key, bool notify) {
    std::cout << "now playing: " << s_key << std::endl;
    auto sensation_t = _sensations[s_key];
    auto sensation = std::get<0>(sensation_t);
    auto params = std::get<1>(sensation_t);
    const auto hand_tracked_sensation = sensation_package.sensation(sensation);
    if (!hand_tracked_sensation) {
      throw "Unknown sensation";
    }

    // Created an instance of the Sensation
    auto sensation_instance = SensationInstance(hand_tracked_sensation.value());

    auto p_keys = Utils::map_get_keys(params);
    for (auto& parameter : p_keys) {
      if (parameter != "duration") {
        sensation_instance.set(parameter, params[parameter]);
      }
    }

    // Set the instance on the emitter
    const auto set_result = emitter.setSensation(sensation_instance);
    if (!set_result) {
      throw "Could not set sensation";
    }

    // auto duration = params["duration"];
    // std::this_thread::sleep_for(std::chrono::milliseconds((int)duration));
    // emitter.pause();

    if (notify) {
      json j = {
          {"name", sensation},
          {"intensity", params[translation[sensation]["intensity"]]},
          {"frequency", params[translation[sensation]["frequency"]]},
          //{"duration", params["duration"]}
      };
      ws->send("stm" + j.dump());
    }

    return sensation_instance;
  };

  // Load the data of the named Sensation
  auto sensation_keys = Utils::map_get_keys(_sensations);
  std::shuffle(sensation_keys.begin(), sensation_keys.end(),
               std::default_random_engine(42));
  int idx = 0;
  try {
    SensationInstance sensation_instance =
        setSensation(sensation_keys[idx], true);

    // Set up Leap
    Leap::Controller leap_control;
    leap_control.setPolicyFlags(
        Leap::Controller::PolicyFlag::POLICY_BACKGROUND_FRAMES);

    // Set up the Leap Frame callback
    LeapHandConverter hand_converter(tracking_transform);
    auto on_frame_callback = [&](const Leap::Controller& controller) {
      const Leap::HandList hands = leap_control.frame().hands();
      if (!hands.isEmpty()) {
        Leap::Hand hand = hands[0];
        sensation_instance.set("hand",
                               hand_converter.toElementSimpleHand(hand));
      } else {
        sensation_instance.set("hand",
                               LeapHandConverter::invalidElementSimpleHand());
      }
      emitter.updateSensationArguments(sensation_instance);
    };

    FrameListener frame_listener = FrameListener(on_frame_callback);
    leap_control.addListener(frame_listener);

    // Start the emitter
    const auto emitter_result = emitter.start();
    if (!emitter_result)
      return 2;

    std::cout << "Hit ENTER to quit..." << std::endl;
    bool advance_with_websocket = false;
    if (advance_with_websocket) {
      while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
        ws->poll();
        ws->dispatch([&](const std::string& message) mutable {
          if (message == "\"stmnext\"") {
            if (emitter.isPaused().value()) {
              emitter.resume();
            }

            idx += 1;
            if (idx >= sensation_keys.size()) {
              idx = 0;
              std::cout << "end reached ---------------------" << std::endl;
            }

            sensation_instance = setSensation(sensation_keys[idx], true);
          }
        });

        int key = getch_noblock();
        // enter
        if (key == 13) {
          break;
        }
      }
    } else {
      while (true) {
        int key = getch_noblock();

        // enter
        if (key == 13) {
          break;
        }

        // space or q
        if (key == 32 || key == 113) {
          if (emitter.isPaused().value()) {
            emitter.resume();
          }

          if (key != 32) {
            idx += 1;
          }
          if (idx >= sensation_keys.size()) {
            idx = 0;
            std::cout << "end reached ---------------------" << std::endl;
          }

          sensation_instance = setSensation(sensation_keys[idx], false);
        }

        /*switch (Utils::hash(key.c_str())) {
          case Utils::hash("q"):
          default:
            std::cout << "Command unknown: " << key << std::endl;
            break;
        }*/
      }
    }

    // Stop the array
    emitter.stop();

    return 0;
  } catch (const std::exception& excpt) {
    std::cout << excpt.what() << std::endl;
    return 4;
  }
}
}  // namespace RandomWalk::Sensations::Websockets