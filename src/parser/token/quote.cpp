#include "tosh/parser/token/quote.hpp"
#include "tosh/parser/token/backslash.hpp"
#include "tosh/parser/token/normal.hpp"

namespace tosh::parser {

QuoteToken::QuoteToken(char quote)
  : TreeToken(TokenType::QUOTE)
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
    _current_token = std::make_unique<BackslashToken>(_quote);
  } else {
    _current_token = std::make_unique<NormalToken>(c);
  }

  return TokenState::CONTINUE;
}
}
