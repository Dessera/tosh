#pragma once

#include <optional>
#include <string_view>
#include <vector>

namespace desh::parser {

void
parse_args(std::vector<std::string>& buffer, std::string_view raw);

std::optional<std::string>
detect_command(std::string_view command);

}