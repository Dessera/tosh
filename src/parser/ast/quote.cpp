#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/base.hpp"

#include <memory>

namespace tosh::ast {

QuoteBackslash::QuoteBackslash(QuoteType quote)
  : Token(TokenType::BACKSLASH)
  , _quote(quote)
{
}

ParseState
QuoteBackslash::on_continue(char c)
{
  if (c == '\n') {
    return ParseState::END_PASS;
  }

  if (is_quote(c) || c == '\\') {
    _bs_char = c;
    return ParseState::END_PASS;
  }

  return ParseState::INVALID;
}

std::string
QuoteBackslash::string() const
{
  if (_bs_char != '\0') {
    return { _bs_char };
  }

  return std::string{};
}

QuoteText::QuoteText(QuoteType quote)
  : QuoteText({}, quote)
{
}

QuoteText::QuoteText(std::string text, QuoteType quote)
  : Token(TokenType::TEXT)
  , _text(std::move(text))
  , _quote(quote)
{
}

ParseState
QuoteText::on_continue(char c)
{
  if (QuoteBackslash::validate(c) || is_quote(c)) {
    return ParseState::END;
  }

  _text += c;
  return ParseState::CONTINUE;
}

std::string
QuoteText::string() const
{
  return _text;
}

// NOLINTNEXTLINE
QuoteExpr::QuoteExpr(QuoteType quote)
  : Token(TokenType::QUOTE)
  , _quote(quote)
{
}

ParseState
QuoteExpr::on_continue(char c)
{
  if (is_quote(c)) {
    return ParseState::END_PASS;
  }

  if (QuoteBackslash::validate(c)) {
    current(std::make_shared<QuoteBackslash>(_quote));
    return ParseState::CONTINUE;
  }

  current(std::make_shared<QuoteText>(_quote));
  return ParseState::REPEAT;
}

ParseState
QuoteExpr::on_invalid(char c)
{
  if (current()->type() != TokenType::BACKSLASH) {
    return ParseState::INVALID;
  }

  current(std::make_shared<QuoteText>(std::string{ '\\', c }, _quote));
  return ParseState::CONTINUE;
}

}
