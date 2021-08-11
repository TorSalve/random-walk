#pragma once
// https://github.com/99x/timercpp

#include <stdio.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

namespace RandomWalk::Time {
class Timer {
  std::atomic<bool> active{true};

 public:
  void setTimeout(const std::function<void(void)>& function, int delay);
  void setInterval(const std::function<void(void)>& function, int interval);
  void stop();
};
}  // namespace RandomWalk::Time
