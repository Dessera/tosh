#pragma once

#include <magic_enum/magic_enum.hpp>

#include <concepts>
#include <cstdint>
#include <format>
#include <memory>
#include <string>

namespace tosh::parser {

class TokenParseContext;

enum class ParseState : uint8_t
{
  CONTINUE, // Continue parsing
  END,      // End parsing (current char is not complete)
  END_PASS, // End parsing (current char is complete)
  INVALID,  // Invalid parse, need to handle
  REPEAT    // Repeat parsing
};
}

namespace tosh::ast {

enum class TokenType : uint8_t
{
  ROOT,          // Token Tree Root
  TEXT,          // Normal Text
  BACKSLASH,     // Backslash Escape
  QUOTE,         // Any Quote
  REDIRECT,      // Redirect
  REDIRECT_SRC,  // Redirect Source
  REDIRECT_OP,   // Redirect Operator
  REDIRECT_DEST, // Redirect Destination
  EXPR           // Normal Expression (Top level elements)
};

class IToken
{
public:
  using Ptr = std::shared_ptr<IToken>;
  using WeakPtr = std::weak_ptr<IToken>;

private:
  TokenType _type;

public:
  IToken(TokenType type);
  virtual ~IToken() = default;

  [[nodiscard]] TokenType type() const { return _type; }

  virtual parser::ParseState on_continue(parser::TokenParseContext& ctx,
                                         char c) = 0;
  virtual parser::ParseState on_invalid(parser::TokenParseContext& ctx, char c);

  [[nodiscard]] virtual std::string string() const;
};

}

template<std::derived_from<tosh::ast::IToken> Derived, typename CharT>
struct std::formatter<Derived, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const Derived& token, std::format_context& ctx) const
  {
    return std::format_to(
      ctx.out(), "{}:{}", magic_enum::enum_name(token.type()), token.string());
  }
};