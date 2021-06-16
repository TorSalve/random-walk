#pragma once

#include <map>
#include <chrono>
#include "json.hpp"

#include "HandTracking.h"

using Seconds = std::chrono::duration<float>;

using json = nlohmann::json;

namespace RandomWalk::Parameters {

    class Configuration {
    public:
        Configuration(float intensity = 1.f, int frequency = 256) : _intensity(intensity), _frequency(frequency) {}
        HandTracking::LeapListening hand;
    private:
        float _intensity;
        int _frequency;
    protected:
        bool _palm_position = true;
        float sine(Seconds t) {
            // return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5;
            return (1.0 - std::cos(2 * M_PI * frequency() * t.count())) * 0.5 * intensity();
        }
    public:
        virtual float evaluate_intensity(Seconds t) {
            return sine(t);
        }
        virtual Ultrahaptics::Vector3 evaluate_position(Seconds t) = 0;
        virtual Ultrahaptics::Vector3 evaluate_position(Seconds t, HandTracking::LeapOutput* leapOutput) = 0;

        virtual std::string to_json() = 0;
        
        const bool& palm_position() const { return _palm_position; }
        const float& intensity() const { return _intensity; }
        void intensity(const float& intensity) { _intensity = intensity; }
        const int& frequency() const { return _frequency; }
        void frequency(const int& frequency) { _frequency = frequency; }
    };

    class Point : public Configuration {
    public:
        Point(float intensity, int frequency, Ultrahaptics::Vector3 offset) : Configuration(intensity, frequency), _offset(offset) { }
    private:
        Ultrahaptics::Vector3 _offset;
    public:
        Ultrahaptics::Vector3 evaluate_position(Seconds t) override {
            return offset();
        };

        const Ultrahaptics::Vector3& offset() const { return _offset; }
        void offset(const Ultrahaptics::Vector3& offset) { _offset = offset; }
    };

    class StaticPoint : public Point {
    public:
        StaticPoint(float intensity, int frequency, Ultrahaptics::Vector3 offset) : Point(intensity, frequency, offset) { }
        Ultrahaptics::Vector3 evaluate_position(Seconds t, HandTracking::LeapOutput* leapOutput) override {
            return Point::evaluate_position(t);
        };
        std::string to_json() override {
            json j = {
                {"name", "Point"},
                {"intensity", intensity()},
                {"frequency", frequency()},
                {"offset.x", offset().x},
                {"offset.y", offset().y},
                {"offset.z", offset().z},
            };
            return j.dump();
        }
    };

    enum class FingerIdx
    {
        THUMB = 0,
        INDEX = 1,
        MIDDLE = 2,
        RING = 3,
        PINKY = 4,
    };

    enum class BoneIdx {
        ROOT = 0,
        PROXIMAL = 1,
        INTERMEDIATE = 2,
        TIP = 3,
    };

    class TrackedPoint : public Point {
    public:
        TrackedPoint(float intensity, int frequency, Ultrahaptics::Vector3 offset, FingerIdx fidx, BoneIdx bidx) : Point(intensity, frequency, offset) { 
            _indicies.emplace_back(fidx, bidx);
            _palm_position = false;
        }
        TrackedPoint(float intensity, int frequency, Ultrahaptics::Vector3 offset, std::vector<std::tuple<FingerIdx, BoneIdx>> indicies, float duration) : Point(intensity, frequency, offset), _indicies(indicies), _duration(duration) {
            _palm_position = false;
        }
        void duration(const int& duration) { _duration = duration; }
        const int& duration() const { return _duration; }
        Ultrahaptics::Vector3 evaluate_position(Seconds t, HandTracking::LeapOutput* leapOutput) override {
            float s = t.count();
            float ms = s * 1000;
            int len = _indicies.size();
            int idx = (int)floor(ms / duration()) % len;

            auto finger_bone = _indicies[idx];
            auto tracking = HandTracking::translate_finger_output(leapOutput);

            return Point::evaluate_position(t) + tracking[(int)std::get<0>(finger_bone)][(int)std::get<1>(finger_bone)];
        };
        std::string to_json() override {
            json j = {
                {"name", "TrackedPoint"},
                {"intensity", intensity()},
                {"frequency", frequency()},
                {"offset.x", offset().x},
                {"offset.y", offset().y},
                {"offset.z", offset().z},
                {"duration", duration()},
            };
            for (size_t i = 0; i < _indicies.size(); i++)
            {
                auto t = _indicies[i];
                auto f = std::get<0>(t);
                auto b = std::get<1>(t);
                j["finger_bone." + std::to_string(i)] = to_string(f) + "_" + to_string(b);
            }
            return j.dump();;
        }
    private:
        static std::string to_string(FingerIdx fid) {
            std::map<FingerIdx, std::string> translation = {
                { FingerIdx::THUMB, "thumb" },
                { FingerIdx::INDEX, "index" },
                { FingerIdx::MIDDLE, "middle" },
                { FingerIdx::RING, "ring" },
                { FingerIdx::PINKY, "pinky" }
            };
            return translation[fid];
        }
        static std::string to_string(BoneIdx bid) {
            std::map<BoneIdx, std::string> translation = {
                { BoneIdx::ROOT, "root" },
                { BoneIdx::PROXIMAL, "proximal" },
                { BoneIdx::INTERMEDIATE, "intermediate" },
                { BoneIdx::TIP, "tip" }
            };
            return translation[bid];
        }
        int _duration = 0;
        std::vector<std::tuple<FingerIdx, BoneIdx>> _indicies;

    };
}

namespace RandomWalk::MariannasParameterSpace {
    const int cells = 4;
    
    enum class RenderMode {
        STATIC, DYNAMIC
    };

