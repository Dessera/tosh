#pragma once

#include "tosh/parser/ast/base.hpp"

#include <cstddef>

namespace tosh::ast {

class BackslashToken : public BaseToken
{
private:
  char _bs_token{};

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  BackslashToken(size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class TextToken : public BaseToken
{
private:
  std::string _str;

public:
  TextToken(size_t level = 0);
  TextToken(std::string str, size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class ExprToken : public BaseToken
{
public:
  ExprToken(size_t level = 0);

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
  ParseState on_end() override;
};

}
