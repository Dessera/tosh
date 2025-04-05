#include "tosh/parser/token/base.hpp"
#include <memory>
#include <ranges>

namespace tosh::parser {

BaseToken::BaseToken(TokenType type)
  : _type(type)
{
}

TokenState
BaseToken::parse_end()
{
  return TokenState::END;
}

TreeToken::TreeToken(TokenType type)
  : BaseToken(type)
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
  } else {
    return create_new_token(c);
  }
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

std::span<const std::shared_ptr<BaseToken>>
TreeToken::tokens() const
{
  return _children;
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
