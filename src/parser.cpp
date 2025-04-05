#include "tosh/parser.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/parser/token/root.hpp"

#include <filesystem>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace {

std::vector<std::string>
split_string(std::string_view str, char delimiter)
{
  std::vector<std::string> tokens;
  std::string_view::size_type start = 0;
  std::string_view::size_type end = 0;

  while (end != std::string_view::npos) {
    end = str.find(delimiter, start);
    tokens.emplace_back(str.substr(start, end - start));
    start = end + 1;
  }

  return tokens;
}

}

namespace tosh::parser {

std::optional<std::string>
detect_command(std::string_view command)
{
  auto* envpath = std::getenv("PATH");
  auto path_list = envpath != nullptr ? split_string(envpath, ':')
                                      : std::vector<std::string>();

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
  std::string buffer{};
  std::getline(input, buffer);

  auto root = std::make_unique<RootToken>();

  for (auto& c : buffer) {
    root->parse_next(c);
  }
  root->parse_end();

  return { std::move(root) };
}

}
