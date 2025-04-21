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

/**
 * @brief Stdin event reader
 */
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

public:
  EventReader(std::FILE* in);
  ~EventReader();

  /**
   * @brief Reads an event which matches the predicate
   *
   * @param pred predicate to match events
   * @return error::Result<Event> event if found, error otherwise
   */
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

    return error::err(error::ErrorCode::EVENT_NOT_FOUND,
                      "Requested event not found");
  }

  /**
   * @brief Reads an event which matches the predicate
   *
   * @tparam Dt duration type
   * @param pred predicate to match events
   * @param timeout read timeout
   * @return error::Result<Event> event if found, EVENT_TIMEOUT if timed out,
   * error otherwise
   */
  template<typename Dt>
  error::Result<Event> read(auto&& pred,
                            const std::chrono::duration<Dt>& timeout)
  {
    std::unique_lock<std::mutex> lock(_lock);

    if (!_cv.wait_for(lock, timeout, [this, &pred]() {
          return std::any_of(_events.begin(), _events.end(), pred);
        })) {
      return error::err(error::ErrorCode::EVENT_TIMEOUT,
                        "Timeout waiting for event");
    }

    if (auto it = std::find_if(_events.begin(), _events.end(), pred);
        it != _events.end()) {
      auto event = std::move(*it);
      _events.erase(it);
      return event;
    }

    return error::err(error::ErrorCode::EVENT_NOT_FOUND,
                      "Requested event not found");
  }

  /**
   * @brief Push an event to the event queue
   *
   * @param event event to push
   */
  void push(const Event& event);

  /**
   * @brief Push an event to the event queue
   *
   * @tparam Args event constructor argument types
   * @param args event constructor arguments
   */
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
