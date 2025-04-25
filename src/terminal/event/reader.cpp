#include "tosh/terminal/event/reader.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/queue.hpp"

#include <array>
#include <cassert>
#include <cstdio>
#include <event2/event.h>
#include <expected>
#include <memory>
#include <string_view>
#include <unistd.h>
#include <utility>

namespace tosh::terminal {

EventReader::EventReader(EventBasePtr base, EventPtr ev)
  : _base(std::move(base))
  , _event(std::move(ev))
{
}

EventReader::~EventReader()
{
  if (_eloop != nullptr) {
    event_base_loopbreak(_base.get());
  }
}

error::Result<EventReader>
EventReader::create(std::FILE* in)
{
  assert(in != nullptr);

  auto queue = std::make_unique<EventQueue>();

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

  return EventReader{ std::move(base), std::move(ev) };
}

error::Result<void>
EventReader::start()
try {
  _eloop = std::make_unique<std::jthread>(handle_event_loop, this);
  return {};
} catch (const std::exception& e) {
  return error::err(error::ErrorCode::EVENT_LOOP_FAILED, e.what());
}

error::Result<void>
EventReader::stop()
{
  event_base_loopbreak(_base.get());

  if (_eloop != nullptr) {
    _eloop = nullptr;
  }

  return {};
}

void
EventReader::handle_event_loop(EventReader* reader)
{
  assert(reader != nullptr);
  event_base_dispatch(reader->_base.get());
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
