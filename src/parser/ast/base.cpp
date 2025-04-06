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
BaseToken::submit_current_token()
{
  if (_current_token) {
    _current_token->parse_end();
    _children.push_back(_current_token);
    _current_token = nullptr;
  }
}

void
BaseToken::set_current_token(std::shared_ptr<BaseToken> token)
{
  _current_token = std::move(token);
}

ParseState
BaseToken::handle_invalid()
{
  return ParseState::CONTINUE;
}

ParseState
BaseToken::parse_next(char c)
{
  if (_current_token) {
    switch (_current_token->parse_next(c)) {
      case ParseState::CONTINUE:
        return ParseState::CONTINUE;
      case ParseState::END:
        submit_current_token();
        return handle_char(c);
      case ParseState::END_PASS:
        submit_current_token();
        return ParseState::CONTINUE;
      case ParseState::INVALID:
        return handle_invalid();
      case ParseState::REPEAT:
        return ParseState::REPEAT;
      case ParseState::ERROR:
        return ParseState::ERROR;
    }
  }

  return handle_char(c);
}

ParseState
BaseToken::parse_end()
{
  submit_current_token();
  return ParseState::END;
}

std::string
BaseToken::to_string() const
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return _children | views::transform([](const auto& token) {
           return token->to_string();
         }) |
         views::join | ranges::to<std::string>();
}

}
