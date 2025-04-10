#include "tosh/parser/parser.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/redirect.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/utils/redirect.hpp"

#include <filesystem>
#include <istream>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <string>
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

std::optional<std::string>
detect_command(std::string_view command)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto* envpath_cstr = std::getenv("PATH");
  auto envpath =
    envpath_cstr == nullptr ? std::string() : std::string(envpath_cstr);

  auto path_list = envpath | views::split(':') |
                   views::transform([](const auto& item) {
                     return std::string(item.begin(), item.end());
                   }) |
                   ranges::to<std::vector<std::string>>();

  for (auto& path : path_list) {
    auto full_path = std::filesystem::path(path) / command;

    if (std::filesystem::exists(full_path)) {
      return full_path;
    }
  }

  if (std::filesystem::exists(command)) {
    return std::string(command);
  }

  return std::nullopt;
}

ParseQuery
TokenParser::parse(std::istream& input)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;
  auto root = std::make_shared<ast::Root>();

  std::string buffer{};
  std::getline(input, buffer);

  for (auto& c : buffer) {
    root->iter_next(c);
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

  return { root, redirects };
}

}
