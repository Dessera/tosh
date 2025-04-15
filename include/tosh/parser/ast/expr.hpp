#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class Backslash : public Token
{
private:
  char _bs_token{};

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  Backslash();

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class Text : public Token
{
private:
  std::string _str;

public:
  Text();
  Text(std::string str);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class Expr : public Token
{
public:
  Expr();

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
  ParseState on_end() override;
};

}
