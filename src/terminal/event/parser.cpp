#include "tosh/terminal/event/parser.hpp"

#include <cassert>
#include <cstddef>

namespace {

using namespace tosh::terminal;

std::optional<Event>
parse_get_cursor(std::string_view input)
{
  assert(input.size() >= 3);

  size_t x = 0;
  size_t y = 0;

  std::size_t pos = 2;

  while (input[pos] != ';' && pos < input.size()) {
    // NOLINTNEXTLINE
    y = y * 10 + (input[pos] - '0');
    ++pos;
  }

  ++pos;

  while (input[pos] != 'R' && pos < input.size()) {
    // NOLINTNEXTLINE
    x = x * 10 + (input[pos] - '0');
    ++pos;
  }

  return EventGetCursor{ .x = x - 1, .y = y - 1 };
}

}

namespace tosh::terminal {

std::optional<Event>
parse(std::string_view input)
{
  assert(!input.empty());

  if (!input.starts_with('\x1b')) {
    return EventGetString{ std::string(input) };
  }

  switch (input.back()) {
    case 'R':
      return parse_get_cursor(input);
    default:
      return std::nullopt;
  }
}

}
