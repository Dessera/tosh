#pragma once

#include "tosh/parser/ast/base.hpp"

#include <magic_enum/magic_enum.hpp>

#include <string>

namespace tosh::ast {

enum QuoteType : char
{
  SINGLE = '\'',
  DOUBLE = '"',
};

class QuoteBackslash : public Token
{
private:
  char _bs_char{};
  QuoteType _quote;

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  QuoteBackslash(QuoteType quote);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class QuoteText : public Token
{
private:
  std::string _text;
  QuoteType _quote;

public:
  QuoteText(QuoteType quote);
  QuoteText(std::string text, QuoteType quote);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class QuoteExpr : public Token
{
private:
  QuoteType _quote;

public:
  constexpr static auto validate(char c)
  {
    return magic_enum::enum_cast<QuoteType>(c);
  }

  QuoteExpr(QuoteType quote);

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
};

}
