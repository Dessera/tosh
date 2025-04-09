#pragma once

#include "tosh/parser/ast/base.hpp"
// #include "tosh/parser/query.hpp"
#include "tosh/utils/tree.hpp"

#include <istream>
#include <optional>
#include <stack>
#include <string_view>

namespace tosh::parser {

std::optional<std::string>
detect_command(std::string_view command);

class TokenParser
{
public:
  // ParseQuery parse(std::istream& input);
  utils::LinkedTree<ast::IToken::Ptr> parse_tokens(std::istream& input);
};

class TokenParseContext
{
public:
  using Ast = utils::LinkedTree<ast::IToken::Ptr>;
  using AstCursor = utils::LinkedTree<ast::IToken::Ptr>::Iterator;

private:
  Ast _ast;
  std::stack<AstCursor> _stack;

public:
  TokenParseContext();

  constexpr void pop() { _stack.pop(); }
  [[nodiscard]] constexpr ast::IToken::WeakPtr current() const
  {
    return *_stack.top();
  };

  [[nodiscard]] constexpr bool empty() const { return _stack.empty(); }

  template<typename T, typename... Args>
  constexpr void push(Args&&... args)
  {
    auto current = _stack.top();
    auto new_node = std::make_shared<T>(std::forward<Args>(args)...);
    _ast.insert(current, new_node);
    _stack.emplace(&current.node()->nodes().back());
  }
};

}
