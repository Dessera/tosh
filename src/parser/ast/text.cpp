#include "tosh/parser/ast/text.hpp"
#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

// NOLINTNEXTLINE
TextToken::TextToken(char quote, size_t level)
  : BaseToken(TokenType::TEXT, level)
  , _quote(quote)
{
}

ParseState
TextToken::handle_char(char c)
{
  if (c == '\\' || (_quote != '\0' && c == _quote) ||
      (_quote == '\0' && (c == '\'' || c == '"' || c == ' '))) {
    return ParseState::END;
  }

  _str += c;

  return ParseState::CONTINUE;
}

std::string
TextToken::to_string() const
{
  return _str;
}

}
