#include "desh/parser/token/expr.hpp"
#include "desh/parser/token/backslash.hpp"
#include "desh/parser/token/base.hpp"
#include "desh/parser/token/normal.hpp"
#include "desh/parser/token/quote.hpp"

namespace desh::parser {

ExprToken::ExprToken(char init)
  : TreeToken(TokenType::EXPR)
{
  create_new_token(init);
}

TokenState
ExprToken::create_new_token(char c)
{
  if (c == ' ') {
    return TokenState::END_PASS;
  }

  if (c == '"' || c == '\'') {
    _current_token = std::make_unique<QuoteToken>(c);
  } else if (c == '\\') {
    _current_token = std::make_unique<BackslashToken>('\0');
  } else {
    _current_token = std::make_unique<NormalToken>(c);
  }

  return TokenState::CONTINUE;
}

}
