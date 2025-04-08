#include "tosh/parser/ast/base.hpp"

#include <cstddef>
#include <memory>
#include <ranges>
#include <vector>

namespace tosh::ast {

BaseToken::BaseToken(TokenType type, size_t level)
  : _type(type)
  , _level(level)
{
}

ParseState
BaseToken::iter_next(char c)
{
  auto status = ParseState::CONTINUE;

  if (c == '\0') {
    return on_end();
  }

  // NOLINTNEXTLINE
  do {
    if (current() == nullptr) {
      status = on_continue(c);
      continue;
    }

    status = current()->iter_next(c);

    if (status == ParseState::END) {
      add(current());
      current(nullptr);
      status = on_continue(c);
    } else if (status == ParseState::END_PASS) {
      add(current());
      current(nullptr);
      status = ParseState::CONTINUE;
    } else if (status == ParseState::INVALID) {
      status = on_invalid(c);
    }
  } while (status == ParseState::REPEAT);

  return status;
}

std::string
BaseToken::string() const
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return nodes() |
         views::transform([](const auto& token) { return token->string(); }) |
         views::join | ranges::to<std::string>();
}

ParseState
BaseToken::on_invalid(char /*c*/)
{
  return ParseState::INVALID;
}

ParseState
BaseToken::on_end()
{
  if (current() == nullptr) {
    return ParseState::END;
  }

  add(current());
  auto status = current()->on_end();
  current(nullptr);
  return status;
}

}
