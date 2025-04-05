#include "desh/parser/token/root.hpp"
#include "desh/parser/token/backslash.hpp"
#include "desh/parser/token/base.hpp"
#include "desh/parser/token/expr.hpp"

namespace desh::parser {

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
