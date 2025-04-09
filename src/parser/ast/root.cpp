#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/parser.hpp"

namespace tosh::ast {

Root::Root()
  : IToken(TokenType::ROOT)
{
}

parser::ParseState
Root::on_continue(parser::TokenParseContext& ctx, char c)
{
  if (c != ' ') {
    ctx.push<Expr>();
    return parser::ParseState::REPEAT;
  }

  return parser::ParseState::CONTINUE;
}

}
