#include "desh/parser/token/root.hpp"
#include "desh/parser/token/backslash.hpp"
#include "desh/parser/token/base.hpp"
#include "desh/parser/token/normal.hpp"
#include "desh/parser/token/quote.hpp"

namespace desh::parser {

RootToken::RootToken()
  : TreeToken(TokenType::ROOT)
{
}

TokenState
RootToken::parse_end()
{
  submit_current_token();
  return TokenState::END;
}

TokenState
RootToken::create_new_token(char c)
{
  if (c == '"' || c == '\'') {
    _current_token = std::make_unique<QuoteToken>(c);
  } else if (c == '\\') {
    _current_token = std::make_unique<BackslashToken>(0);
  } else if (c == ' ') {
    // pass
  } else {
    _current_token = std::make_unique<NormalToken>(c);
  }

  return TokenState::CONTINUE;
}

}
