#include "tosh/repl.hpp"
#include "tosh/builtins/base.hpp"
#include "tosh/builtins/cd.hpp"
#include "tosh/builtins/check_args.hpp"
#include "tosh/builtins/echo.hpp"
#include "tosh/builtins/exec.hpp"
#include "tosh/builtins/exit.hpp"
#include "tosh/builtins/pwd.hpp"
#include "tosh/builtins/type.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <print>
#include <ranges>
#include <string>

namespace tosh::repl {

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
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  while (true) {
    std::print("$ ");
    std::cin >> std::ws;
  }
  //   _query = _parser.parse(std::cin);

  //   // null -> next line
  //   if (_query.ast().empty()) {
  //     continue;
  //   }

  //   // builtin -> execute builtin
  //   auto args = _query.ast().nodes() |
  //               views::transform([](auto& token) { return token->string(); })
  //               | ranges::to<std::vector<std::string>>();
  //   if (has_builtin(args[0])) {
  //     execute_builtin(args[0], args);
  //     continue;
  //   }

  //   // not builtin -> exec builtin
  //   args.insert(args.begin(), "exec");
  //   execute_builtin("exec", args);
  // }
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
