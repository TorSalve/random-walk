// https://github.com/99x/timercpp

#include <atomic>
#include <chrono>
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
