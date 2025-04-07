#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/text.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>

namespace tosh::ast {

// NOLINTNEXTLINE
RedirectSrcToken::RedirectSrcToken(size_t level)
  : BaseToken(TokenType::REDIRECT_SRC, level)
{
}

ParseState
RedirectSrcToken::on_continue(char c)
{
  if (validate(c)) {
    _src += c;
    return ParseState::CONTINUE;
  }

  return ParseState::END;
}

std::string
RedirectSrcToken::string() const
{
  return _src;
}

// NOLINTNEXTLINE
RedirectOpToken::RedirectOpToken(size_t level)
  : BaseToken(TokenType::REDIRECT_OP, level)
{
}

ParseState
RedirectOpToken::on_continue(char c)
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
RedirectOpToken::string() const
{
  return _op;
}

RedirectDestToken::RedirectDestToken(size_t level)
  : TextToken('\0', level)
{
  type(TokenType::REDIRECT_DEST);
}

// NOLINTNEXTLINE
RedirectToken::RedirectToken(size_t level)
  : BaseToken(TokenType::REDIRECT, level)
{
}

ParseState
RedirectToken::on_continue(char c)
{
  if (_op == nullptr) {
    if (RedirectSrcToken::validate(c) && _src == nullptr) {
      _src = std::make_shared<RedirectSrcToken>(level() + 1);
      current(_src);
      return ParseState::REPEAT;
    }

    if (RedirectOpToken::validate(c)) {
      _op = std::make_shared<RedirectOpToken>(level() + 1);
      current(_op);
      return ParseState::REPEAT;
    }

    return ParseState::INVALID;
  }

  if (_dest == nullptr) {
    if (c == ' ') {
      return ParseState::CONTINUE;
    }

    _dest = std::make_shared<RedirectDestToken>(level() + 1);
    current(_dest);
    return ParseState::REPEAT;
  }

  return ParseState::END;
}

}
