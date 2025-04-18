#include "tosh/terminal/event.hpp"
#include "tosh/error.hpp"

#include <cstdio>
#include <event2/event.h>
#include <unistd.h>

namespace tosh::terminal {

InputEventPool::InputEventPool(std::FILE* in)
  : _base(create_event_base(in))
  , _event_loop(create_event_loop_thread())
{
}

InputEventPool::~InputEventPool()
{
  event_base_loopbreak(_base);
  if (_event_loop.joinable()) {
    _event_loop.join();
  }

  event_base_free(_base);
}

void
// NOLINTNEXTLINE
InputEventPool::on_input_ready(evutil_socket_t fd, short event, void* arg)
{
  char buf[256];
  auto* pool = static_cast<InputEventPool*>(arg);

  auto len = read(fd, buf, sizeof(buf));
}

event_base*
InputEventPool::create_event_base(std::FILE* in)
{
  auto* base = event_base_new();
  if (base == nullptr) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  auto* ev = event_new(base,
                       fileno(in),
                       EV_READ | EV_PERSIST,
                       InputEventPool::on_input_ready,
                       this);
  if (ev == nullptr) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  if (event_add(ev, nullptr) == -1) {
    event_free(ev);
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  return base;
}

std::thread
InputEventPool::create_event_loop_thread()
{
  return std::thread([this]() { event_base_dispatch(_base); });
}

}