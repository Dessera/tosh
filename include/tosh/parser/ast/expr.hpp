#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class Backslash : public IToken
{
private:
  char _bs_token{};

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  Backslash();

  parser::ParseState on_continue(parser::TokenParseContext& ctx,
                                 char c) override;
  [[nodiscard]] std::string string() const override;
};

class Text : public IToken
{
private:
  std::string _str;

public:
  Text();
  Text(std::string str);

  parser::ParseState on_continue(parser::TokenParseContext& ctx,
                                 char c) override;
  [[nodiscard]] std::string string() const override;
};

class Expr : public IToken
{
public:
  Expr();

private:
  parser::ParseState on_continue(parser::TokenParseContext& ctx,
                                 char c) override;
  parser::ParseState on_invalid(parser::TokenParseContext& ctx,
                                char c) override;
};

}
