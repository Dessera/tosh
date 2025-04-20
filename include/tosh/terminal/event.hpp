#pragma once

#include "tosh/common.hpp"
#include "tosh/utils/queue.hpp"

#include <event2/event.h>
#include <string>
#include <thread>

namespace tosh::terminal {

enum class TOSH_EXPORT InputEventType : uint8_t
{
  ASCII,
  GETCURSOR,
  ARROWUP,
  ARROWDOWN,
  ARROWLEFT,
  ARROWRIGHT,
};

class TOSH_EXPORT InputEventPool
{
public:
  constexpr static int EV_BUFSIZ = 256;

private:
  event_base* _base;
  std::thread _event_loop;
  utils::SyncQueue<std::string> _ansi_events{};
  utils::SyncQueue<char> _input_events{};

public:
  InputEventPool(std::FILE* in);
  ~InputEventPool();

  constexpr auto read_ansi_event() { return _ansi_events.pop(); }
  constexpr auto read_input_event() { return _input_events.pop(); }

private:
  static void on_input_ready(evutil_socket_t fd, short events, void* arg);
  static void event_loop();

  event_base* create_event_base(std::FILE* in);
  std::thread create_event_loop_thread();
};

}