#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/text.hpp"
#include <cstddef>
#include <memory>

namespace tosh::ast {

ExprToken::ExprToken(size_t level)
  : BaseToken(TokenType::EXPR, level)
{
}

ParseState
ExprToken::handle_char(char c)
{
  if (c == ' ') {
    return ParseState::END_PASS;
  }

  if (c == '"' || c == '\'') {
    set_current(std::make_shared<QuoteToken>(c, level() + 1));
    return ParseState::CONTINUE;
  }

  if (c == '\\') {
    set_current(std::make_shared<BackslashToken>('\0', level() + 1));
    return ParseState::CONTINUE;
  }

  set_current(std::make_shared<TextToken>('\0', level() + 1));
  return ParseState::REPEAT;
}

// ParseState
// ExprToken::handle_invalid()
// {
// }

}
