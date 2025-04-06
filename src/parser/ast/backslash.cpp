#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include <cstddef>

namespace tosh::ast {

// NOLINTNEXTLINE
BackslashToken::BackslashToken(char quote, size_t level)
  : BaseToken(TokenType::BACKSLASH, level)
  , _quote(quote)
{
}

ParseState
BackslashToken::handle_char(char c)
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

  return ParseState::END_PASS;
}

std::string
BackslashToken::to_string() const
{
  return _str;
}

}
