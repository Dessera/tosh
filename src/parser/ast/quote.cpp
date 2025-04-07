#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/backslash.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/text.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

// NOLINTNEXTLINE
QuoteToken::QuoteToken(char quote, size_t level)
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

  if (c == '\\') {
    current(std::make_shared<BackslashToken>(_quote, level() + 1));
    return ParseState::CONTINUE;
  }

  current(std::make_shared<TextToken>(_quote, level() + 1));
  return ParseState::REPEAT;
}

ParseState
QuoteToken::on_invalid(char c)
{
  if (current()->type() != TokenType::BACKSLASH) {
    return ParseState::INVALID;
  }

  current(
    std::make_shared<TextToken>(std::string{ '\\', c }, _quote, level() + 1));
  return ParseState::CONTINUE;
}

}
