#include "tosh/parser/parser.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"

#include <filesystem>
#include <istream>
#include <memory>
#include <optional>
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

ParseQuery
TokenParser::parse(std::istream& input)
{
  auto root = std::make_shared<ast::RootToken>();

  std::string buffer{};
  std::getline(input, buffer);

  for (auto& c : buffer) {
    root->iter_next(c);
  }
  root->iter_next('\0');

  return { root };
}

}
