#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
namespace tosh::terminal {

enum class EventType : uint8_t
{
  ASCII,
  GETCURSOR,
  ARROWUP,
  ARROWDOWN,
  ARROWLEFT,
  ARROWRIGHT,
};

using EventPayload =
  std::variant<std::string, std::pair<std::size_t, std::size_t>>;

struct Event
{
  EventType type;
  EventPayload payload;
};

class EventParser
{
public:
  [[nodiscard]] std::optional<Event> parse(std::string_view input) const;

private:
  [[nodiscard]] std::optional<Event> parse_get_cursor(
    std::string_view input) const;
};

}