    class Pattern {
    public:
        Pattern() {}
        Pattern(float duration) : _duration(duration) {}
        Pattern* clone() const { return new Pattern(*this); }
    protected:
        std::vector<std::tuple<int, int>> pattern = { {0, 0} };
        // pattern cycle in ms
        float _duration = 200;
        std::string m_name = "Pattern";
    public:
        void duration(const float& duration) { _duration = (float)duration; }
        const int& duration() const { return (int)_duration; }
        std::string name() { return m_name; }
        Ultrahaptics::Vector3 evaluate_at(Seconds t, Ultrahaptics::Vector3 offsets[cells][cells]) {
            float s = t.count();
            float ms = s * 1000;
            int len = pattern.size();
            int idx = (int)floor(ms / duration()) % len;
            auto p = pattern[idx];
            // std::cout << std::get<0>(p) << std::get<1>(p) << std::endl;
            return offsets[std::get<0>(p)][std::get<1>(p)];
        }
    };

    class ULtBR : public Pattern {
    public:
        ULtBR(float _duration = 200) : Pattern{_duration} {
            pattern = {
                {0, 0}, {1, 1}, {2, 2}, {3, 3}
            };
            m_name = "ULtBR";
        };
    };

    class BLtBR : public Pattern {
    public:
        BLtBR(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {3, 0}, {3, 1}, {3, 2}, {3, 3}
            };
            m_name = "BLtBR";
        };
    };

    class ULtUR : public Pattern {
    public:
        ULtUR(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {0, 0}, {0, 1}, {0, 2}, {0, 3}
            };
            m_name = "ULtUR";
        };
    };

    class BLtUR : public Pattern {
    public:
        BLtUR(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {3, 0}, {2, 1}, {1, 2}, {0, 3}
            };
            m_name = "BLtUR";
        };
    };

    class Circ : public Pattern {
    public:
        Circ(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}
            };
            m_name = "Circ";
        };
    };

    class CircSm : public Pattern {
    public:
        CircSm(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {1, 1}, {1, 2}, {2, 2}, {2, 1},
            };
            m_name = "CircSm";
        };
    };

    class CircGrow : public Pattern {
    public:
        CircGrow(float _duration = 200) : Pattern{ _duration } {
            pattern = {
                {1, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}
            };
            m_name = "CircGrow";
        };
    };

    class Config : public Parameters::Configuration {
    public:
        Config(float _intensity, int _frequency, Ultrahaptics::Vector3 _leap_offset, Pattern _pattern, RenderMode _mode = RenderMode::DYNAMIC) :
            Parameters::Configuration{ _intensity, _frequency}, leap_offset(_leap_offset), pattern(_pattern), mode(_mode) {};
        Ultrahaptics::Vector3 leap_offset;
        RenderMode mode = RenderMode::DYNAMIC;
        Pattern pattern;
    private:
        const float cell_size_x = 20.f; // in decimeter 20 = 2cm
        float cell_offset_x = cell_size_x / 2;
        float cell_max_x = cell_size_x + cell_offset_x;

        const float cell_size_z = 20.f; // in decimeter 20 = 2cm
        float cell_offset_z = cell_size_z / 2;
        float cell_max_z = cell_size_z + cell_offset_z;

    public:
        Ultrahaptics::Vector3 offsets[cells][cells] = {
            {   Ultrahaptics::Vector3(-cell_max_x, 0.f, -cell_max_z),
                Ultrahaptics::Vector3(-cell_offset_x, 0.f, -cell_max_z),
                Ultrahaptics::Vector3(cell_offset_x, 0.f, -cell_max_z),
                Ultrahaptics::Vector3(cell_max_x, 0.f, -cell_max_z)
            },
            {   Ultrahaptics::Vector3(-cell_max_x, 0.f, -cell_offset_z),
                Ultrahaptics::Vector3(-cell_offset_x, 0.f, -cell_offset_z),
                Ultrahaptics::Vector3(cell_offset_x, 0.f, -cell_offset_z),
                Ultrahaptics::Vector3(cell_max_x, 0.f, -cell_offset_z)
            },
            {   Ultrahaptics::Vector3(-cell_max_x, 0.f, cell_offset_z),
                Ultrahaptics::Vector3(-cell_offset_x, 0.f, cell_offset_z),
                Ultrahaptics::Vector3(cell_offset_x, 0.f, cell_offset_z),
                Ultrahaptics::Vector3(cell_max_x, 0.f, cell_offset_z)
            },
            {   Ultrahaptics::Vector3(-cell_max_x, 0.f, cell_max_z),
                Ultrahaptics::Vector3(-cell_offset_x, 0.f, cell_max_z),
                Ultrahaptics::Vector3(cell_offset_x, 0.f, cell_max_z),
                Ultrahaptics::Vector3(cell_max_x, 0.f, cell_max_z)
            },
        };
        Ultrahaptics::Vector3 evaluate_position(Seconds t) override {
            Ultrahaptics::Vector3 offset;
            if (mode == RenderMode::STATIC) {
                offset = leap_offset;
            }
            else if (mode == RenderMode::DYNAMIC) {
                offset = pattern.evaluate_at(t, offsets) + Ultrahaptics::Vector3(-20.f, 0.f, 0.f);
            }
            //std::cout << pattern.name() << std::endl;
            return offset;
        }
        Ultrahaptics::Vector3 evaluate_position(Seconds t, HandTracking::LeapOutput* leapOutput) {
            return evaluate_position(t);
        }
        std::string to_json() override {
            json j = {
                {"name", pattern.name() },
                {"intensity", intensity()},
                {"frequency", frequency()},
                {"offset.x", leap_offset.x},
                {"offset.y", leap_offset.y},
                {"offset.z", leap_offset.z},
                {"duration", pattern.duration()},
            };
            return j.dump();
        }
    };
}