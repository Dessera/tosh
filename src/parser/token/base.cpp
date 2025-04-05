#include "tosh/parser/token/base.hpp"

#include <cstddef>
#include <memory>
#include <ranges>

namespace tosh::parser {

const char*
token_type_to_string(TokenType type)
{
  switch (type) {
    case TokenType::ROOT:
      return "ROOT";
    case TokenType::NORMAL:
      return "NORMAL";
    case TokenType::BACKSLASH:
      return "BACKSLASH";
    case TokenType::QUOTE:
      return "QUOTE";
    case TokenType::EXPR:
      return "EXPR";
    default:
      return "UNKNOWN";
  }
}

BaseToken::BaseToken(TokenType type, size_t level)
  : _type(type)
  , _level(level)
{
}

TokenState
BaseToken::parse_end()
{
  return TokenState::END;
}

TreeToken::TreeToken(TokenType type, size_t level)
  : BaseToken(type, level)
{
}

TokenState
TreeToken::parse_next(char c)
{
  if (_current_token) {
    switch (_current_token->parse_next(c)) {
      case TokenState::CONTINUE:
        return TokenState::CONTINUE;
      case TokenState::END:
        submit_current_token();
        return create_new_token(c);
      case TokenState::END_PASS:
        submit_current_token();
        return TokenState::CONTINUE;
      case TokenState::ERROR:
        return TokenState::ERROR;
    }
  }

  return create_new_token(c);
}

TokenState
TreeToken::parse_end()
{
  submit_current_token();
  return TokenState::END;
}

std::string
TreeToken::to_string() const
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return _children | views::transform([](const auto& token) {
           return token->to_string();
         }) |
         views::join | ranges::to<std::string>();
}

void
TreeToken::clear()
{
  _children.clear();
  _current_token = nullptr;
}

bool
TreeToken::is_empty() const
{
  return _children.empty();
}

void
TreeToken::submit_current_token()
{
  if (_current_token) {
    _current_token->parse_end();
    _children.push_back(std::move(_current_token));
    _current_token = nullptr;
  }
}

}
