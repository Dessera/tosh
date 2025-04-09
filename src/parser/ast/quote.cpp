#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/parser.hpp"

namespace tosh::ast {

QuoteBackslash::QuoteBackslash(QuoteType quote)
  : IToken(TokenType::BACKSLASH)
  , _quote(quote)
{
}

parser::ParseState
QuoteBackslash::on_continue(parser::TokenParseContext& /*ctx*/, char c)
{
  if (c == _quote || validate(c)) {
    _bs_char = c;
    return parser::ParseState::END_PASS;
  }

  return parser::ParseState::INVALID;
}

parser::ParseState
QuoteBackslash::on_invalid(parser::TokenParseContext& ctx, char c)
{
  return parser::ParseState::INVALID;
}

std::string
QuoteBackslash::string() const
{
  return { _bs_char };
}

QuoteText::QuoteText(QuoteType quote)
  : QuoteText({}, quote)
{
}

QuoteText::QuoteText(std::string text, QuoteType quote)
  : IToken(TokenType::TEXT)
  , _text(std::move(text))
  , _quote(quote)
{
}

parser::ParseState
QuoteText::on_continue(parser::TokenParseContext& /*ctx*/, char c)
{
  if (QuoteBackslash::validate(c) || c == _quote) {
    return parser::ParseState::END;
  }

  _text += c;
  return parser::ParseState::CONTINUE;
}

std::string
QuoteText::string() const
{
  return _text;
}

// NOLINTNEXTLINE
QuoteExpr::QuoteExpr(QuoteType quote)
  : IToken(TokenType::QUOTE)
  , _quote(quote)
{
}

parser::ParseState
QuoteExpr::on_continue(parser::TokenParseContext& ctx, char c)
{
  if (c == _quote) {
    return parser::ParseState::END_PASS;
  }

  if (QuoteBackslash::validate(c)) {
    ctx.push<QuoteBackslash>(_quote);
    return parser::ParseState::CONTINUE;
  }

  ctx.push<QuoteText>(_quote);
  return parser::ParseState::REPEAT;
}

}
