#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <semaphore>

namespace tosh::utils {

template<typename T>
class SyncQueue
{
private:
  std::queue<T> _queue{};
  std::mutex _mutex;
  // std::counting_semaphore<> _sem{ 0 };
  std::condition_variable _cv;

public:
  template<typename... Args>
  constexpr void emplace(Args&&... args)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.emplace(std::forward<Args>(args)...);

    _cv.notify_one();
  }

  constexpr void push(T&& item)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(std::move(item));

    _cv.notify_one();
  }

  constexpr T pop()
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _cv.wait(lock, [this]() { return !_queue.empty(); });

    T item = _queue.front();
    _queue.pop();
    return item;
  }
};

}
