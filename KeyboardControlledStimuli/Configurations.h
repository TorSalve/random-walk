#pragma once

#include "Parameters.h"
#include "Utils.hpp"

namespace RandomWalk::Configurations {
using namespace Ultraleap::Haptics;
using namespace RandomWalk::Parameters;

using ConfigurationMap = std::map<std::string, Configuration*>;
class Configurations {
 public:
  Configurations() {
    std::vector<int> durations = {128, 256, 1024};
    std::vector<int> frequencies = {32, 128, 256, 512};
    std::vector<float> intensities = {0.4, 0.6, 0.8, 1};

    float width = 100.f;
    float height = 500.f;
    float width_f = 256.f;
    float height_f = 512.f;

    for (auto& d : durations) {
      for (auto& f : frequencies) {
        for (auto& i : intensities) {
          std::string key = "_d" + std::to_string(d) + "_f" +
                            std::to_string(f) + "_i" + std::to_string(i);

          configurations.insert(
              {"point" + key, new StaticPoint(i, f, d, std_offset)});

          configurations.insert(
              {"brush" + key, new Brush(i, f, d, std_offset, width, height,
                                        width_f, height_f)});

          configurations.insert(
              {"square" + key, new Square(i, f, d, Vector3({-20.f, 0.f, 20.f}),
                                          10, 10, {150, 150}, powf(2.f, 11.f),
                                          Square_Rendering::Random)});

          configurations.insert(
              {"ripple" + key,
               new Ripple(i, f, d, std_offset, 50, {50, 200}, 10.f)});
        }
      }
    }

    // configurations.insert({ "point", new StaticPoint(1.f, 256, 256,
    // std_offset) });
    /*float width = 100.f;
    float height = 500.f;
    float width_f = 256.f;
    float height_f = 512.f;
    configurations.insert(
        {"brush", new Brush(std_intensity, std_frequency, 100000, std_offset,
                            width, height, width_f, height_f)});*/

    /*configurations.insert(
        {"ripple", new Ripple(std_intensity, std_frequency, 100000, std_offset,
                              50, {50, 200}, 10.f)});*/

    /*configurations.insert(
        {"square_r", new Square(std_intensity, std_frequency, std_duration,
                                Vector3({-20.f, 0.f, 20.f}), 10, 10, {150, 150},
                                powf(2.f, 11.f), Square_Rendering::Random)});*/

    /*configurations.insert(
        {"square_rw",
         new Square(std_intensity, std_frequency, std_duration,
                    Vector3({-20.f, 0.f, 20.f}), 10, 10, {150, 150},
                    powf(2.f, 11.f), Square_Rendering::RowWise)});*/

    /*
    TESTING OLD THINGS

    configurations.insert({ "point", new StaticPoint(std_intensity,
    std_frequency, std_offset) }); configurations.insert({ "tracked_point",
    new TrackedPoint(std_intensity, std_frequency, std_offset,
    FingerIdx::INDEX, BoneIdx::TIP) });

    std::vector<std::tuple<FingerIdx, BoneIdx>> indicies = {
        { FingerIdx::INDEX, BoneIdx::TIP },
        { FingerIdx::INDEX, BoneIdx::ROOT }
    };
    configurations.insert({ "tracked_point_multiple", new
    TrackedPoint(std_intensity, std_frequency, std_offset, indicies, 124) });

     configurations.insert({ "circ", m_helper(MariannasParameterSpace::Circ(),
    std_intensity, std_frequency, std_duration) }); configurations.insert({
    "circ_sm", m_helper(MariannasParameterSpace::CircSm(), std_intensity,
    std_frequency, std_duration) });

     std::vector<int> durations = { 2, 128, 256, 1024 };
     std::vector<int> frequencies = { 2, 128, 256, 1024 };
     std::vector<float> intensities = { .2f, .4f, .6f, .8f, 1.f };
     std::vector<MariannasParameterSpace::Pattern> available_patterns = {
         MariannasParameterSpace::Circ(), MariannasParameterSpace::CircSm(),
    MariannasParameterSpace::CircGrow(), MariannasParameterSpace::ULtBR(),
    MariannasParameterSpace::BLtBR(), MariannasParameterSpace::BLtUR(),
    MariannasParameterSpace::ULtUR()
     };
     for (auto& p : available_patterns)
     {
         for (auto& d : durations)
         {
             for (auto& f : frequencies)
             {
                 for (auto& i : intensities)
                 {
                     std::string key = "M_" + p.name() + "_d" +
    std::to_string(d) + "_f" + std::to_string(f) + "_i" + std::to_string(i);
                     configurations.insert(std::make_pair(key,
    m_helper(*p.clone(), i, f)));
                 }
             }
         }
     }
     */
  }
  ConfigurationMap get_configurations() { return configurations; }
  Configuration* get_configuration(std::string key) {
    return configurations[key];
  }
  std::vector<std::string> get_keys(bool sorted = false) {
    std::vector<std::string> keys;
    for (auto const& cmap : configurations)
      keys.push_back(cmap.first);
    if (sorted) {
      std::sort(keys.begin(), keys.end());
    }
    return keys;
  }
  std::tuple<std::string, Configuration*> get_random(
      std::vector<std::string> exclude = {}) {
    std::sort(exclude.begin(), exclude.end());
    auto _keys = get_keys(true);
    std::vector<std::string> keys;
    std::set_difference(_keys.begin(), _keys.end(), exclude.begin(),
                        exclude.end(), std::back_inserter(keys));
    std::string r_key = *Utils::select_randomly(keys.begin(), keys.end());
    return std::make_tuple(r_key, configurations[r_key]);
  }
  int size() { return configurations.size(); }

 private:
  ConfigurationMap configurations;
  static int std_frequency;
  static float std_intensity;
  static float std_duration;
  static Vector3 std_offset;
  MariannasParameterSpace::Config* m_helper(
      MariannasParameterSpace::Pattern pattern,
      float intensity = std_intensity,
      int frequency = std_frequency,
      int duration = std_duration,
      Vector3 offset = std_offset,
      MariannasParameterSpace::RenderMode mode =
          MariannasParameterSpace::RenderMode::DYNAMIC) {
    pattern.duration(duration);
    return new MariannasParameterSpace::Config(intensity, frequency, offset,
                                               pattern, mode);
  }
};
}  // namespace RandomWalk::Configurations