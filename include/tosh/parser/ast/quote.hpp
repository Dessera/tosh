#pragma once

#include "tosh/parser/ast/base.hpp"

#include <magic_enum/magic_enum.hpp>

#include <cstddef>
#include <string>

namespace tosh::ast {

enum QuoteType : char
{
  SINGLE = '\'',
  DOUBLE = '"',
};

class QuoteBackslashToken : public BaseToken
{
private:
  char _bs_char{};
  QuoteType _quote;

public:
  constexpr static bool validate(char c) { return c == '\\'; }

  QuoteBackslashToken(QuoteType quote, size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class QuoteTextToken : public BaseToken
{
private:
  std::string _text;
  QuoteType _quote;

public:
  QuoteTextToken(QuoteType quote, size_t level = 0);
  QuoteTextToken(std::string text, QuoteType quote, size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

class QuoteToken : public BaseToken
{
private:
  QuoteType _quote;

public:
  constexpr static auto validate(char c)
  {
    return magic_enum::enum_cast<QuoteType>(c);
  }

  QuoteToken(QuoteType quote, size_t level = 0);

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
};

}
