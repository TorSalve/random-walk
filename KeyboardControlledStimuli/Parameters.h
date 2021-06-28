#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <map>
#include <optional>

#include "ExploringSensationLibrary.h"
#include "HandTracking.h"
#include "Utils.hpp"
#include "json.hpp"

using Seconds = std::chrono::duration<float>;

using json = nlohmann::json;

namespace RandomWalk::Parameters {

class Configuration {
 public:
  Configuration(float intensity = 1.f,
                int frequency = 256,
                float duration = 256)
      : _intensity(intensity), _frequency(frequency), _duration(duration) {}
  HandTracking::LeapListening hand;

 private:
  float _intensity;
  int _frequency;
  float _duration;
  std::optional<Seconds> _started;
  float _delay = 1.f;

 protected:
  bool _palm_position = true;
  virtual float intensity_modulation(Seconds t) { return sine(t); }
  float sine(Seconds t) {
    // return (1.0 - std::cos(2 * M_PI * frequency * t.count())) * 0.5;
    return (1.0 - std::cos(2 * M_PI * frequency() * t.count())) * 0.5 *
           intensity();
  }
  bool playtime(Seconds t) {
    if (!_started.has_value()) {
      _started = Seconds(t.count());
    }
    auto start = _started.value().count();  // +_delay;
    /*std::cout << t.count() << "|" << start << "|" << duration() << "|"
              << start + (duration() / 1000) << std::endl;*/
    return
        // delay condition
        // t.count() >= start &&
        // out of delay
        t.count() <= start + (duration() / 1000);
  }

 public:
  void reset_playtime() {
    if (_started.has_value()) {
      _started.reset();
    }
  }
  float evaluate_intensity(Seconds t) {
    float t_intensity = 0.f;
    if (playtime(t)) {
      t_intensity = intensity_modulation(t);
    }
    return t_intensity;
  }
  virtual Ultrahaptics::Vector3 evaluate_position(Seconds t) = 0;
  virtual Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) = 0;
  virtual void pre_hook(HandTracking::LeapOutput* leapOutput, int sample_rate) {
  }

  virtual std::string to_json() = 0;

  const bool& palm_position() const { return _palm_position; }
  const float& intensity() const { return _intensity; }
  void intensity(const float& intensity) { _intensity = intensity; }
  const int& frequency() const { return _frequency; }
  void frequency(const int& frequency) { _frequency = frequency; }
  void duration(const float& duration) { _duration = duration; }
  const float& duration() const { return _duration; }
};

class Point : public Configuration {
 public:
  Point(float intensity,
        int frequency,
        float duration,
        Ultrahaptics::Vector3 offset)
      : Configuration(intensity, frequency, duration), _offset(offset) {}

 private:
  Ultrahaptics::Vector3 _offset;

 public:
  Ultrahaptics::Vector3 evaluate_position(Seconds t) override {
    return offset();
  };

  const Ultrahaptics::Vector3& offset() const { return _offset; }
  void offset(const Ultrahaptics::Vector3& offset) { _offset = offset; }
};

