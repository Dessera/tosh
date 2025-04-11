#include "tosh/repl.hpp"
#include "tosh/builtins/base.hpp"
#include "tosh/builtins/cd.hpp"
#include "tosh/builtins/check_args.hpp"
#include "tosh/builtins/echo.hpp"
#include "tosh/builtins/exec.hpp"
#include "tosh/builtins/exit.hpp"
#include "tosh/builtins/pwd.hpp"
#include "tosh/builtins/type.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <print>
#include <sys/wait.h>
#include <unistd.h>

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
  while (true) {
    std::print("$ ");
    auto query = _parser.parse(std::cin);

    // null -> next line
    if (query.ast().empty()) {
      continue;
    }

    // builtin -> execute builtin
    auto args = query.args();
    if (has_builtin(args[0])) {
      if (auto res = _builtins.at(args[0])->execute(*this, query);
          !res.has_value()) {
      }
      continue;
    }

    // not builtin -> exec builtin
    // args.insert(args.begin(), "exec");
    // execute_builtin("exec", args);
  }
}

error::Result<void>
Repl::execute(
  parser::ParseQuery& query,
  const std::function<error::Result<void>(parser::ParseQuery&)>& callback)
{
  if (auto pid = fork(); pid == 0) {
    for (auto& redirect : query.redirects()) {
      if (auto res = redirect->apply(); !res.has_value()) {
        res.error().log();
        std::exit(EXIT_FAILURE);
      }
    }
    if (auto res = callback(query); !res.has_value()) {
      res.error().log();
      std::exit(EXIT_FAILURE);
    } else {
      std::exit(0);
    }
  } else if (pid > 0) {
    waitpid(pid, nullptr, 0);
    return {};
  } else {
    return error::err(error::ErrorCode::BUILTIN_FORK_FAILED);
  }
}

}
