#include "desh/parser.hpp"
#include "desh/parser/token/root.hpp"

#include <filesystem>
#include <istream>
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

namespace desh::parser {

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

void
TokenParser::parse(std::istream& input)
{
  std::string buffer{};
  std::getline(input, buffer);

  _root.clear();

  for (auto& c : buffer) {
    _root.parse_next(c);
  }

  _root.parse_end();
}

RootToken&
TokenParser::root()
{
  return _root;
}

}
