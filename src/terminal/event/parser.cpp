#include "tosh/terminal/event/parser.hpp"

#include <cassert>
#include <cstddef>

namespace tosh::terminal {

std::optional<Event>
EventParser::parse(std::string_view input) const
{
  assert(!input.empty());

  if (input.starts_with('\x1b')) {
    // get cursor position
    if (input.ends_with('B')) {
      return parse_get_cursor(input);
    }

    return std::nullopt;
  }

  return Event{ .type = EventType::ASCII, .payload = std::string(input) };
}

std::optional<Event>
EventParser::parse_get_cursor(std::string_view input) const
{
  assert(input.size() >= 3);

  // get first number (y)
}

}