#include "tosh/terminal/event/parser.hpp"
#include "magic_enum/magic_enum.hpp"

#include <algorithm>
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
  namespace ranges = std::ranges;

  assert(!input.empty());

  if (auto key = magic_enum::enum_cast<EventSpecialKey::Key>(input.front());
      key.has_value()) {
    return EventSpecialKey{ key.value() };
  }

  if (ranges::all_of(
        input, [](char c) { return (c >= ' ' && c <= '~') || c == '\n'; })) {
    return EventGetString{ std::string(input) };
  }

  if (!input.starts_with('\x1b')) {
    return std::nullopt;
  }

  switch (input.back()) {
    case 'R':
      return parse_get_cursor(input);
    case 'A':
      return EventMoveCursor{ EventMoveCursor::Direction::UP };
    case 'B':
      return EventMoveCursor{ EventMoveCursor::Direction::DOWN };
    case 'C':
      return EventMoveCursor{ EventMoveCursor::Direction::RIGHT };
    case 'D':
      return EventMoveCursor{ EventMoveCursor::Direction::LEFT };
    default:
      return std::nullopt;
  }
}

}
