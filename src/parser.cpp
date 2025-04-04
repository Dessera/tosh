#include "desh/parser.hpp"

#include <filesystem>
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

TokenBuffer
TokenBuffer::parse(std::string_view raw)
{
  // TokenBuffer buffer;
  std::vector<std::string> tokens{};
  std::string token_buffer{};

  char stop_quote = '\0';
  bool in_quotes = false;

  tokens.emplace_back(""); // placeholder for the builtin

  std::string_view::iterator start = raw.begin();
  std::string_view::iterator it = raw.begin();
  // NOLINTNEXTLINE
  while (it != raw.end()) {
    if (*it == ' ') {
      if (!in_quotes) {
        if (start != it) {
          // tokens.emplace_back(start, it);
          tokens.push_back(token_buffer);
          token_buffer.clear();
        }
        // NOLINTNEXTLINE
        start = it + 1;
      } else {
        token_buffer += *it;
      }
    } else if (*it == '"' || *it == '\'') {
      if (in_quotes && stop_quote == *it) {
        in_quotes = false;
      } else {
        in_quotes = true;
        stop_quote = *it;
      }
    } else if (*it == '\\') {
      if (it + 1 != raw.end()) {
        if (in_quotes) {
          if (*(it + 1) == '\\' || *(it + 1) == stop_quote) {
            token_buffer += *(it + 1);
          } else {
            token_buffer += *it;
            token_buffer += *(it + 1);
          }
        } else {
          token_buffer += *(it + 1);
        }
        ++it;
      }
    } else {
      token_buffer += *it;
    }

    ++it;
  }

  if (start != raw.end()) {
    tokens.push_back(token_buffer);
  }

  return TokenBuffer{ std::move(tokens) };
}

TokenBuffer::TokenBuffer(std::vector<std::string> tokens)
  : _tokens{ std::move(tokens) }
{
}

std::span<const std::string>
TokenBuffer::args() const
{
  return std::span(_tokens).subspan(1);
}

bool
TokenBuffer::is_empty() const
{
  return _tokens.size() == 1;
}

std::span<const std::string>
TokenBuffer::args_with_prefix(const std::string& prefix)
{
  // nobody can touch this except through this function
  _tokens.front() = prefix;
  return _tokens;
}

}