class Brush : public Point {
 public:
  Brush(float intensity,
        int frequency,
        float duration,
        Ultrahaptics::Vector3 offset,
        float width,
        float height,
        float width_frequency,
        float height_frequecy)
      : Point(intensity, frequency, duration, offset),
        _width(width),
        _height(height),
        _width_frequency(width_frequency),
        _height_frequency(height_frequecy) {
    _palm_position = false;
    _displacementX = _right * _width * 0.5;
    _displacementY = _up * _height * 0.5;
    _width_sec = 1 / _width_frequency;
    _height_sec = 1 / _height_frequency;
  }
  Ultrahaptics::Vector3 evaluate_position(Seconds t) override {
    throw "Provide hand tracking data";
  }
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) override {
    float fraction = fmod(t.count(), _width_sec) / _width_sec;
    Ultrahaptics::Vector3 position =
        Utils::lerp(_endpointXA, _endpointXB, fraction);

    if (_height > 1.f) {
      if (_last_fraction > fraction) {
        _height_fraction += _height_sec;
        _height_fraction = _height_fraction > 1 ? 0 : _height_fraction;
      }
      position += Utils::lerp(_endpointYA, _endpointYB, _height_fraction);
    }

    _last_fraction = fraction;
    return Point::evaluate_position(t) + position;
  }
  void pre_hook(HandTracking::LeapOutput* leapOutput,
                int sample_rate) override {
    _endpointXA = leapOutput->palm_position - _displacementX;
    _endpointXB = leapOutput->palm_position + _displacementX;

    _endpointYA = leapOutput->palm_position - _displacementY;
    _endpointYB = leapOutput->palm_position + _displacementY;
  }

 private:
  Ultrahaptics::Vector3 _right = Ultrahaptics::Vector3(1, 0, 0);
  Ultrahaptics::Vector3 _up = Ultrahaptics::Vector3(0, 0, 1);
  float _width;
  float _height;
  float _width_frequency;
  float _width_sec;
  float _height_frequency;
  float _height_sec;
  float _height_fraction = 0.5f;
  float _last_fraction = 1;
  Ultrahaptics::Vector3 _displacementX;
  Ultrahaptics::Vector3 _displacementY;
  Ultrahaptics::Vector3 _endpointXA = Ultrahaptics::Vector3();
  Ultrahaptics::Vector3 _endpointXB = Ultrahaptics::Vector3();
  Ultrahaptics::Vector3 _endpointYA = Ultrahaptics::Vector3();
  Ultrahaptics::Vector3 _endpointYB = Ultrahaptics::Vector3();
  std::string to_json() override {
    json j = {
        {"name", "Point"},
        {"intensity", intensity()},
        {"frequency", frequency()},
        {"duration", duration()},
    };
    return j.dump();
  }
};

class Line : public Brush {
  Line(float intensity,
       int frequency,
       float duration,
       Ultrahaptics::Vector3 offset,
       float width,
       float width_frequency)
      : Brush(intensity,
              frequency,
              duration,
              offset,
              width,
              1.f,
              width_frequency,
              1.f) {}
};

class StaticPoint : public Point {
 public:
  StaticPoint(float intensity,
              int frequency,
              float duration,
              Ultrahaptics::Vector3 offset)
      : Point(intensity, frequency, duration, offset) {}
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) override {
    return Point::evaluate_position(t);
  };
  std::string to_json() override {
    json j = {
        {"name", "Point"},          {"intensity", intensity()},
        {"frequency", frequency()}, {"duration", duration()},
        {"offset.x", offset().x},   {"offset.y", offset().y},
        {"offset.z", offset().z},
    };
    return j.dump();
  }
};

enum class FingerIdx {
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

class Ripple : public Point {
 public:
  Ripple(float intensity,
         int frequency,
         float duration,
         Ultrahaptics::Vector3 offset,
         int num_points,
         std::tuple<int, int> boundaries,
         float jump_frequency)
      : Point(intensity, frequency, duration, offset),
        _num_points(num_points),
        _boundaries(boundaries),
        _jump_frequency(jump_frequency) {
    std::srand(_seed);
    int width = std::get<0>(boundaries);
    int height = std::get<1>(boundaries);
    for (size_t i = 0; i < _num_points; i++) {
      float w = (std::rand() % width) - width;
      float h = (std::rand() % height) - height;
      offsets.emplace_back(w, 0, h);
    }
    /*Utils::print_vector(offsets);*/
  }
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) override {
    int idx = (int)floor(t.count() / (1 / _jump_frequency)) % _num_points;
    Ultrahaptics::Vector3 position = offsets[idx];
    return Point::evaluate_position(t) + position;
  }
  std::string to_json() override {
    json j = {{"name", "Ripple"},         {"intensity", intensity()},
              {"frequency", frequency()}, {"duration", duration()},
              {"offset.x", offset().x},   {"offset.y", offset().y},
              {"offset.z", offset().z},   {"jump_frequency", _jump_frequency}};
    return j.dump();
  }

 private:
  unsigned int _seed = 42;
  int _num_points;
  std::tuple<float, float> _boundaries;
  std::vector<Ultrahaptics::Vector3> offsets;
  float _jump_frequency;
};

