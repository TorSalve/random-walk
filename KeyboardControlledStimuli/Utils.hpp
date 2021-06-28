#pragma once

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <vector>

#include <ultraleap/haptics/vector3.hpp>

namespace RandomWalk::Utils {

Ultrahaptics::Vector3 lerp(const Ultrahaptics::Vector3& A,
                           const Ultrahaptics::Vector3& B,
                           float t);

std::map<std::string, std::string> map_swap(
    std::map<std::string, std::string> map);

template <typename IntType>
std::vector<IntType> linspace(IntType start, IntType end, int num_in) {
  std::vector<IntType> linspaced;

  double num = static_cast<double>(num_in);

  if (num == 0) {
    return linspaced;
  }
  if (num == 1) {
    linspaced.push_back(start);
    return linspaced;
  }

  double delta = (end - start) / (num - 1);

  for (int i = 0; i < num - 1; ++i) {
    linspaced.push_back(start + delta * i);
  }
  linspaced.push_back(end);  // I want to ensure that start and end
                             // are exactly the same as the input
  return linspaced;
};

template <typename IntType>
void print_element(IntType d) {
  std::cout << d << " ";
}

template <typename IntType>
void print_element(std::vector<IntType> vec, bool p_size = false) {
  if (p_size)
    std::cout << "size: " << vec.size() << std::endl;
  for (IntType d : vec)
    print_element(d);
  std::cout << std::endl;
};

template <typename IntType>
void print_element(std::string_view comment, std::vector<IntType> vec) {
  std::cout << comment;
  print_element(vec);
}

template <typename IntType>
void print_element(const std::map<std::string, IntType>& m) {
  for (const auto& [key, value] : m) {
    std::cout << "\t" << key << " = ";
    print_element(value);
    std::cout << " ;";
  }
  std::cout << std::endl;
}

template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop, IntType step) {
  if (step == IntType(0)) {
    throw std::invalid_argument("step for range must be non-zero");
  }

  std::vector<IntType> result;
  IntType i = start;
  while ((step > 0) ? (i < stop) : (i > stop)) {
    result.push_back(i);
    i += step;
  }

  return result;
}

template <typename IntType>
std::vector<IntType> range(IntType start, IntType stop) {
  return range(start, stop, IntType(1));
}

template <typename IntType>
std::vector<IntType> range(IntType stop) {
  return range(IntType(0), stop, IntType(1));
}

// https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
template <typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
  std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
  std::advance(start, dis(g));
  return start;
}

template <typename Iter>
Iter select_randomly(Iter start, Iter end) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return select_randomly(start, end, gen);
}

template <typename KType, typename VType>
std::vector<KType> map_get_keys(std::map<KType, VType> map,
                                bool sorted = false) {
  std::vector<KType> keys;
  for (auto const& cmap : map)
    keys.push_back(cmap.first);
  if (sorted) {
    std::sort(keys.begin(), keys.end());
  }
  return keys;
}

template <typename KType, typename VType>
std::vector<VType> map_get_values(std::map<KType, VType> map) {
  std::vector<VType> values;
  for (auto const& cmap : map)
    values.push_back(cmap.second);
  return values;
}

template <typename KType, typename VType>
std::tuple<KType, VType> map_get_random(std::map<KType, VType> map,
                                        std::vector<std::string> exclude = {}) {
  std::sort(exclude.begin(), exclude.end());
  std::vector<KType> _keys = map_get_keys(map, true);
  std::vector<KType> keys;
  std::set_difference(_keys.begin(), _keys.end(), exclude.begin(),
                      exclude.end(), std::back_inserter(keys));
  KType r_key = *Utils::select_randomly(keys.begin(), keys.end());
  return std::make_tuple(r_key, map[r_key]);
}

constexpr unsigned int hash(const char* s, int off = 0) {
  return !s[off] ? 5381 : (hash(s, off + 1) * 33) ^ s[off];
};

void replace(std::string& str, const std::string& from, const std::string& to);

std::string random_string(size_t length);

}  // namespace RandomWalk::Utils