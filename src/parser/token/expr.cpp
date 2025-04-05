#include "tosh/parser/token/expr.hpp"
#include "tosh/parser/token/backslash.hpp"
#include "tosh/parser/token/base.hpp"
#include "tosh/parser/token/normal.hpp"
#include "tosh/parser/token/quote.hpp"
#include <cstddef>

namespace tosh::parser {

// NOLINTNEXTLINE
ExprToken::ExprToken(char init, size_t level)
  : TreeToken(TokenType::EXPR, level)
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
    _current_token = std::make_unique<QuoteToken>(c, _level + 1);
  } else if (c == '\\') {
    _current_token = std::make_unique<BackslashToken>('\0', _level + 1);
  } else {
    _current_token = std::make_unique<NormalToken>(c, _level + 1);
  }

  return TokenState::CONTINUE;
}

}
