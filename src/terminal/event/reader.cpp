#include "tosh/terminal/event/reader.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/queue.hpp"

#include <array>
#include <cassert>
#include <cstdio>
#include <event2/event.h>
#include <event2/thread.h>
#include <expected>
#include <memory>
#include <string_view>
#include <unistd.h>
#include <utility>

namespace tosh::terminal {

EventReader::EventReader(EventBasePtr base,
                         EventPtr ev,
                         std::unique_ptr<EventQueue> queue,
                         std::jthread eloop)
  : _base(std::move(base))
  , _event(std::move(ev))
  , _queue(std::move(queue))
  , _eloop(std::move(eloop))
{
}

EventReader::~EventReader()
{
  event_base_loopbreak(_base.get());
}

error::Result<EventReader>
EventReader::create(std::FILE* in)
{
  assert(in != nullptr);

  auto queue = std::make_unique<EventQueue>();

  if (evthread_use_pthreads() == -1) {
    return error::err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  auto base = EventBasePtr(event_base_new());
  if (base == nullptr) {
    return error::err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  auto ev = EventPtr(event_new(
    base.get(), fileno(in), EV_READ | EV_PERSIST, handle_event, queue.get()));
  if (ev == nullptr) {
    return error::err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  if (event_add(ev.get(), nullptr) == -1) {
    return error::err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  std::jthread eloop{ handle_event_loop, base.get() };

  return EventReader{
    std::move(base), std::move(ev), std::move(queue), std::move(eloop)
  };
}

void
EventReader::handle_event_loop(event_base* base)
{
  assert(base != nullptr);
  event_base_dispatch(base);
}

void
EventReader::handle_event(evutil_socket_t fd, short event, void* arg)
{
  assert(arg != nullptr);
  auto* queue = static_cast<EventQueue*>(arg);

  if (!(event & EV_READ)) {
    return;
  }

  std::array<char, EVBUFSIZ> buf{};
  auto n = ::read(fd, buf.data(), buf.size());
  if (n <= 0) {
    return;
  }

  if (auto payload = parse(std::string_view(buf.data(), n));
      payload.has_value()) {
    queue->push(payload.value());
  }
}

}
