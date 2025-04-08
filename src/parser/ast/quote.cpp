#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/base.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

QuoteBackslashToken::QuoteBackslashToken(QuoteType quote, size_t level)
  : BaseToken(TokenType::BACKSLASH, level)
  , _quote(quote)
{
}

ParseState
QuoteBackslashToken::on_continue(char c)
{
  if (c == _quote || c == '\\') {
    _bs_char = c;
    return ParseState::END_PASS;
  }

  return ParseState::INVALID;
}

std::string
QuoteBackslashToken::string() const
{
  return { _bs_char };
}

QuoteTextToken::QuoteTextToken(QuoteType quote, size_t level)
  : QuoteTextToken({}, quote, level)
{
}

QuoteTextToken::QuoteTextToken(std::string text, QuoteType quote, size_t level)
  : BaseToken(TokenType::TEXT, level)
  , _text(std::move(text))
  , _quote(quote)
{
}

ParseState
QuoteTextToken::on_continue(char c)
{
  if (QuoteBackslashToken::validate(c) || c == _quote) {
    return ParseState::END;
  }

  _text += c;
  return ParseState::CONTINUE;
}

std::string
QuoteTextToken::string() const
{
  return _text;
}

// NOLINTNEXTLINE
QuoteToken::QuoteToken(QuoteType quote, size_t level)
  : BaseToken(TokenType::QUOTE, level)
  , _quote(quote)
{
}

ParseState
QuoteToken::on_continue(char c)
{
  if (c == _quote) {
    return ParseState::END_PASS;
  }

  if (QuoteBackslashToken::validate(c)) {
    current(std::make_shared<QuoteBackslashToken>(_quote, level() + 1));
    return ParseState::CONTINUE;
  }

  current(std::make_shared<QuoteTextToken>(_quote, level() + 1));
  return ParseState::REPEAT;
}

ParseState
QuoteToken::on_invalid(char c)
{
  if (current()->type() != TokenType::BACKSLASH) {
    return ParseState::INVALID;
  }

  current(std::make_shared<QuoteTextToken>(
    std::string{ '\\', c }, _quote, level() + 1));
  return ParseState::CONTINUE;
}

}
