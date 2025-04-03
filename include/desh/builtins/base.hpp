#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace desh::builtins {

using CommandArgType = std::variant<std::string, int, double, bool>;

class BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  virtual int execute(const std::vector<std::string_view>& args) = 0;
};

}