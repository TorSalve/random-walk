#include <stdio.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>

#include "Timer.hpp"

namespace RandomWalk::Time {
void Timer::setTimeout(const std::function<void(void)>& function, int delay) {
  active = true;
  std::thread t([=]() {
    if (!active.load())
      return;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    if (!active.load())
      return;
    function();
  });
  t.detach();
}

void Timer::setInterval(const std::function<void(void)>& function,
                        int interval) {
  active = true;
  std::thread t([=]() {
    while (active.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(interval));
      if (!active.load())
        return;
      function();
    }
  });
  t.detach();
}

void Timer::stop() {
  active = false;
}
}  // namespace RandomWalk::Time