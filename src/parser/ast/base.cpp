#include "tosh/parser/ast/base.hpp"

#include <cstddef>
#include <memory>
#include <ranges>
#include <utility>

namespace tosh::ast {

const char*
token_type_to_string(TokenType type)
{
  switch (type) {
    case TokenType::ROOT:
      return "ROOT";
    case TokenType::TEXT:
      return "TEXT";
    case TokenType::BACKSLASH:
      return "BACKSLASH";
    case TokenType::QUOTE:
      return "QUOTE";
    case TokenType::EXPR:
      return "EXPR";
    case TokenType::REDIRECT:
      return "REDIRECT";
    case TokenType::REDIRECT_SRC:
      return "REDIRECT_SRC";
    case TokenType::REDIRECT_DST:
      return "REDIRECT_DST";
    case TokenType::REDIRECT_OP:
      return "REDIRECT_OP";
    default:
      return "UNKNOWN";
  }
}

BaseToken::BaseToken(TokenType type, size_t level)
  : _type(type)
  , _level(level)
{
}

void
BaseToken::push_current()
{
  if (_current_token != nullptr) {
    _current_token->parse_end();
    _tokens.push_back(_current_token);
    _current_token = nullptr;
  }
}

void
BaseToken::set_current(std::shared_ptr<BaseToken> token)
{
  if (_current_token == nullptr) {
    _current_token = std::move(token);
  }
}

ParseState
BaseToken::handle_invalid()
{
  return ParseState::INVALID;
}

ParseState
BaseToken::parse_next(char c)
{
  auto status = ParseState::CONTINUE;

  // NOLINTNEXTLINE
  do {
    if (_current_token) {
      status = _current_token->parse_next(c);
      if (status == ParseState::INVALID) {
        status = handle_invalid();
      } else if (status == ParseState::END) {
        push_current();
        status = handle_char(c);
      } else if (status == ParseState::END_PASS) {
        push_current();
        status = ParseState::CONTINUE;
      }
    } else {
      status = handle_char(c);
    }
  } while (status == ParseState::REPEAT);

  return status;
}

ParseState
BaseToken::parse_end()
{
  push_current();
  return ParseState::END;
}

std::string
BaseToken::to_string() const
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return _tokens | views::transform([](const auto& token) {
           return token->to_string();
         }) |
         views::join | ranges::to<std::string>();
}

}
