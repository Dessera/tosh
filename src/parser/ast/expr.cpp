#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/text.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

ExprToken::ExprToken(size_t level)
  : BaseToken(TokenType::EXPR, level)
{
}

ParseState
ExprToken::on_continue(char c)
{
  if (c == ' ') {
    return ParseState::END_PASS;
  }

  if (c == '"' || c == '\'') {
    current(std::make_shared<QuoteToken>(c, level() + 1));
    return ParseState::CONTINUE;
  }

  if (c == '\\') {
    current(std::make_shared<BackslashToken>('\0', level() + 1));
    return ParseState::CONTINUE;
  }

  if (RedirectToken::validate(c)) {
    current(std::make_shared<RedirectToken>(level() + 1));
    return ParseState::REPEAT;
  }

  current(std::make_shared<TextToken>('\0', level() + 1));
  return ParseState::REPEAT;
}

ParseState
ExprToken::on_invalid(char /*c*/)
{
  if (current()->type() == TokenType::REDIRECT) {
    current()->on_end();
    current(
      std::make_shared<TextToken>(current()->string(), '\0', level() + 1));
    return ParseState::REPEAT;
  }

  return ParseState::INVALID;
}

ParseState
ExprToken::on_end()
{
  if (current() != nullptr && current()->type() == TokenType::REDIRECT) {
    // convert shared BaseToken to shared RedirectToken
    auto redirect = std::static_pointer_cast<RedirectToken>(current());

    // not a complete redirect ->> append to text token
    if (!redirect->is_complete()) {
      redirect->on_end();
      current(
        std::make_shared<TextToken>(redirect->string(), '\0', level() + 1));
    }
  }

  return BaseToken::on_end();
}

}
