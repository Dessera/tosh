#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/utils/redirect.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

namespace tosh::ast {

// NOLINTNEXTLINE
RedirectSrcToken::RedirectSrcToken(size_t level)
  : BaseToken(TokenType::REDIRECT_SRC, level)
{
}

ParseState
RedirectSrcToken::on_continue(char c)
{
  if (auto res = validate(c); res.has_value()) {
    // NOLINTNEXTLINE
    _src = _src * 10 + res.value();
    return ParseState::CONTINUE;
  }

  return ParseState::END;
}

std::string
RedirectSrcToken::string() const
{
  return std::to_string(_src);
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

utils::RedirectOpType
RedirectOpToken::to_optype() const
{
  return str_to_optype(_op);
}

utils::RedirectOpType
RedirectOpToken::str_to_optype(std::string_view str)
{
  if (str == "<") {
    return utils::RedirectOpType::IN;
  }

  if (str == ">") {
    return utils::RedirectOpType::OUT;
  }

  if (str == ">>") {
    return utils::RedirectOpType::APPEND;
  }

  if (str == "<<") {
    return utils::RedirectOpType::HEREDOC;
  }

  if (str == "<&") {
    return utils::RedirectOpType::IN_MERGE;
  }

  if (str == ">&") {
    return utils::RedirectOpType::OUT_MERGE;
  }

  [[unlikely]] return utils::RedirectOpType::UNKNOWN;
}

RedirectDestToken::RedirectDestToken(size_t level)
  : TextToken(level)
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
    if (RedirectSrcToken::validate(c).has_value() && _src == nullptr) {
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

std::optional<utils::RedirectOp>
RedirectToken::to_op() const
{
  if (!is_complete()) {
    return std::nullopt;
  }

  int fd = -1;
  auto op_type = _op->to_optype();

  if (_src == nullptr) {
    if (op_type == utils::RedirectOpType::IN ||
        op_type == utils::RedirectOpType::HEREDOC ||
        op_type == utils::RedirectOpType::IN_MERGE) {
      fd = 0;
    } else {
      fd = 1;
    }
  } else {
    fd = _src->to_fd();
  }

  return utils::RedirectOp(fd, _dest->string(), op_type);
};

}
