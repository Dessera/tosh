#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/quote.hpp"
#include "tosh/parser/ast/redirect.hpp"

#include <cstddef>
#include <memory>

namespace tosh::ast {

// NOLINTNEXTLINE
BackslashToken::BackslashToken(size_t level)
  : BaseToken(TokenType::BACKSLASH, level)
{
}

ParseState
BackslashToken::on_continue(char c)
{
  _bs_token = c;
  return ParseState::END_PASS;
}

std::string
BackslashToken::string() const
{
  return { _bs_token };
}

// NOLINTNEXTLINE
TextToken::TextToken(size_t level)
  : TextToken({}, level)
{
}

// NOLINTNEXTLINE
TextToken::TextToken(std::string str, size_t level)
  : BaseToken(TokenType::TEXT, level)
  , _str(std::move(str))
{
}

ParseState
TextToken::on_continue(char c)
{
  if (QuoteToken::validate(c).has_value() || BackslashToken::validate(c) ||
      c == ' ') {
    return ParseState::END;
  }

  _str += c;

  return ParseState::CONTINUE;
}

std::string
TextToken::string() const
{
  return _str;
}

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

  if (auto quote = QuoteToken::validate(c); quote.has_value()) {
    current(std::make_shared<QuoteToken>(quote.value(), level() + 1));
    return ParseState::CONTINUE;
  }

  if (BackslashToken::validate(c)) {
    current(std::make_shared<BackslashToken>(level() + 1));
    return ParseState::CONTINUE;
  }

  if (RedirectToken::validate(c)) {
    current(std::make_shared<RedirectToken>(level() + 1));
    return ParseState::REPEAT;
  }

  current(std::make_shared<TextToken>(level() + 1));
  return ParseState::REPEAT;
}

ParseState
ExprToken::on_invalid(char /*c*/)
{
  if (current()->type() == TokenType::REDIRECT) {
    current()->on_end();
    current(std::make_shared<TextToken>(current()->string(), level() + 1));
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
      current(std::make_shared<TextToken>(redirect->string(), level() + 1));
    }
  }

  return BaseToken::on_end();
}

}