enum class Square_Rendering { Random, Spiral, ColumnWise, RowWise };

class Square : public Point {
 public:
  Square(float intensity,
         int frequency,
         float duration,
         Ultrahaptics::Vector3 offset,
         int width_resolution,
         int height_resolution,
         std::tuple<int, int> boundaries,
         float jump_frequency,
         Square_Rendering rendering = Square_Rendering::Random)
      : Point(intensity, frequency, duration, offset),
        _boundaries(boundaries),
        _jump_frequency(jump_frequency) {
    int width = std::get<0>(boundaries);
    int height = std::get<1>(boundaries);

    auto ws = Utils::range(-width / 2, width / 2, width_resolution);
    auto hs = Utils::range(-height / 2, height / 2, height_resolution);

    /*Utils::print_vector(ws);
    Utils::print_vector(hs);*/

    std::vector<Ultrahaptics::Vector3> _points;
    std::function<void(int, int, int, int)> append;
    append = [_points, ws, hs, &append](int i, int j, int m, int n) mutable {
      // If i or j lies outside the matrix
      if (i >= m or j >= n)
        return;

      // Print First Row
      for (int p = j; p < n; p++)
        _points.emplace_back(ws[i], 0, hs[p]);

      // Print Last Column
      for (int p = i + 1; p < m; p++)
        _points.emplace_back(ws[p], 0, hs[n - 1]);

      // Print Last Row, if Last and
      // First Row are not same
      if ((m - 1) != i)
        for (int p = n - 2; p >= j; p--)
          _points.emplace_back(ws[m - 1], 0, hs[p]);

      // Print First Column,  if Last and
      // First Column are not same
      if ((n - 1) != j)
        for (int p = m - 2; p > i; p--)
          _points.emplace_back(ws[p], 0, hs[j]);

      append(i + 1, j + 1, m - 1, n - 1);
    };

    switch (rendering) {
      case Square_Rendering::Random:
        for (auto& h : hs) {
          for (auto& w : ws) {
            points.emplace_back(w, 0, h);
          }
        }
        std::shuffle(points.begin(), points.end(),
                     std::default_random_engine(std::time(0)));
        break;
      case Square_Rendering::Spiral:
        append(0, 0, ws.size(), hs.size());
        points = _points;
        break;
      case Square_Rendering::ColumnWise:
        for (auto& w : ws) {
          for (auto& h : hs) {
            points.emplace_back(w, 0, h);
          }
        }
        break;
      case Square_Rendering::RowWise:
        for (auto& h : hs) {
          for (auto& w : ws) {
            points.emplace_back(w, 0, h);
          }
        }
        break;
      default:
        break;
    }
  }
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) override {
    int idx = (int)floor(t.count() / (1 / _jump_frequency)) % points.size();
    Ultrahaptics::Vector3 position = points[idx];
    return Point::evaluate_position(t) + position;
  }
  float intensity_modulation(Seconds t) override { return intensity(); }
  std::string to_json() override {
    json j = {{"name", "Square"},         {"intensity", intensity()},
              {"frequency", frequency()}, {"duration", duration()},
              {"offset.x", offset().x},   {"offset.y", offset().y},
              {"offset.z", offset().z},   {"jump_frequency", _jump_frequency}};
    return j.dump();
  }

 private:
  std::tuple<float, float> _boundaries;
  std::vector<Ultrahaptics::Vector3> points;
  float _jump_frequency;
};

