#include "desh/parser/token/quote.hpp"
#include "desh/parser/token/backslash.hpp"
#include "desh/parser/token/normal.hpp"

namespace desh::parser {

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
