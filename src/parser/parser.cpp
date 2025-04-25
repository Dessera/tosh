#include "tosh/terminal/event/parser.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/repl.hpp"
#include "tosh/utils/redirect.hpp"

#include <cstdio>
#include <exception>
#include <memory>
#include <print>
#include <ranges>
#include <string>
#include <sys/types.h>
#include <termios.h>
#include <variant>
#include <vector>

namespace {

constexpr bool
is_redirect(const tosh::ast::Token& token)
{
  return token.type() == tosh::ast::TokenType::REDIRECT;
}

constexpr bool
is_home(const tosh::ast::Token& token)
{
  return token.type() == tosh::ast::TokenType::HOME;
}

constexpr bool
is_redirect_expr(const tosh::ast::Token& token)
{
  return is_redirect(token) ||
         (token.nodes().size() == 1 && is_redirect(*token.nodes().front()));
}

constexpr auto
home_to_text(const tosh::ast::Token::Ptr& token)
{
  auto ptr = std::static_pointer_cast<tosh::ast::HomeDir>(token);
  return std::make_shared<tosh::ast::Text>(ptr->home());
}

}

namespace tosh::parser {

error::Result<ParseQuery>
TokenParser::parse(repl::Repl& repl)
try {
  auto& doc = repl.doc();

  ast::Root::Ptr root = std::make_shared<ast::Root>();

  while (true) {
    auto op = UNWRAPERR(doc.get_op());

    if (auto* ev = std::get_if<terminal::EventGetString>(&op); ev != nullptr) {
      for (auto c : ev->str) {
        RETERR(doc.insert(c));
      }
      handle_rebuild_ast(repl, root);
    } else if (auto* ev = std::get_if<terminal::EventMoveCursor>(&op);
               ev != nullptr) {
      if (ev->direction == terminal::EventMoveCursor::Direction::LEFT) {
        RETERR(doc.backward());
      } else if (ev->direction == terminal::EventMoveCursor::Direction::RIGHT) {
        RETERR(doc.forward());
      }
    } else if (auto* ev = std::get_if<terminal::EventSpecialKey>(&op);
               ev != nullptr) {
      if (ev->key == terminal::EventSpecialKey::Key::BACKSPACE) {
        RETERR(doc.remove());
      }
    }
  }
  root->parse_next('\0');

  root->replace_all(is_home, home_to_text);

  auto _ = doc.leave();
  return ParseQuery{ root, make_redirects(root) };
} catch (const std::exception& e) {
  return error::err(error::ErrorCode::UNKNOWN, e.what());
}

// void
// TokenParser::handle_completion(repl::Repl& repl,
//                                ast::Root::Ptr& root,
//                                utils::CommandBuffer& buffer)
// {
//   // end current token
//   root->current()->parse_next('\0');

//   auto curr = root->current()->string();
//   auto res = repl.find_fuzzy(curr);

//   if (!res.empty() && res.front().size() > curr.size()) {
//     auto s = res.front().substr(curr.size());
//     buffer.insert(s);
//     root->current()->current(std::make_shared<ast::Text>(s));
//   }
// }

void
TokenParser::handle_rebuild_ast(repl::Repl& repl, ast::Root::Ptr& root)
{
  auto& doc = repl.doc();

  root->clear();
  for (auto c : doc.string()) {
    root->parse_next(c);
  }
}

// void
// TokenParser::handle_cin_eof(ast::Root::Ptr& root, utils::CommandBuffer&
// buffer)
// {
//   root->add(std::make_shared<ast::Text>("exit"));
//   buffer.insert("exit");
// }

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