class TrackedPoint : public Point {
 public:
  TrackedPoint(float intensity,
               int frequency,
               float duration,
               Ultrahaptics::Vector3 offset,
               FingerIdx fidx,
               BoneIdx bidx)
      : Point(intensity, frequency, duration, offset) {
    _indicies.emplace_back(fidx, bidx);
    _palm_position = false;
  }
  TrackedPoint(float intensity,
               int frequency,
               float duration,
               Ultrahaptics::Vector3 offset,
               std::vector<std::tuple<FingerIdx, BoneIdx>> indicies)
      : Point(intensity, frequency, duration, offset), _indicies(indicies) {
    _palm_position = false;
  }
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) override {
    float s = t.count();
    float ms = s * 1000;
    int len = _indicies.size();
    int idx = (int)floor(ms / duration()) % len;

    auto finger_bone = _indicies[idx];
    auto tracking = HandTracking::translate_finger_output(leapOutput);

    return Point::evaluate_position(t) +
           tracking[(int)std::get<0>(finger_bone)]
                   [(int)std::get<1>(finger_bone)];
  };
  std::string to_json() override {
    json j = {
        {"name", "TrackedPoint"},   {"intensity", intensity()},
        {"frequency", frequency()}, {"duration", duration()},
        {"offset.x", offset().x},   {"offset.y", offset().y},
        {"offset.z", offset().z},
    };
    for (size_t i = 0; i < _indicies.size(); i++) {
      auto t = _indicies[i];
      auto f = std::get<0>(t);
      auto b = std::get<1>(t);
      j["finger_bone." + std::to_string(i)] = to_string(f) + "_" + to_string(b);
    }
    return j.dump();
    ;
  }

 private:
  static std::string to_string(FingerIdx fid) {
    std::map<FingerIdx, std::string> translation = {
        {FingerIdx::THUMB, "thumb"},
        {FingerIdx::INDEX, "index"},
        {FingerIdx::MIDDLE, "middle"},
        {FingerIdx::RING, "ring"},
        {FingerIdx::PINKY, "pinky"}};
    return translation[fid];
  }
  static std::string to_string(BoneIdx bid) {
    std::map<BoneIdx, std::string> translation = {
        {BoneIdx::ROOT, "root"},
        {BoneIdx::PROXIMAL, "proximal"},
        {BoneIdx::INTERMEDIATE, "intermediate"},
        {BoneIdx::TIP, "tip"}};
    return translation[bid];
  }
  std::vector<std::tuple<FingerIdx, BoneIdx>> _indicies;
};
}  // namespace RandomWalk::Parameters

namespace RandomWalk::MariannasParameterSpace {
const int cells = 4;

enum class RenderMode { STATIC, DYNAMIC };

class Pattern {
 public:
  Pattern() {}
  Pattern(float duration) : _duration(duration) {}
  Pattern* clone() const { return new Pattern(*this); }

 protected:
  std::vector<std::tuple<int, int>> pattern = {{0, 0}};
  // pattern cycle in ms
  int _duration = 200;
  std::string m_name = "Pattern";

 public:
  void duration(const int& duration) { _duration = duration; }
  const int& duration() const { return _duration; }
  std::string name() { return m_name; }
  Ultrahaptics::Vector3 evaluate_at(
      Seconds t,
      Ultrahaptics::Vector3 offsets[cells][cells]) {
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
    pattern = {{0, 0}, {1, 1}, {2, 2}, {3, 3}};
    m_name = "ULtBR";
  };
};

class BLtBR : public Pattern {
 public:
  BLtBR(float _duration = 200) : Pattern{_duration} {
    pattern = {{3, 0}, {3, 1}, {3, 2}, {3, 3}};
    m_name = "BLtBR";
  };
};

class ULtUR : public Pattern {
 public:
  ULtUR(float _duration = 200) : Pattern{_duration} {
    pattern = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};
    m_name = "ULtUR";
  };
};

class BLtUR : public Pattern {
 public:
  BLtUR(float _duration = 200) : Pattern{_duration} {
    pattern = {{3, 0}, {2, 1}, {1, 2}, {0, 3}};
    m_name = "BLtUR";
  };
};

class Circ : public Pattern {
 public:
  Circ(float _duration = 200) : Pattern{_duration} {
    pattern = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 3},
               {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}};
    m_name = "Circ";
  };
};

class CircSm : public Pattern {
 public:
  CircSm(float _duration = 200) : Pattern{_duration} {
    pattern = {
        {1, 1},
        {1, 2},
        {2, 2},
        {2, 1},
    };
    m_name = "CircSm";
  };
};

