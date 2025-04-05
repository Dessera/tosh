#include "desh/parser/token/backslash.hpp"

namespace desh::parser {

BackslashToken::BackslashToken(char quote)
  : BaseToken(TokenType::BACKSLASH)
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
