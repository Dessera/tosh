#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"

namespace tosh::ast {

RootToken::RootToken()
  : BaseToken(TokenType::ROOT)
{
}

ParseState
RootToken::on_continue(char c)
{
  if (c != ' ') {
    current(std::make_shared<ExprToken>(level() + 1));
    return ParseState::REPEAT;
  }

  return ParseState::CONTINUE;
}

}
