#pragma once

#include "tosh/parser/query.hpp"

#include <istream>
#include <optional>
#include <string_view>

namespace tosh::parser {

std::optional<std::string>
detect_command(std::string_view command);

class TokenParser
{
public:
  ParseQuery parse(std::istream& input);
};

}
