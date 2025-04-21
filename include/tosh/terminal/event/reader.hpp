#pragma once

#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"

#include <condition_variable>
#include <cstddef>
#include <cstdio>
#include <deque>
#include <event2/event.h>
#include <mutex>

namespace tosh::terminal {

class EventReader
{
public:
  constexpr static size_t EVBUFSIZ = 256;

private:
  event_base* _base;
  std::thread _eloop;

  std::deque<Event> _events;

  std::mutex _lock;
  std::condition_variable _cv;

  EventParser _parser{};

public:
  EventReader(std::FILE* in);
  ~EventReader();

  template<typename Dt>
  error::Result<bool> poll(auto&& pred,
                           const std::chrono::duration<Dt>& timeout)
  {
    std::unique_lock<std::mutex> lock(_lock);

    if (_cv.wait_for(lock, timeout, [this, &pred]() {
          return std::any_of(_events.begin(), _events.end(), pred);
        })) {
      if (auto it = std::find_if(_events.begin(), _events.end(), pred);
          it != _events.end()) {
        _events.erase(it);
        return true;
      }
    }

    return false;
  }

  error::Result<bool> poll(auto&& pred)
  {
    std::unique_lock<std::mutex> lock(_lock);

    _cv.wait(lock, [this, &pred]() {
      return std::any_of(_events.begin(), _events.end(), pred);
    });

    if (auto it = std::find_if(_events.begin(), _events.end(), pred);
        it != _events.end()) {
      _events.erase(it);
      return true;
    }

    return false;
  }

  error::Result<Event> read(auto&& pred)
  {
    std::unique_lock<std::mutex> lock(_lock);

    _cv.wait(lock, [this, &pred]() {
      return std::any_of(_events.begin(), _events.end(), pred);
    });

    if (auto it = std::find_if(_events.begin(), _events.end(), pred);
        it != _events.end()) {
      auto event = std::move(*it);
      _events.erase(it);
      return event;
    }

    return error::err(error::ErrorCode::EVENT_NOT_FOUND, "Event not found");
  }

  void push(const Event& event);

  template<typename... Args>
  void emplace(Args&&... args)
  {
    {
      std::lock_guard<std::mutex> lock(_lock);
      _events.emplace_back(std::forward<Args>(args)...);
    }
    _cv.notify_one();
  }

private:
  event_base* create_base(std::FILE* in);
  std::thread create_event_loop();

  static void handle_event_loop(EventReader* reader);
  static void handle_event(evutil_socket_t fd, short events, void* arg);
};

}
