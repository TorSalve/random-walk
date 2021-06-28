#include "Utils.hpp"

namespace RandomWalk::Utils {
Ultrahaptics::Vector3 lerp(const Ultrahaptics::Vector3& A,
                           const Ultrahaptics::Vector3& B,
                           float t) {
  return A * t + B * (1.f - t);
}

std::map<std::string, std::string> map_swap(
    std::map<std::string, std::string> map) {
  std::map<std::string, std::string> newmap;
  for (auto& pair : map) {
    newmap.insert({pair.second, pair.first});
  }
  return newmap;
}

void replace(std::string& str, const std::string& from, const std::string& to) {
  if (from.empty())
    return;
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();  // In case 'to' contains 'from', like replacing
                               // 'x' with 'yx'
  }
}

std::string random_string(size_t length) {
  auto randchar = []() -> char {
    const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    const size_t max_index = (sizeof(charset) - 1);
    return charset[rand() % max_index];
  };
  std::string str(length, 0);
  std::generate_n(str.begin(), length, randchar);
  return str;
}

}  // namespace RandomWalk::Utils