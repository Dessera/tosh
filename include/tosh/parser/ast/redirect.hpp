#pragma once

#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/text.hpp"
#include <array>
#include <cstddef>
#include <memory>

namespace tosh::ast {

class RedirectSrcToken : public BaseToken
{
private:
  std::string _src;

public:
  RedirectSrcToken(size_t level = 0);

  ParseState handle_char(char c) override;
  [[nodiscard]] std::string to_string() const override;
};

class RedirectOpToken : public BaseToken
{
public:
  constexpr static std::array VALID_OPCS = { '<', '>', '&' };
  constexpr static std::array VALID_OPS = { "<", ">", ">>", ">&", "<&" };

private:
  std::string _op;

public:
  RedirectOpToken(size_t level = 0);

  ParseState handle_char(char c) override;
  [[nodiscard]] std::string to_string() const override;
};

class RedirectToken : public BaseToken
{
private:
  std::shared_ptr<RedirectSrcToken> _src{ nullptr };
  std::shared_ptr<RedirectOpToken> _op{ nullptr };
  std::shared_ptr<TextToken> _dest{ nullptr };

public:
  RedirectToken(size_t level = 0);

  ParseState handle_char(char c) override;
  ParseState handle_invalid() override;
};

}
