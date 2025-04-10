#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"

namespace tosh::ast {

Root::Root()
  : Token(TokenType::ROOT)
{
}

ParseState
Root::on_continue(char c)
{
  if (c != ' ') {
    current(std::make_shared<Expr>());
    return ParseState::REPEAT;
  }

  return ParseState::CONTINUE;
}

}
