#include "tosh/parser/token/quote.hpp"
#include "tosh/parser/token/backslash.hpp"
#include "tosh/parser/token/normal.hpp"
#include <cstddef>

namespace tosh::parser {

// NOLINTNEXTLINE
QuoteToken::QuoteToken(char quote, size_t level)
  : TreeToken(TokenType::QUOTE, level)
  , _quote(quote)
{
}

TokenState
QuoteToken::create_new_token(char c)
{
  if (c == _quote) {
    return TokenState::END_PASS;
  }

  if (c == '\\') {
    _current_token = std::make_unique<BackslashToken>(_quote, _level + 1);
  } else {
    _current_token = std::make_unique<NormalToken>(c, _quote, _level + 1);
  }

  return TokenState::CONTINUE;
}
}
