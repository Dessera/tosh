#pragma once

#include "tosh/common.hpp"
#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class TOSH_EXPORT Backslash : public Token
{
private:
  char _bs_token{};

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  Backslash();

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class TOSH_EXPORT Text : public Token
{
private:
  std::string _str;

public:
  Text();
  Text(std::string str);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class TOSH_EXPORT HomeDir : public Token
{
private:
  bool _slash{ false };

public:
  constexpr static bool validate(char c) { return c == '~'; }

  HomeDir();

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
  [[nodiscard]] std::string home() const;
};

class TOSH_EXPORT Expr : public Token
{
public:
  Expr();

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
  ParseState on_end() override;
};

}
