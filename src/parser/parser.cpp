#include "tosh/parser/parser.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/repl.hpp"
#include "tosh/utils/buffer.hpp"
#include "tosh/utils/redirect.hpp"

#include <cstdio>
#include <exception>
#include <iostream>
#include <memory>
#include <print>
#include <ranges>
#include <string>
#include <sys/types.h>
#include <termios.h>
#include <vector>

namespace {

constexpr bool
is_redirect(const tosh::ast::Token& token)
{
  return token.type() == tosh::ast::TokenType::REDIRECT;
}

constexpr bool
is_redirect_expr(const tosh::ast::Token& token)
{
  return is_redirect(token) ||
         (token.nodes().size() == 1 && is_redirect(*token.nodes().front()));
}

}

namespace tosh::parser {

error::Result<ParseQuery>
TokenParser::parse(repl::Repl& repl)
try {
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto root = std::make_shared<ast::Root>();
  auto buffer = utils::CommandBuffer{ std::cout };

  while (true) {
    char c = buffer.getchar();

    if (utils::is_ascii(c) ||
        utils::is_command(c, utils::CommandType::NEXT_LINE)) {
      // Insert character, need rebuild AST if not at the end of buffer
      if (!buffer.end()) {
        root->clear();
        for (auto c : buffer.string()) {
          root->parse_next(c);
        }
        continue;
      }

      if (root->parse_next(c) == ast::ParseState::END) {
        break;
      }
    }

    if (utils::is_command(c, utils::CommandType::BACKSPACE)) {
      root->clear();
      for (auto c : buffer.string()) {
        root->parse_next(c);
      }
      continue;
    }

    if (utils::is_command(c, utils::CommandType::TAB) &&
        root->current() != nullptr) {
      root->current()->parse_next('\0');
      auto curr = root->current()->string();
      auto res = repl.find_fuzzy(curr);
      if (!res.empty()) {
        auto s = res.front().substr(curr.size());
        buffer.insert(s);
        root->current()->current(std::make_shared<ast::Text>(s));
      }
    }

    if (utils::is_command(c, utils::CommandType::END) && root->empty()) {
      root->add(std::make_shared<ast::Text>("exit"));
      break;
    }
  }
  root->parse_next('\0');

  auto redirects =
    root->find_all(is_redirect) | views::transform([](const auto& token) {
      auto ptr = std::static_pointer_cast<ast::Redirect>(token);
      return ptr->to_op();
    }) |
    views::filter([](const auto& op) { return op.has_value(); }) |
    views::transform([](const auto& op) {
      return utils::RedirectFactory::create(op.value());
    }) |
    ranges::to<std::vector<std::shared_ptr<utils::RedirectOperation>>>();
  root->remove_all(is_redirect_expr);

  return ParseQuery{ root, redirects };
} catch (const std::exception& e) {
  return error::err(error::ErrorCode::UNKNOWN, e.what());
}

}
