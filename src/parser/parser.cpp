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
  ast::Root::Ptr root = std::make_shared<ast::Root>();
  auto buffer = utils::CommandBuffer{ std::cout };

  while (true) {
    char c = buffer.getchar();

    if (utils::is_ascii(c) ||
        utils::is_command(c, utils::CommandType::NEXT_LINE)) {
      if (!buffer.end()) {
        handle_rebuild_ast(root, buffer);
        continue;
      }

      if (root->parse_next(c) == ast::ParseState::END) {
        break;
      }
    }

    if (utils::is_command(c, utils::CommandType::BACKSPACE)) {
      handle_rebuild_ast(root, buffer);
      continue;
    }

    if (utils::is_command(c, utils::CommandType::TAB) &&
        root->current() != nullptr) {
      handle_completion(repl, root, buffer);
      continue;
    }

    if (utils::is_command(c, utils::CommandType::END) && root->empty()) {
      handle_cin_eof(root, buffer);
      break;
    }
  }
  root->parse_next('\0');

  return ParseQuery{ root, make_redirects(root) };
} catch (const std::exception& e) {
  return error::err(error::ErrorCode::UNKNOWN, e.what());
}

void
TokenParser::handle_completion(repl::Repl& repl,
                               ast::Root::Ptr& root,
                               utils::CommandBuffer& buffer)
{
  // end current token
  root->current()->parse_next('\0');

  auto curr = root->current()->string();
  auto res = repl.find_fuzzy(curr);

  if (!res.empty() && res.front().size() > curr.size()) {
    auto s = res.front().substr(curr.size());
    buffer.insert(s);
    root->current()->current(std::make_shared<ast::Text>(s));
  }
}

void
TokenParser::handle_rebuild_ast(ast::Root::Ptr& root,
                                utils::CommandBuffer& buffer)
{
  root->clear();
  for (auto c : buffer.string()) {
    root->parse_next(c);
  }
}

void
TokenParser::handle_cin_eof(ast::Root::Ptr& root, utils::CommandBuffer& buffer)
{
  root->add(std::make_shared<ast::Text>("exit"));
  buffer.insert("exit");
}

std::vector<std::shared_ptr<utils::RedirectOperation>>
TokenParser::make_redirects(ast::Root::Ptr& root)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

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

  return redirects;
}
}
