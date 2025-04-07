#pragma once

#include "tosh/utils/node.hpp"

#include <magic_enum/magic_enum.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <ranges>
#include <string>

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

enum class ParseState : uint8_t
{
  CONTINUE, // Continue parsing
  END,      // End parsing (current char is not complete)
  END_PASS, // End parsing (current char is complete)
  INVALID,  // Invalid parse, need to handle
  REPEAT    // Repeat parsing
};

class BaseToken
  : public utils::INode<BaseToken>
  , public utils::ICursor<BaseToken>
{
protected:
  // NOLINTNEXTLINE
  TokenType _type;
  // NOLINTNEXTLINE
  size_t _level;

public:
  BaseToken(TokenType type, size_t level = 0);
  virtual ~BaseToken() = default;

  [[nodiscard]] virtual std::string string() const;
  virtual ParseState on_invalid(char c);
  virtual ParseState on_end();
  virtual ParseState on_continue(char c) = 0;

  [[nodiscard]] constexpr TokenType type() const { return _type; }
  constexpr void type(TokenType type) { _type = type; }
  [[nodiscard]] constexpr size_t level() const { return _level; }

  ParseState iter_next(char c);
};

}

template<std::derived_from<tosh::ast::BaseToken> Derived, typename CharT>
struct std::formatter<Derived, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const Derived& token, std::format_context& ctx) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    if (token.empty()) {
      return std::format_to(ctx.out(),
                            "{}{}: {}",
                            std::string(token.level() * 2, ' '),
                            magic_enum::enum_name(token.type()),
                            token.string());
    }

    auto children = token.nodes() | views::transform([](const auto& token) {
                      return std::format("{}", *token);
                    }) |
                    views::join_with('\n') | ranges::to<std::string>();

    return std::format_to(ctx.out(),
                          "{}{}: \n{}",
                          std::string(token.level() * 2, ' '),
                          magic_enum::enum_name(token.type()),
                          children);
  }
};