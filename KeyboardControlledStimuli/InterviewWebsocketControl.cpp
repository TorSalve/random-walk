
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

#include <boost/asio.hpp>

#include "easywsclient.hpp"
#include "json.hpp"

#include <cstdint>

#include <Leap.h>

#include "ultraleap/haptics/sensations.hpp"
#include "ultraleap/haptics/streaming.hpp"

#include "LeapHandConverter.hpp"

#include "Timer.hpp"
#include "Utils.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using json = nlohmann::json;

namespace RandomWalk::Interview::Websockets {

const bool randomize = true;
const bool advance_with_websocket = true;
const int repetitions = 1;

// const std::string sensation_configuration = "SensationConfigs/Test.json";
// const std::string sensation_configuration = "SensationConfigs/Study1.json";
const std::string sensation_configuration = "SensationConfigs/Study2.json";

using namespace Ultraleap::Haptics;
static easywsclient::WebSocket::pointer ws = NULL;

// #0 parameter name, #1 parameter value
typedef std::map<std::string, float> parameters;
// #0 sensation name, #1 sensation key, #2 parameters
typedef std::tuple<std::string, std::string, parameters> sensation;
// #0 sensation identifier, #1 sensation
typedef std::map<std::string, sensation> sensations;
typedef std::vector<float> sensation_value;
// #0 sensation name, #1 sensation key
typedef std::map<std::string, sensation_value> sensation_values;

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
  if (advance_with_websocket) {
    const std::string ws_url = "ws://localhost:8081/";
    ws = easywsclient::WebSocket::from_url(ws_url);
  }
#pragma endregion

#pragma region LOAD_SENSATIONS_PACKAGE
  // Load the sensation package
  const auto sensation_package_result =
      SensationPackage::loadFromFile(lib, "StandardSensations.ssp");
  if (!sensation_package_result) {
    std::cerr << "Unknown package" << std::endl;
    return 1;
  }
  const auto& sensation_package = sensation_package_result.value();
#pragma endregion

#pragma region PARSE_SENSATION_CONFIG

  sensations _sensations;

  json jsensations;
  try {
    std::ifstream fj(sensation_configuration);
    try {
      fj >> jsensations;
    } catch (const std::exception&) {
      std::cerr << "Failed to parse sensations JSON" << std::endl;
      return 1;
    }
  } catch (const std::exception&) {
    std::cerr << "Failed to load sensations JSON" << std::endl;
    return 1;
  }

  std::string sensations_key = "sensations";
  std::string shared_params_key = "shared_params";
  std::string sensation_key = "sensation";
  std::string sensation_name_key = "name";
  std::string sensation_shared_params_key = "shared_params";
  std::string sensation_params_key = "params";

  std::map<std::tuple<std::string, std::string>, sensation_values> iterators;
  if (jsensations.contains(sensations_key)) {
    for (auto& jsensation : jsensations[sensations_key]) {
      parameters parameters;

      std::string sensation_id = jsensation[sensation_key];
      Utils::replace(sensation_id, "\"", "");

      std::string sensation_name = sensation_id;
      if (jsensation.contains(sensation_name_key)) {
        sensation_name = jsensation[sensation_name_key];
        Utils::replace(sensation_name, "\"", "");
      }

      std::tuple<std::string, std::string> id = {sensation_name, sensation_id};

      // insert shared parameters
      if (jsensation.contains(sensation_shared_params_key)) {
        auto curr_sensation_it = iterators.find(id);
        sensation_values curr_sensation = {};
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
            sensation_value curr_params;
            if (curr_param_it != curr_sensation.end()) {
              curr_params = curr_param_it->second;
            }

            auto values = jsensations[shared_params_key][it.key()];
            curr_params.insert(curr_params.end(), values.begin(), values.end());
            curr_sensation.insert({value, curr_params});
          }
        }
        iterators[id] = curr_sensation;
      }

      if (jsensation.contains(sensation_params_key)) {
        auto curr_sensation_it = iterators.find(id);
        sensation_values curr_sensation;
        if (curr_sensation_it != iterators.end()) {
          curr_sensation = curr_sensation_it->second;
        }

        auto o = jsensation[sensation_params_key];
        for (json::iterator it = o.begin(); it != o.end(); ++it) {
          sensation_value curr_params;
          auto curr_param_it = curr_sensation.find(it.key());
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
        iterators[id] = curr_sensation;
      }

      if (!jsensation.contains(sensation_shared_params_key) &&
          !jsensation.contains(sensation_params_key)) {
        iterators[id] = {};
      }
    }

