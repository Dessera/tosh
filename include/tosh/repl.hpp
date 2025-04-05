#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/parser.hpp"

#include <iostream>
#include <istream>
#include <map>
#include <memory>
#include <string>

namespace tosh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

  parser::TokenParser _parser{};

  std::istream* _pstdin{ &std::cin };
  std::ostream* _pstdout{ &std::cout };
  std::ostream* _pstderr{ &std::cerr };

public:
  Repl();

  [[noreturn]] void run();

  [[nodiscard]] bool has_builtin(const std::string& name) const;
  int execute_builtin(const std::string& name,
                      std::span<const std::string> args);

  std::istream& get_stdin() { return *_pstdin; }
  std::ostream& get_stdout() { return *_pstdout; }
  std::ostream& get_stderr() { return *_pstderr; }

  parser::TokenParser& get_parser() { return _parser; }
};

}