#include "tosh/terminal/event/reader.hpp"

#include <array>
#include <cassert>
#include <cstdio>
#include <event2/event.h>
#include <string_view>
#include <unistd.h>

namespace tosh::terminal {

EventReader::EventReader(std::FILE* in)
  : _base(event_base_new())
  , _event(nullptr)
{
  if (_base == nullptr) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  auto in_fd = fileno(in);
  if (in_fd == -1) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  _event = event_new(_base, in_fd, EV_READ | EV_PERSIST, handle_event, this);
  if (_event == nullptr) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  if (event_add(_event, nullptr) == -1) {
    throw error::raw_err(error::ErrorCode::EVENT_LOOP_FAILED);
  }

  _eloop = std::thread(handle_event_loop, this);
}

EventReader::~EventReader()
{
  event_base_loopbreak(_base);
  if (_eloop.joinable()) {
    _eloop.join();
  }

  event_free(_event);
  event_base_free(_base);
}

void
EventReader::push(const Event& event)
{
  {
    std::lock_guard<std::mutex> lock(_lock);
    _events.push_back(event);
  }
  _cv.notify_one();
}

void
EventReader::handle_event_loop(EventReader* reader)
{
  assert(reader != nullptr);
  event_base_dispatch(reader->_base);
}

void
EventReader::handle_event(evutil_socket_t fd, short event, void* arg)
{
  assert(arg != nullptr);
  EventReader* reader = static_cast<EventReader*>(arg);

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
    reader->push(payload.value());
  }
}

}
