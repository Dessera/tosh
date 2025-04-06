#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/text.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

// NOLINTNEXTLINE
QuoteToken::QuoteToken(char quote, size_t level)
  : BaseToken(TokenType::QUOTE, level)
  , _quote(quote)
{
}

ParseState
QuoteToken::handle_char(char c)
{
  if (c == _quote) {
    return ParseState::END_PASS;
  }

  if (c == '\\') {
    set_current_token(std::make_shared<BackslashToken>(_quote, level() + 1));
    return ParseState::CONTINUE;
  }

  set_current_token(std::make_shared<TextToken>(_quote, level() + 1));
  return ParseState::REPEAT;
}

}
