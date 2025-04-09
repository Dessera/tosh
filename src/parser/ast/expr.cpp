#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/redirect.hpp"

#include <memory>
#include <string>

namespace tosh::ast {

// NOLINTNEXTLINE
Backslash::Backslash()
  : BaseToken(TokenType::BACKSLASH)
{
}

ParseState
Backslash::on_continue(char c)
{
  _bs_token = c;
  return ParseState::END_PASS;
}

std::string
Backslash::string() const
{
  return { _bs_token };
}

// NOLINTNEXTLINE
Text::Text()
  : Text(std::string{})
{
}

// NOLINTNEXTLINE
Text::Text(std::string str)
  : BaseToken(TokenType::TEXT)
  , _str(std::move(str))
{
}

ParseState
Text::on_continue(char c)
{
  if (QuoteExpr::validate(c).has_value() || Backslash::validate(c) ||
      c == ' ') {
    return ParseState::END;
  }

  _str += c;

  return ParseState::CONTINUE;
}

std::string
Text::string() const
{
  return _str;
}

Expr::Expr()
  : BaseToken(TokenType::EXPR)
{
}

ParseState
Expr::on_continue(char c)
{
  if (c == ' ') {
    return ParseState::END_PASS;
  }

  if (auto quote = QuoteExpr::validate(c); quote.has_value()) {
    current(std::make_shared<QuoteExpr>(quote.value()));
    return ParseState::CONTINUE;
  }

  if (Backslash::validate(c)) {
    current(std::make_shared<Backslash>());
    return ParseState::CONTINUE;
  }

  if (Redirect::validate(c)) {
    current(std::make_shared<Redirect>());
    return ParseState::REPEAT;
  }

  current(std::make_shared<Text>());
  return ParseState::REPEAT;
}

ParseState
Expr::on_invalid(char /*c*/)
{
  if (current()->type() == TokenType::REDIRECT) {
    current()->on_end();
    current(std::make_shared<Text>(current()->string()));
    return ParseState::REPEAT;
  }

  return ParseState::INVALID;
}

ParseState
Expr::on_end()
{
  if (current() != nullptr && current()->type() == TokenType::REDIRECT) {
    // convert shared BaseToken to shared RedirectToken
    auto redirect = std::static_pointer_cast<Redirect>(current());

    // not a complete redirect ->> append to text token
    if (!redirect->is_complete()) {
      redirect->on_end();
      current(std::make_shared<Text>(redirect->string()));
    }
  }

  return BaseToken::on_end();
}

}
