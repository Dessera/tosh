#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/queue.hpp"

#include <cstddef>
#include <cstdio>
#include <event2/event.h>
#include <memory>
#include <thread>

namespace tosh::terminal {

struct EventBaseCleaner
{
  constexpr void operator()(event_base* base) const { event_base_free(base); }
};

using EventBasePtr = std::unique_ptr<event_base, EventBaseCleaner>;

struct EventCleaner
{
  constexpr void operator()(event* ev) const { event_free(ev); }
};

using EventPtr = std::unique_ptr<event, EventCleaner>;

/**
 * @brief Stdin event reader
 */
class EventReader
{
public:
  constexpr static size_t EVBUFSIZ = 256;

private:
  EventBasePtr _base;
  EventPtr _event;
  std::unique_ptr<EventQueue> _queue;

  std::jthread _eloop;

public:
  EventReader(EventBasePtr base,
              EventPtr ev,
              std::unique_ptr<EventQueue> queue,
              std::jthread eloop);
  ~EventReader();

  TOSH_DELETE_COPY(EventReader)
  TOSH_DEFAULT_MOVE(EventReader)

  constexpr auto read(auto&& pred, auto&& timeout)
  {
    return _queue->read(std::forward<decltype(pred)>(pred),
                        std::forward<decltype(timeout)>(timeout));
  }

  constexpr auto read(auto&& pred)
  {
    return _queue->read(std::forward<decltype(pred)>(pred));
  }

  static error::Result<EventReader> create(std::FILE* in);

private:
  static void handle_event_loop(event_base* base);
  static void handle_event(evutil_socket_t fd, short events, void* arg);
};

}
