#pragma once

#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/utils/redirect.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

namespace tosh::ast {

class RedirectSrcToken : public BaseToken
{
private:
  int _src;

public:
  constexpr static std::optional<int> validate(char c)
  {
    return c >= '0' && c <= '9' ? std::optional(c - '0') : std::nullopt;
  }

  RedirectSrcToken(size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;

  [[nodiscard]] constexpr int to_fd() const { return _src; }
};

class RedirectOpToken : public BaseToken
{
public:
  constexpr static std::array VALID_OPCS = { '<', '>', '&' };
  constexpr static std::array VALID_OPS = { "<", ">", ">>", ">&", "<&" };

private:
  std::string _op;

public:
  constexpr static bool validate(char c) { return c == '<' || c == '>'; }

  RedirectOpToken(size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;

  [[nodiscard]] utils::RedirectOpType to_optype() const;

private:
  static utils::RedirectOpType str_to_optype(std::string_view str);
};

class RedirectDestToken : public TextToken
{
public:
  RedirectDestToken(size_t level = 0);
};

class RedirectToken : public BaseToken
{
private:
  std::shared_ptr<RedirectSrcToken> _src{ nullptr };
  std::shared_ptr<RedirectOpToken> _op{ nullptr };
  std::shared_ptr<RedirectDestToken> _dest{ nullptr };

public:
  constexpr static bool validate(char c)
  {
    return RedirectSrcToken::validate(c) || RedirectOpToken::validate(c);
  }

  RedirectToken(size_t level = 0);

  ParseState on_continue(char c) override;

  [[nodiscard]] constexpr bool is_complete() const { return _op && _dest; }
  [[nodiscard]] std::optional<utils::RedirectOp> to_op() const;
};

}
