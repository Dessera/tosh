#pragma once

#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/utils/redirect.hpp"

#include <array>
#include <memory>
#include <optional>

namespace tosh::ast {

class RedirectSrc : public Token
{
private:
  int _src;

public:
  constexpr static std::optional<int> validate(char c)
  {
    return c >= '0' && c <= '9' ? std::optional(c - '0') : std::nullopt;
  }

  RedirectSrc();

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;

  [[nodiscard]] constexpr int to_fd() const { return _src; }
};

class RedirectOp : public Token
{
public:
  constexpr static std::array VALID_OPCS = { '<', '>', '&' };

private:
  std::string _op;

public:
  constexpr static bool validate(char c) { return c == '<' || c == '>'; }

  RedirectOp();

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;

  [[nodiscard]] utils::RedirectOpType to_optype() const;
};

class RedirectDest : public Text
{
public:
  RedirectDest();
};

class Redirect : public Token
{
private:
  std::shared_ptr<RedirectSrc> _src{ nullptr };
  std::shared_ptr<RedirectOp> _op{ nullptr };
  std::shared_ptr<RedirectDest> _dest{ nullptr };

public:
  constexpr static bool validate(char c)
  {
    return RedirectSrc::validate(c) || RedirectOp::validate(c);
  }

  Redirect();

  ParseState on_continue(char c) override;

  [[nodiscard]] constexpr bool is_complete() const { return _op && _dest; }
  [[nodiscard]] std::optional<utils::RedirectOp> to_op() const;
};

}
