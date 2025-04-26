#pragma once

#include <magic_enum/magic_enum.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <list>
#include <memory>
#include <ranges>
#include <string>

namespace tosh::ast {

/**
 * @brief Token Type (used to identify the type of token in runtime)
 */
enum class TokenType : uint8_t
{
  ROOT,          // Token Tree Root
  TEXT,          // Normal Text
  BACKSLASH,     // Backslash Escape
  QUOTE,         // Any Quote
  HOME,          // Home (~)
  REDIRECT,      // Redirect
  REDIRECT_SRC,  // Redirect Source
  REDIRECT_OP,   // Redirect Operator
  REDIRECT_DEST, // Redirect Destination
  EXPR           // Normal Expression (Top level elements)
};

/**
 * @brief Parse State
 */
enum class ParseState : uint8_t
{
  CONTINUE, // Continue parsing
  END,      // End parsing (current char is not complete)
  END_PASS, // End parsing (current char is complete)
  INVALID,  // Invalid parse, need to handle
  REPEAT    // Repeat parsing
};

/**
 * @brief Base class for all AST nodes
 */
class Token
{
public:
  using Ptr = std::shared_ptr<Token>;

private:
  TokenType _type;
  std::list<Ptr> _nodes;
  Ptr _curr{ nullptr };

public:
  Token(TokenType type);
  virtual ~Token() = default;

  /**
   * @brief Get the string representation of the token
   *
   * @return std::string String representation of the token
   */
  [[nodiscard]] virtual std::string string() const;

  /**
   * @brief Handler for invalid characters from child nodes
   *
   * @param c Invalid character
   * @return ParseState Parse state after handling the invalid character
   */
  virtual ParseState on_invalid(char c);

  /**
   * @brief Handler for the end of the token
   *
   * @return ParseState Parse state after handling the end of the token
   */
  virtual ParseState on_end();

  /**
   * @brief Handler for the current character
   *
   * @param c Current character
   * @return ParseState Parse state after handling the character
   */
  virtual ParseState on_continue(char c) = 0;

  /**
   * @brief Get the token type
   *
   * @return constexpr TokenType Token type
   */
  [[nodiscard]] constexpr auto type() const noexcept { return _type; }

  /**
   * @brief Set the token type
   *
   * @param type Token type to set
   */
  constexpr void type(TokenType type) noexcept { _type = type; }

  /**
   * @brief Push a node to the node list
   *
   * @param node Node to push
   */
  constexpr void add(const Ptr& node) { _nodes.push_back(node); }

  /**
   * @brief Pop the last node
   */
  constexpr void pop_back() noexcept { _nodes.pop_back(); }

  /**
   * @brief Get child nodes
   *
   * @return constexpr auto& Child nodes
   */
  [[nodiscard]] constexpr auto& nodes() const noexcept { return _nodes; }

  /**
   * @brief Get child nodes
   *
   * @return constexpr auto& Child nodes
   */
  [[nodiscard]] constexpr auto& nodes() noexcept { return _nodes; }

  /**
   * @brief Check if the node is empty
   *
   * @return true if the node is empty
   * @return false if the node is not empty
   */
  [[nodiscard]] bool empty() const noexcept { return _nodes.empty(); }

  /**
   * @brief Set the current node
   *
   * @param node Node to set as current
   */
  constexpr void current(const Ptr& node) noexcept { _curr = node; }

  /**
   * @brief Get the current node
   *
   * @return constexpr auto& Current node pointer
   */
  [[nodiscard]] constexpr auto& current() const noexcept { return _curr; }

  /**
   * @brief Get the current node
   *
   * @return constexpr auto& Current node pointer
   */
  [[nodiscard]] constexpr auto& current() noexcept { return _curr; }

  /**
   * @brief Parse the next character
   *
   * @param c Character to parse
   * @return ParseState Parse state after parsing the character
   */
  ParseState parse_next(char c);

  void clear();

  /**
   * @brief Find all nodes that satisfy the given predicate
   *
   * @param f Predicate function
   * @return std::vector<Ptr> Nodes that satisfy the predicate
   */
  std::vector<Ptr> find_all(auto&& f) const
  {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    auto subs =
      nodes() |
      views::transform([&f](const auto& node) { return node->find_all(f); }) |
      views::join | ranges::to<std::vector<Ptr>>();

    auto curr = nodes() |
                views::filter([&f](const auto& node) { return f(*node); }) |
                ranges::to<std::vector<Ptr>>();

    curr.insert(curr.end(), subs.begin(), subs.end());
    return curr;
  }

  /**
   * @brief Remove all nodes that satisfy the given predicate
   *
   * @param f Predicate function
   */
  void remove_all(auto&& f)
  {
    nodes().remove_if([&f](const auto& node) { return f(*node); });

    for (auto& node : nodes()) {
      node->remove_all(f);
    }
  }

  /**
   * @brief Replace all nodes that satisfy the given predicate with the given
   * replacement
   *
   * @param pred Predicate function
   * @param repl Replacement function
   */
  void replace_all(auto&& pred, auto&& repl)
  {
    for (auto& node : nodes()) {
      if (pred(*node)) {
        node = repl(node);
      }
    }

    for (auto& node : nodes()) {
      node->replace_all(pred, repl);
    }
  }
};

template<TokenType... Args>
struct TokenFilter
{
  constexpr bool operator()(const tosh::ast::Token& token) const
  {
    return (... || filter_token<Args>(token));
  }

private:
  template<TokenType Tp>
  [[nodiscard]] constexpr bool filter_token(const tosh::ast::Token& token) const
  {
    return token.type() == Tp;
  }
};

}

template<std::derived_from<tosh::ast::Token> Derived, typename CharT>
struct std::formatter<Derived, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  auto format(const Derived& token, std::format_context& ctx) const
  {
    return std::format_to(ctx.out(), "{}", format_impl(&token, 0));
  }

private:
  std::string format_impl(const tosh::ast::Token* token, size_t level) const
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

    return std::format("{}{}:\n{}",
                       std::string(level * 2, ' '),
                       magic_enum::enum_name(token->type()),
                       children);
  }
};
