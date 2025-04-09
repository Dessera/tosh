#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/parser.hpp"
// #include "tosh/parser/ast/redirect.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

// NOLINTNEXTLINE
Backslash::Backslash()
  : IToken(TokenType::BACKSLASH)
{
}

parser::ParseState
Backslash::on_continue(parser::TokenParseContext& /*ctx*/, char c)
{
  _bs_token = c;
  return parser::ParseState::END_PASS;
}

std::string
Backslash::string() const
{
  return { _bs_token };
}

// NOLINTNEXTLINE
Text::Text()
  : IToken(TokenType::TEXT)
{
}

// NOLINTNEXTLINE
Text::Text(std::string str)
  : IToken(TokenType::TEXT)
  , _str(std::move(str))
{
}

parser::ParseState
Text::on_continue(parser::TokenParseContext& /*ctx*/, char c)
{
  if (QuoteExpr::validate(c).has_value() || Backslash::validate(c) ||
      c == ' ') {
    return parser::ParseState::END;
  }

  _str += c;
  return parser::ParseState::CONTINUE;
}

std::string
Text::string() const
{
  return _str;
}

Expr::Expr()
  : IToken(TokenType::EXPR)
{
}

parser::ParseState
Expr::on_continue(parser::TokenParseContext& ctx, char c)
{
  if (c == ' ') {
    return parser::ParseState::END_PASS;
  }

  if (auto quote = QuoteExpr::validate(c); quote.has_value()) {
    ctx.push<QuoteExpr>(quote.value());
    return parser::ParseState::CONTINUE;
  }

  if (Backslash::validate(c)) {
    ctx.push<Backslash>();
    return parser::ParseState::CONTINUE;
  }

  // if (RedirectToken::validate(c)) {
  //   current(std::make_shared<RedirectToken>(level() + 1));
  //   return ParseState::REPEAT;
  // }

  ctx.push<Text>();
  return parser::ParseState::REPEAT;
}

// ParseState
// ExprToken::on_invalid(char /*c*/)
// {
//   if (current()->type() == TokenType::REDIRECT) {
//     current()->on_end();
//     current(std::make_shared<TextToken>(current()->string(), level() + 1));
//     return ParseState::REPEAT;
//   }

//   return ParseState::INVALID;
// }

}
