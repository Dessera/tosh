#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/text.hpp"

#include <algorithm>
#include <cstddef>
#include <format>
#include <memory>

namespace {

constexpr bool
is_num(char c)
{
  return c >= '0' && c <= '9';
}

}

namespace tosh::ast {

// NOLINTNEXTLINE
RedirectSrcToken::RedirectSrcToken(size_t level)
  : BaseToken(TokenType::REDIRECT_SRC, level)
{
}

ParseState
RedirectSrcToken::handle_char(char c)
{
  if (is_num(c)) {
    _src += c;
    return ParseState::CONTINUE;
  }

  return ParseState::END;
}

std::string
RedirectSrcToken::to_string() const
{
  return _src;
}

// NOLINTNEXTLINE
RedirectOpToken::RedirectOpToken(size_t level)
  : BaseToken(TokenType::REDIRECT_OP, level)
{
}

ParseState
RedirectOpToken::handle_char(char c)
{
  if (std::ranges::find(VALID_OPCS, c) != VALID_OPCS.end()) {
    _op += c;
    return ParseState::CONTINUE;
  }

  if (std::ranges::find(VALID_OPS, _op) != VALID_OPS.end()) {
    return ParseState::END;
  }

  return ParseState::INVALID;
}

std::string
RedirectOpToken::to_string() const
{
  return _op;
}

// NOLINTNEXTLINE
RedirectToken::RedirectToken(size_t level)
  : BaseToken(TokenType::REDIRECT, level)
{
}

ParseState
RedirectToken::handle_char(char c)
{
  if (_op == nullptr) {
    if (is_num(c)) {
      _src = std::make_shared<RedirectSrcToken>(level() + 1);
      set_current_token(_src);
      return ParseState::REPEAT;
    }
    if (std::ranges::find(RedirectOpToken::VALID_OPCS, c) !=
        RedirectOpToken::VALID_OPCS.end()) {
      _op = std::make_shared<RedirectOpToken>(level() + 1);
      set_current_token(_op);
      return ParseState::REPEAT;
    }
  }

  if (_dest == nullptr) {
    _dest = std::make_shared<TextToken>('\0', level() + 1);
    set_current_token(_dest);
    return ParseState::REPEAT;
  }

  return ParseState::END;
}

ParseState
RedirectToken::handle_invalid()
{
  return ParseState::INVALID;
}

}
