#include "tosh/parser/parser.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
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

bool
is_normal_char(char c)
{
  return (c >= ' ' && c <= '~') || c == '\t';
}

bool
is_input_cmd(char c, tosh::parser::InputCommand cmd)
{
  return c == static_cast<char>(cmd);
}

}

namespace tosh::parser {

error::Result<ParseQuery>
TokenParser::parse()
try {
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;
  enable_raw_stdin();

  auto root = std::make_shared<ast::Root>();

  while (true) {
    auto buffer = read_line();

    for (auto& c : buffer) {
      root->iter_next(c);
    }

    if (root->iter_next('\n') == ast::ParseState::END) {
      break;
    }
  }
  root->iter_next('\0');

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

  disable_raw_stdin();
  return ParseQuery{ root, redirects };
} catch (const std::exception& e) {
  disable_raw_stdin();
  return error::err(error::ErrorCode::UNKNOWN, e.what());
}
void
TokenParser::enable_raw_stdin()
{
  // NOLINTNEXTLINE
  termios trim;
  tcgetattr(STDIN_FILENO, &trim);
  trim.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &trim);
}

void
TokenParser::disable_raw_stdin()
{
  // NOLINTNEXTLINE
  termios trim;
  tcgetattr(STDIN_FILENO, &trim);
  trim.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &trim);
}

std::string
TokenParser::read_line()
{
  utils::CommandBuffer buffer{ std::cout };
  bool escape = false;
  bool lwin = false;

  while (true) {
    char c = static_cast<char>(std::getchar());

    // if special ctrl character
    if (is_input_cmd(c, InputCommand::NEXTLINE)) {
      buffer.stream_insert(c);
      break;
    }

    // bacspace
    if (is_input_cmd(c, InputCommand::BACKSPACE)) {
      buffer.remove(1);
    }

    if (is_input_cmd(c, InputCommand::LWIN)) {
      lwin = true;
      continue;
    }

    if (is_input_cmd(c, InputCommand::ESCAPE)) {
      escape = true;
      continue;
    }

    // normal character
    if (is_normal_char(c)) {
      if (escape && lwin) {
        if (c == 'D') {
          buffer.backward(1);
        } else if (c == 'C') {
          buffer.forward(1);
        }
      } else {
        buffer.insert(c);
      }
    }

    lwin = false;
    escape = false;
  }

  return buffer.get();
}

}
