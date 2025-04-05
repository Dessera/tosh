#include "tosh/parser/token/backslash.hpp"
#include <cstddef>

namespace tosh::parser {

// NOLINTNEXTLINE
BackslashToken::BackslashToken(char quote, size_t level)
  : BaseToken(TokenType::BACKSLASH, level)
  , _quote(quote)
{
}

TokenState
BackslashToken::parse_next(char c)
{
  if (_quote != '\0') {
    if (c == _quote || c == '\\') {
      _str = c;
    } else {
      _str = { '\\', c };
    }
  } else {
    _str = c;
  }

  return TokenState::END_PASS;
}

std::string
BackslashToken::to_string() const
{
  return _str;
}

}