class CircGrow : public Pattern {
 public:
  CircGrow(float _duration = 200) : Pattern{_duration} {
    pattern = {{1, 1}, {1, 2}, {2, 2}, {2, 1}, {0, 0}, {0, 1}, {0, 2}, {0, 3},
               {1, 3}, {2, 3}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 0}, {1, 0}};
    m_name = "CircGrow";
  };
};

class Config : public Parameters::Configuration {
 public:
  Config(float _intensity,
         int _frequency,
         Ultrahaptics::Vector3 _leap_offset,
         Pattern _pattern,
         RenderMode _mode = RenderMode::DYNAMIC)
      : Parameters::Configuration{_intensity, _frequency},
        leap_offset(_leap_offset),
        pattern(_pattern),
        mode(_mode){};
  Ultrahaptics::Vector3 leap_offset;
  RenderMode mode = RenderMode::DYNAMIC;
  Pattern pattern;

 private:
  const float cell_size_x = 20.f;  // in decimeter 20 = 2cm
  float cell_offset_x = cell_size_x / 2;
  float cell_max_x = cell_size_x + cell_offset_x;

  const float cell_size_z = 20.f;  // in decimeter 20 = 2cm
  float cell_offset_z = cell_size_z / 2;
  float cell_max_z = cell_size_z + cell_offset_z;

 public:
  Ultrahaptics::Vector3 offsets[cells][cells] = {
      {Ultrahaptics::Vector3(-cell_max_x, 0.f, -cell_max_z),
       Ultrahaptics::Vector3(-cell_offset_x, 0.f, -cell_max_z),
       Ultrahaptics::Vector3(cell_offset_x, 0.f, -cell_max_z),
       Ultrahaptics::Vector3(cell_max_x, 0.f, -cell_max_z)},
      {Ultrahaptics::Vector3(-cell_max_x, 0.f, -cell_offset_z),
       Ultrahaptics::Vector3(-cell_offset_x, 0.f, -cell_offset_z),
       Ultrahaptics::Vector3(cell_offset_x, 0.f, -cell_offset_z),
       Ultrahaptics::Vector3(cell_max_x, 0.f, -cell_offset_z)},
      {Ultrahaptics::Vector3(-cell_max_x, 0.f, cell_offset_z),
       Ultrahaptics::Vector3(-cell_offset_x, 0.f, cell_offset_z),
       Ultrahaptics::Vector3(cell_offset_x, 0.f, cell_offset_z),
       Ultrahaptics::Vector3(cell_max_x, 0.f, cell_offset_z)},
      {Ultrahaptics::Vector3(-cell_max_x, 0.f, cell_max_z),
       Ultrahaptics::Vector3(-cell_offset_x, 0.f, cell_max_z),
       Ultrahaptics::Vector3(cell_offset_x, 0.f, cell_max_z),
       Ultrahaptics::Vector3(cell_max_x, 0.f, cell_max_z)},
  };
  Ultrahaptics::Vector3 evaluate_position(Seconds t) override {
    Ultrahaptics::Vector3 offset;
    if (mode == RenderMode::STATIC) {
      offset = leap_offset;
    } else if (mode == RenderMode::DYNAMIC) {
      offset = pattern.evaluate_at(t, offsets) +
               Ultrahaptics::Vector3(-20.f, 0.f, 0.f);
    }
    // std::cout << pattern.name() << std::endl;
    return offset;
  }
  Ultrahaptics::Vector3 evaluate_position(
      Seconds t,
      HandTracking::LeapOutput* leapOutput) {
    return evaluate_position(t);
  }
  std::string to_json() override {
    json j = {
        {"name", pattern.name()},         {"intensity", intensity()},
        {"frequency", frequency()},       {"offset.x", leap_offset.x},
        {"offset.y", leap_offset.y},      {"offset.z", leap_offset.z},
        {"duration", pattern.duration()},
    };
    return j.dump();
  }
};
}  // namespace RandomWalk::MariannasParameterSpace

namespace RandomWalk::Sensations {
class Config : public Parameters::Configuration {
  Config(float intensity = 1.f,
         int frequency = 256,
         KnownSensation sensation = KnownSensation::CircleWithFixedFrequency)
      : Parameters::Configuration(intensity, frequency) {}
};
}  // namespace RandomWalk::Sensations