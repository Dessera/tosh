#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <vector>

namespace tosh::ast {

enum class TokenType : uint8_t
{
  ROOT,         // Token Tree Root
  TEXT,         // Normal Text
  BACKSLASH,    // Backslash Escape
  QUOTE,        // Any Quote
  REDIRECT,     // Redirect
  REDIRECT_SRC, // Redirect Source
  REDIRECT_OP,  // Redirect Operator
  REDIRECT_DST, // Redirect Destination
  EXPR          // Normal Expression (Top level elements)
};

const char*
token_type_to_string(TokenType type);

enum class ParseState : uint8_t
{
  CONTINUE, // Continue parsing
  END,      // End parsing (current char is not complete)
  END_PASS, // End parsing (current char is complete)
  INVALID,  // Invalid parse, need to handle
  REPEAT,   // Repeat parsing
  ERROR     // Error parsing
};

class BaseToken
{
protected:
  // NOLINTNEXTLINE
  TokenType _type;
  // NOLINTNEXTLINE
  size_t _level;
  // NOLINTNEXTLINE
  std::vector<std::shared_ptr<BaseToken>> _children;
  // NOLINTNEXTLINE
  std::shared_ptr<BaseToken> _current_token{ nullptr };

public:
  BaseToken(TokenType type, size_t level = 0);
  virtual ~BaseToken() = default;

  virtual ParseState handle_char(char c) = 0;
  virtual ParseState handle_invalid();
  [[nodiscard]] virtual std::string to_string() const;

  [[nodiscard]] constexpr TokenType type() const { return _type; }
  [[nodiscard]] constexpr size_t level() const { return _level; }

  [[nodiscard]] constexpr std::span<const std::shared_ptr<BaseToken>> tokens()
    const
  {
    return _children;
  }

  [[nodiscard]] constexpr bool is_empty() const { return _children.empty(); }

  ParseState parse_next(char c);
  ParseState parse_end();

protected:
  void submit_current_token();
  void set_current_token(std::shared_ptr<BaseToken> token);
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

    if (token.is_empty()) {
      return std::format_to(ctx.out(),
                            "{}{}: {}",
                            std::string(token.level() * 2, ' '),
                            token_type_to_string(token.type()),
                            token.to_string());
    }

    auto children = token.tokens() | views::transform([](const auto& token) {
                      return std::format("{}", *token);
                    }) |
                    views::join_with('\n') | ranges::to<std::string>();

    return std::format_to(ctx.out(),
                          "{}{}: \n{}",
                          std::string(token.level() * 2, ' '),
                          token_type_to_string(token.type()),
                          children);
  }
};