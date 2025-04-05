#include "tosh/parser/token/normal.hpp"

namespace tosh::parser {

// NOLINTNEXTLINE
NormalToken::NormalToken(char init, char quote, size_t level)
  : BaseToken(TokenType::NORMAL, level)
  , _str{ init }
  , _quote(quote)
{
}

TokenState
NormalToken::parse_next(char c)
{
  if (c == ' ' || c == '\\' || c == _quote) {
    return TokenState::END;
  }

  _str += c;

  return TokenState::CONTINUE;
}

std::string
NormalToken::to_string() const
{
  return _str;
}

}
