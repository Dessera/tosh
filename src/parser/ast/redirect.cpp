#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/utils/redirect.hpp"

#include <algorithm>
#include <memory>
#include <optional>

namespace tosh::ast {

// NOLINTNEXTLINE
RedirectSrc::RedirectSrc()
  : Token(TokenType::REDIRECT_SRC)
{
}

ParseState
RedirectSrc::on_continue(char c)
{
  if (c == '\n') {
    return ParseState::INVALID;
  }

  if (auto res = validate(c); res.has_value()) {
    // NOLINTNEXTLINE
    _src = _src * 10 + res.value();
    return ParseState::CONTINUE;
  }

  return ParseState::END;
}

std::string
RedirectSrc::string() const
{
  return std::to_string(_src);
}

// NOLINTNEXTLINE
RedirectOp::RedirectOp()
  : Token(TokenType::REDIRECT_OP)
{
}

ParseState
RedirectOp::on_continue(char c)
{
  if (std::ranges::find(VALID_OPCS, c) != VALID_OPCS.end()) {
    _op += c;
    return ParseState::CONTINUE;
  }

  if (std::ranges::find(utils::REDIRECT_OP_STRS, _op) !=
      utils::REDIRECT_OP_STRS.end()) {
    return ParseState::END;
  }

  return ParseState::INVALID;
}

std::string
RedirectOp::string() const
{
  return _op;
}

utils::RedirectType
RedirectOp::to_optype() const
{
  return utils::redirect_str_to_type(_op);
}

RedirectDest::RedirectDest()
{
  type(TokenType::REDIRECT_DEST);
}

// NOLINTNEXTLINE
Redirect::Redirect()
  : Token(TokenType::REDIRECT)
{
}

ParseState
Redirect::on_continue(char c)
{
  if (_op == nullptr) {
    if (RedirectSrc::validate(c).has_value() && _src == nullptr) {
      _src = std::make_shared<RedirectSrc>();
      current(_src);
      return ParseState::REPEAT;
    }

    if (RedirectOp::validate(c)) {
      _op = std::make_shared<RedirectOp>();
      current(_op);
      return ParseState::REPEAT;
    }

    return ParseState::INVALID;
  }

  if (_dest == nullptr) {
    if (c == ' ' || c == '\n') {
      return ParseState::CONTINUE;
    }

    _dest = std::make_shared<RedirectDest>();
    current(_dest);
    return ParseState::REPEAT;
  }

  return ParseState::END;
}

std::optional<utils::Redirect>
Redirect::to_op() const
{
  if (!is_complete()) {
    return std::nullopt;
  }

  int fd = -1;
  auto op_type = _op->to_optype();

  if (_src == nullptr) {
    if (op_type == utils::RedirectType::IN ||
        op_type == utils::RedirectType::HEREDOC ||
        op_type == utils::RedirectType::IN_MERGE) {
      fd = 0;
    } else {
      fd = 1;
    }
  } else {
    fd = _src->to_fd();
  }

  return utils::Redirect(fd, _dest->string(), op_type);
};

}
