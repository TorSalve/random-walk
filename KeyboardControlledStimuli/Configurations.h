#pragma once

#include "Parameters.h"
#include "Utils.h"


namespace RandomWalk::Configurations {
    using namespace Ultraleap::Haptics;
    using namespace RandomWalk::Parameters;

    using ConfigurationMap = std::map<std::string, Configuration*>;
    class Configurations {
    public:
        Configurations() {
            /* configurations.insert({ "point", new StaticPoint(std_intensity, std_frequency, std_offset) });
            configurations.insert({ "tracked_point", new TrackedPoint(std_intensity, std_frequency, std_offset, FingerIdx::INDEX, BoneIdx::TIP) });

            std::vector<std::tuple<FingerIdx, BoneIdx>> indicies = {
                { FingerIdx::INDEX, BoneIdx::TIP },
                { FingerIdx::INDEX, BoneIdx::ROOT }
            };
            configurations.insert({ "tracked_point_multiple", new TrackedPoint(std_intensity, std_frequency, std_offset, indicies, 124) });

             configurations.insert({ "circ", m_helper(MariannasParameterSpace::Circ(), std_intensity, std_frequency, std_duration) });
             configurations.insert({ "circ_sm", m_helper(MariannasParameterSpace::CircSm(), std_intensity, std_frequency, std_duration) });

             std::vector<int> durations = { 2, 128, 256, 1024 };
             std::vector<int> frequencies = { 2, 128, 256, 1024 };
             std::vector<float> intensities = { .2f, .4f, .6f, .8f, 1.f };
             std::vector<MariannasParameterSpace::Pattern> available_patterns = {
                 MariannasParameterSpace::Circ(), MariannasParameterSpace::CircSm(), MariannasParameterSpace::CircGrow(), MariannasParameterSpace::ULtBR(), MariannasParameterSpace::BLtBR(), MariannasParameterSpace::BLtUR(), MariannasParameterSpace::ULtUR()
             };
             for (auto& p : available_patterns)
             {
                 for (auto& d : durations)
                 {
                     for (auto& f : frequencies)
                     {
                         for (auto& i : intensities)
                         {
                             std::string key = "M_" + p.name() + "_d" + std::to_string(d) + "_f" + std::to_string(f) + "_i" + std::to_string(i);
                             configurations.insert(std::make_pair(key, m_helper(*p.clone(), i, f)));
                         }
                     }
                 }
             }
             */
        }
        ConfigurationMap get_configurations() {
            return configurations;
        }
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
        std::tuple<std::string, Configuration*> get_random(std::vector<std::string> exclude = {}) {
            std::sort(exclude.begin(), exclude.end());
            auto _keys = get_keys(true);
            std::vector<std::string> keys;
            std::set_difference(_keys.begin(), _keys.end(), exclude.begin(), exclude.end(), std::back_inserter(keys));
            std::string r_key = *Utils::select_randomly(keys.begin(), keys.end());
            return std::make_tuple(r_key, configurations[r_key]);
        }
        int size() {
            return configurations.size();
        }
    private:
        ConfigurationMap configurations;
        static int std_frequency;
        static float std_intensity;
        static int std_duration;
        static Vector3 std_offset;
        MariannasParameterSpace::Config* m_helper(
            MariannasParameterSpace::Pattern pattern,
            float intensity = std_intensity, int frequency = std_frequency, int duration = std_duration, Vector3 offset = std_offset,
            MariannasParameterSpace::RenderMode mode = MariannasParameterSpace::RenderMode::DYNAMIC
        ) {
            pattern.duration(duration);
            return new MariannasParameterSpace::Config(intensity, frequency, offset, pattern, mode);
        }
    };
}