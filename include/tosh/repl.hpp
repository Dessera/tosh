#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/parser.hpp"

#include <map>
#include <memory>
#include <string>

namespace tosh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

  std::string _buffer;
  parser::TokenParser _parser{};

public:
  Repl();

  [[noreturn]] void run();

  [[nodiscard]] bool has_builtin(const std::string& name) const;
  int execute_builtin(const std::string& name,
                      std::span<const std::string> args);
};

}