    for (auto sensation : iterators) {
      std::string sensation_name = std::get<0>(sensation.first);
      std::string sensation_id = std::get<1>(sensation.first);

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
        std::string k = sensation_name + "_" + sensation_id;
        parameters params;
        for (int i = 0; i < n; i++) {
          auto key = keys[i];
          auto value = values[i][indices[i]];
          std::stringstream stream;
          stream << std::fixed << std::setprecision(3) << value;
          std::string s = stream.str();
          k = k + "_" + key.substr(0, 2) + s;
          params.insert({key, value});
        }
        _sensations.insert({k, {sensation_name, sensation_id, params}});

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
#pragma endregion

#pragma region RANDOMIZATION

  // Load the data of the named Sensation
  auto sensation_keys = Utils::map_get_keys(_sensations);

  auto shuffle_keys = [&]() {
    if (randomize) {
      std::shuffle(sensation_keys.begin(), sensation_keys.end(),
                   std::default_random_engine(time(NULL)));
    }
  };
  shuffle_keys();
#pragma endregion

  int current_repetition = 0;
  int idx = -1;

  auto setSensation = [&](std::string _sensation_id, sensation _sensation,
                          bool notify = false, bool play = true) {
    if (!emitter.isPaused().value()) {
      emitter.pause();
    }

    // auto sensation_t = _sensations[s_key];
    std::string sensation_name = std::get<0>(_sensation);
    std::string sensation_id = std::get<1>(_sensation);
    parameters params = std::get<2>(_sensation);

    const auto hand_tracked_sensation =
        sensation_package.sensation(sensation_id);
    if (!hand_tracked_sensation) {
      throw "Unknown sensation";
    }

    std::cout << "now playing: " << sensation_name << std::endl;

    // Created an instance of the Sensation
    auto sensation_instance = SensationInstance(hand_tracked_sensation.value());

    auto p_keys = Utils::map_get_keys(params);
    auto excluded_keys = {"duration", "meta_frequency"};
    for (auto& parameter : p_keys) {
      if (std::find(excluded_keys.begin(), excluded_keys.end(), parameter) ==
          excluded_keys.end()) {
        sensation_instance.set(parameter, params[parameter]);
      }
    }

    // Set the instance on the emitter
    const auto set_result = emitter.setSensation(sensation_instance);
    if (!set_result) {
      throw "Could not set sensation";
    }
    emitter.updateSensationArguments(sensation_instance);
    try {
      emitter.resume();
    } catch (const std::exception&) {
    }

    auto duration_it = params.find("duration");
    if (duration_it != params.end()) {
      float duration = duration_it->second;

      auto meta_frequency_it = params.find("meta_frequency");
      if (meta_frequency_it != params.end() && play) {
        float meta_frequency = meta_frequency_it->second;
        int frac = (int)duration / meta_frequency;

        auto get_now = []() {
          return (std::chrono::system_clock::now().time_since_epoch() /
                  std::chrono::milliseconds(1));
        };

        auto toggle_emitter = [&]() {
          if (emitter.isPaused().value()) {
            emitter.resume();
          } else {
            emitter.pause();
          }
        };

        Time::Timer t = Time::Timer();

        std::cout << "start playing \t\t" << get_now() << std::endl;
        t.setInterval(toggle_emitter, frac);
        t.setTimeout(
            [&]() {
              t.stop();
              emitter.clearSensation();
              std::cout << "finished playing \t" << get_now() << std::endl
                        << "-" << std::endl;
              if (advance_with_websocket) {
                ws->send("stmfinishedplaying");
              }

              // check for end?
              std::cout << "idx: " << idx
                        << " senskeys:" << sensation_keys.size() << std::endl;
              if (idx + 1 >= sensation_keys.size()) {
                current_repetition += 1;
                std::cout << "end reached ---------------------" << std::endl;

                if (current_repetition >= repetitions) {
                  std::cout << "no more repetitions -------------" << std::endl;
                  if (advance_with_websocket) {
                    ws->send("stmend");
                  }
                }
                // shuffle_keys();
              }
            },
            duration);
      }
    }

    if (notify && advance_with_websocket) {
      json jsensation;
      jsensation["name"] = sensation_name;
      jsensation["sensation"] = sensation_id;
      jsensation["id"] = _sensation_id;

      for (auto param : params) {
        jsensation[param.first] = param.second;
      }
      ws->send("stm" + jsensation.dump());
    }

    return sensation_instance;
  };

  std::cout << "Running interview configuration." << std::endl;
  std::cout << "Hit ENTER to quit..." << std::endl;
  if (!advance_with_websocket) {
    std::cout << "Hit q to switch sensation..." << std::endl;
  }

  // Utils::print_element(sensation_keys);
  std::cout << sensation_keys.size() << " combinations, " << repetitions
            << " repetitions, " << sensation_keys.size() * repetitions
            << " total trials" << std::endl;

  try {
    sensation training_sensation = {"training_sensation",
                                    "RW.AmplitudeModulatedPoint",
                                    {{"maxIntensity", 1}, {"frequency", 250}}};
    SensationInstance sensation_instance =
        setSensation("", training_sensation, false, false);
#pragma region LEAP_SETUP

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
#pragma endregion

    // Start the emitter
    const auto emitter_result = emitter.start();
    if (!emitter_result)
      return 2;
      // emitter.pause();

#pragma region SENSATION_LOOP
    auto nextSensation = [&](bool advance = true) {
      if (advance) {
        idx += 1;
      }

      std::cout << "idx: " << idx << std::endl;

      if (idx < sensation_keys.size()) {
        std::cout << idx << "/" << sensation_keys.size() << std::endl;
        std::string current_sensation = sensation_keys[idx];
        sensation_instance = setSensation(
            current_sensation, _sensations[current_sensation], advance);
      }
    };

    if (advance_with_websocket) {
      while (ws->getReadyState() != easywsclient::WebSocket::CLOSED) {
        ws->poll();
        ws->dispatch([&](const std::string& message) mutable {
          if (message == "\"sdc\"") {
            emitter.pause();
          }
          bool next = message == "\"stmnext\"";
          bool replay = message == "\"stmreplay\"";
          if (next || replay) {
            nextSensation(next);
          }
        });

        if (idx >= static_cast<int>(sensation_keys.size())) {
          break;
        }

        int key = getch_noblock();
        // enter
        if (key == 13) {
          break;
        }
      }
    } else {
      while (true) {
        if (idx >= static_cast<int>(sensation_keys.size())) {
          break;
        }

        int key = getch_noblock();

        // q
        if (key == 113) {
          nextSensation();
        }

        // enter
        if (key == 13) {
          break;
        }
      }
    }

    // Stop the array
    emitter.stop();

    return 0;
  } catch (const std::exception& excpt) {
    std::cout << excpt.what() << std::endl;
    return 4;
  }
#pragma endregion
}
}  // namespace RandomWalk::Interview::Websockets