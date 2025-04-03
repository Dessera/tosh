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
  std::string buffer{};
  std::vector<std::string> token_buffer{};

  while (true) {
    token_buffer.clear();

    std::print("$ ");
    std::getline(std::cin, buffer);

    parser::parse_args(token_buffer, buffer);

    // null -> next line
    if (token_buffer.empty()) {
      continue;
    }

    // builtin -> execute builtin
    if (execute_builtin(token_buffer[0], token_buffer) == EXIT_SUCCESS) {
      continue;
    }

    // not builtin -> exec builtin
    auto exec_args = token_buffer;
    exec_args.insert(exec_args.begin(), "exec");
    execute_builtin("exec", exec_args);
  }
}

bool
Repl::has_builtin(const std::string& name) const
{
  return _builtins.find(name) != _builtins.end();
}

int
Repl::execute_builtin(const std::string& name,
                      const std::vector<std::string>& args)
{
  if (!has_builtin(name)) {
    return EXIT_FAILURE;
  }

  return _builtins.at(name)->execute((*this), args);
}

}
