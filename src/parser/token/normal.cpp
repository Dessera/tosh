#include "desh/parser/token/normal.hpp"

namespace desh::parser {

NormalToken::NormalToken(char init)
  : BaseToken(TokenType::NORMAL)
  , _str{ init }
{
}

TokenState
NormalToken::parse_next(char c)
{
  if (std::ranges::find(reserved_chars, c) != reserved_chars.end()) {
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
