#include "desh/repl.hpp"
#include "desh/builtins/base.hpp"
#include "desh/builtins/cd.hpp"
#include "desh/builtins/check_args.hpp"
#include "desh/builtins/echo.hpp"
#include "desh/builtins/exec.hpp"
#include "desh/builtins/exit.hpp"
#include "desh/builtins/pwd.hpp"
#include "desh/builtins/type.hpp"
#include "desh/parser.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <print>
#include <string>
#include <sys/wait.h>

namespace desh::repl {

Repl::Repl()
  : _builtins(
      { { "exit",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Exit()) },
        { "check-args",
          std::shared_ptr<builtins::BaseCommand>(new builtins::CheckArgs()) },
        { "echo",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Echo()) },
        { "type",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Type()) },
        { "pwd", std::shared_ptr<builtins::BaseCommand>(new builtins::Pwd()) },
        { "cd", std::shared_ptr<builtins::BaseCommand>(new builtins::Cd()) },
        { "exec",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Exec()) } })
{
}

void
Repl::run()
{
  while (true) {
    std::print("$ ");
    std::getline(std::cin, _buffer);

    _parser = parser::TokenBuffer::parse(_buffer);

    // null -> next line
    if (_parser.is_empty()) {
      continue;
    }

    // builtin -> execute builtin
    auto args = _parser.args();
    if (has_builtin(args[0])) {
      execute_builtin(args[0], args);
      continue;
    }

    // not builtin -> exec builtin
    execute_builtin("exec", _parser.args_with_prefix("exec"));
  }
}

bool
Repl::has_builtin(const std::string& name) const
{
  return _builtins.find(name) != _builtins.end();
}

int
Repl::execute_builtin(const std::string& name,
                      std::span<const std::string> args)
{
  if (!has_builtin(name)) {
    return EXIT_FAILURE;
  }

  return _builtins.at(name)->execute((*this), args);
}

}
