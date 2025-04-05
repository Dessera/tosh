#include "tosh/parser/token/root.hpp"
#include "tosh/parser/token/backslash.hpp"
#include "tosh/parser/token/base.hpp"
#include "tosh/parser/token/expr.hpp"

namespace tosh::parser {

RootToken::RootToken()
  : TreeToken(TokenType::ROOT)
{
}

TokenState
RootToken::create_new_token(char c)
{
  if (c != ' ') {
    _current_token = std::make_unique<ExprToken>(c);
  }

  return TokenState::CONTINUE;
}

}
