#pragma once

#include "desh/builtins/base.hpp"
#include <map>
#include <memory>
#include <string>
namespace desh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

public:
  Repl();

  [[noreturn]] void run();

  [[nodiscard]] bool has_builtin(const std::string& name) const;
};

}