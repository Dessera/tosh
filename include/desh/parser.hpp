#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace desh::parser {

std::optional<std::string>
detect_command(std::string_view command);

class TokenBuffer
{
private:
  std::vector<std::string> _tokens;

public:
  static TokenBuffer parse(std::string_view raw);

  TokenBuffer() = default;
  TokenBuffer(std::vector<std::string> tokens);

  [[nodiscard]] std::span<const std::string> args() const;

  [[nodiscard]] bool is_empty() const;

  [[nodiscard]] std::span<const std::string> args_with_prefix(
    const std::string& prefix);
};

}