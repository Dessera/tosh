#pragma once

#include "tosh/error.hpp"

#include <event2/event.h>
#include <queue>
#include <string>
#include <thread>

namespace tosh::terminal {

class InputEventPool
{
private:
  event_base* _base;
  std::thread _event_loop;
  std::queue<std::string> _ansi_codes;
  std::queue<std::string> _ascii_codes;

public:
  InputEventPool(std::FILE* in);
  ~InputEventPool();

  InputEventPool(const InputEventPool&) = delete;
  InputEventPool& operator=(const InputEventPool&) = delete;

  InputEventPool(InputEventPool&&) noexcept = default;
  InputEventPool& operator=(InputEventPool&&) = default;

private:
  static void on_input_ready(evutil_socket_t fd, short events, void* arg);
  static void event_loop();

  event_base* create_event_base(std::FILE* in);
  std::thread create_event_loop_thread();
};

}