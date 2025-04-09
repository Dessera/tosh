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

class BaseToken : public utils::INode<BaseToken>
{
protected:
  // NOLINTNEXTLINE
  TokenType _type;

public:
  BaseToken(TokenType type);
  virtual ~BaseToken() = default;

  [[nodiscard]] virtual std::string string() const;
  virtual ParseState on_invalid(char c);
  virtual ParseState on_end();
  virtual ParseState on_continue(char c) = 0;

  [[nodiscard]] constexpr TokenType type() const { return _type; }
  constexpr void type(TokenType type) { _type = type; }

  ParseState iter_next(char c);
};

}

template<std::derived_from<tosh::ast::BaseToken> Derived, typename CharT>
struct std::formatter<Derived, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const Derived& token, std::format_context& ctx) const
  {
    return std::format_to(ctx.out(), "{}", format_impl(&token, 0));
  }

private:
  std::string format_impl(const tosh::ast::BaseToken* token, size_t level) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    if (token == nullptr) {
      return {};
    }

    if (token->empty()) {
      return std::format("{}{}: {}",
                         std::string(level * 2, ' '),
                         magic_enum::enum_name(token->type()),
                         token->string());
    }

    auto children = token->nodes() | views::transform([&](const auto& token) {
                      return format_impl(token.get(), level + 1);
                    }) |
                    views::join_with('\n') | ranges::to<std::string>();

    return std::format("{}{}: \n{}",
                       std::string(level * 2, ' '),
                       magic_enum::enum_name(token->type()),
                       children);
  }
};