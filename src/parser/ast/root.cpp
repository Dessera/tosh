#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"

namespace tosh::ast {

RootToken::RootToken()
  : BaseToken(TokenType::ROOT)
{
}

ParseState
RootToken::handle_char(char c)
{
  if (c != ' ') {
    set_current(std::make_shared<ExprToken>(level() + 1));
    return ParseState::REPEAT;
  }

  return ParseState::CONTINUE;
}

}
