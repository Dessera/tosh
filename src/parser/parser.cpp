#include "tosh/terminal/event/parser.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/repl.hpp"
#include "tosh/utils/redirect.hpp"

#include <cstdio>
#include <memory>
#include <print>
#include <ranges>
#include <sys/types.h>
#include <termios.h>
#include <vector>

using tosh::ast::Token;
using tosh::ast::TokenFilter;
using tosh::ast::TokenType;

using tosh::utils::RedirectFactory;
using tosh::utils::RedirectOperation;

namespace {

using namespace tosh;

auto
make_redirects(Token::Ptr& root)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto redirects =
    root->find_all(TokenFilter<TokenType::REDIRECT>()) |
    views::transform([](const auto& token) {
      auto ptr = std::static_pointer_cast<ast::Redirect>(token);
      return ptr->to_op();
    }) |
    views::filter([](const auto& op) { return op.has_value(); }) |
    views::transform(
      [](const auto& op) { return RedirectFactory::create(op.value()); }) |
    ranges::to<std::vector<std::shared_ptr<RedirectOperation>>>();

  root->remove_all([](const auto& token) {
    return token.type() == TokenType::REDIRECT ||
           (token.nodes().size() == 1 &&
            token.nodes().front()->type() == TokenType::REDIRECT);
  });

  return redirects;
}

constexpr void
make_home_dir(Token::Ptr& root)
{
  root->replace_all(TokenFilter<TokenType::HOME>(), [](const auto& token) {
    auto ptr = std::static_pointer_cast<ast::HomeDir>(token);
    return std::make_shared<ast::Text>(ptr->home());
  });
}

error::Result<void>
handle_completion(repl::Repl& repl, Token::Ptr& root)
{
  auto& doc = repl.doc();

  if (!doc.end()) {
    return {};
  }

  root->current()->parse_next('\0');

  auto curr = root->current()->string();
  auto res = repl.find_fuzzy(curr);

  if (!res.empty() && res.front().size() > curr.size()) {
    auto s = res.front().substr(curr.size());

    for (auto c : s) {
      RETERR(doc.insert(c));
    }

    root->current()->current(std::make_shared<ast::Text>(s));
  }

  return {};
}

error::Result<bool>
handle_user_input(repl::Repl& repl,
                  Token::Ptr& root,
                  const terminal::EventGetString& event)
{
  auto& doc = repl.doc();

  for (auto c : event.str) {
    RETERR(doc.insert(c));
  }

  if (!doc.end()) {
    root->clear();
  }

  const auto& str = doc.end() ? event.str : doc.string();

  for (auto c : str) {
    if (root->parse_next(c) == ast::ParseState::END) {
      return true;
    }
  }

  return false;
}

error::Result<void>
handle_move_cursor(repl::Repl& repl,
                   Token::Ptr& /*root*/,
                   const terminal::EventMoveCursor& event)
{
  auto& doc = repl.doc();

  if (event.direction == terminal::EventMoveCursor::Direction::LEFT) {
    RETERR(doc.backward());
  } else if (event.direction == terminal::EventMoveCursor::Direction::RIGHT) {
    RETERR(doc.forward());
  }

  return {};
}

error::Result<bool>
handle_special_key(repl::Repl& repl,
                   Token::Ptr& root,
                   const terminal::EventSpecialKey& event)
{
  auto& doc = repl.doc();

  if (event.key == terminal::EventSpecialKey::Key::BACKSPACE) {
    RETERR(doc.remove());
    root->clear();
    for (auto c : doc.string()) {
      if (root->parse_next(c) == ast::ParseState::END) {
        return true;
      }
    }
  } else if (event.key == terminal::EventSpecialKey::Key::TAB) {
    RETERR(handle_completion(repl, root));
  } else if (event.key == terminal::EventSpecialKey::Key::KEOF) {
    root->clear();
    root->current(std::make_shared<ast::Text>("exit"));
    return true;
  }

  return false;
}

error::Result<bool>
handle_op(repl::Repl& repl, Token::Ptr& root, const terminal::Event& event)
{

  if (const auto* ev = std::get_if<terminal::EventGetString>(&event);
      ev != nullptr) {
    return UNWRAPERR(handle_user_input(repl, root, *ev));
  }
  if (const auto* ev = std::get_if<terminal::EventMoveCursor>(&event);
      ev != nullptr) {
    RETERR(handle_move_cursor(repl, root, *ev));
    return false;
  }
  if (const auto* ev = std::get_if<terminal::EventSpecialKey>(&event);
      ev != nullptr) {
    return UNWRAPERR(handle_special_key(repl, root, *ev));
  }

  return false;
}

}

namespace tosh::parser {

error::Result<ParseQuery>
parse(repl::Repl& repl)
{
  auto& doc = repl.doc();
  RETERR(doc.enter());

  Token::Ptr root = std::make_shared<ast::Root>();

  while (true) {
    auto event = UNWRAPERR(doc.get_op());
    if (UNWRAPERR(handle_op(repl, root, event))) {
      break;
    }
  }

  root->parse_next('\0');

  make_home_dir(root);

  return ParseQuery{ root, make_redirects(root) };
}

}
