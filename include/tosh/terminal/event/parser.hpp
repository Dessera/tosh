#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace tosh::terminal {

/**
 * @brief Get string event.
 *
 */
struct EventGetString
{
  std::string str;
};

/**
 * @brief Get cursor event.
 */
struct EventGetCursor
{
  std::size_t x;
  std::size_t y;
};

/**
 * @brief Event type.
 */
using Event = std::variant<EventGetString, EventGetCursor>;

/**
 * @brief Parses a string into an event.
 *
 * @param input String to parse.
 * @return std::optional<Event> Parsed event, or std::nullopt if the string
 * could not be parsed.
 */
std::optional<Event>
parse(std::string_view input);

/**
 * @brief Event filter that checks if the event is one of the specified types.
 *
 * @tparam Ts Types to check for.
 */
template<typename... Ts>
struct EventFilter
{
  constexpr bool operator()(const Event& event) const
  {
    return (... || std::holds_alternative<Ts>(event));
  }
};

}
