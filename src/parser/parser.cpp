#include "tosh/parser/parser.hpp"
#include "tosh/parser/ast/base.hpp"
#include "tosh/parser/ast/root.hpp"
// #include "tosh/parser/query.hpp"
#include "tosh/utils/tree.hpp"

#include <filesystem>
#include <istream>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <vector>

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

utils::LinkedTree<ast::IToken::Ptr>
TokenParser::parse_tokens(std::istream& input)
{
  TokenParseContext ctx{};

  std::string buffer{};
  std::getline(input, buffer);

  for (auto& c : buffer) {
    auto res = ctx.current().lock()->on_continue(ctx, c);
  }
}

TokenParseContext::TokenParseContext()
  : _ast(std::make_shared<ast::IToken>(ast::Root()))
  , _stack({ _ast.begin() })
{
}

}
