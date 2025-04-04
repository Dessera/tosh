#pragma once

#include "desh/builtins/base.hpp"
#include "desh/parser.hpp"

#include <map>
#include <memory>
#include <string>

namespace desh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

  std::string _buffer;
  parser::TokenBuffer _parser;

public:
  Repl();

  [[noreturn]] void run();

  [[nodiscard]] bool has_builtin(const std::string& name) const;
  int execute_builtin(const std::string& name,
                      std::span<const std::string> args);
};

}