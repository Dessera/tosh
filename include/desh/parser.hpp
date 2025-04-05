#pragma once

#include "desh/parser/token/root.hpp"
#include <istream>
#include <optional>
#include <string_view>

namespace desh::parser {

std::optional<std::string>
detect_command(std::string_view command);

class TokenParser
{
private:
  RootToken _root;

public:
  void parse(std::istream& input);

  [[nodiscard]] RootToken& root();
};

}
