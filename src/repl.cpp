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
#include <filesystem>
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
  char* home = std::getenv("HOME");
  if (home != nullptr) {
    _home = home;
  } else {
    _home = "/";
    std::println(
      std::cerr,
      "warning: cannot find user home directory, `~` will refer to `/`");
  }
}

void
Repl::run()
{
  while (true) {
    std::print("$ ");

    auto res = _parser.parse(std::cin);
    if (!res.has_value()) {
      res.error().log();
      continue;
    }

    auto query = res.value();

    // null -> next line
    if (query.ast().empty()) {
      continue;
    }

    // builtin -> execute builtin
    auto prefix = query.prefix();
    if (has_builtin(prefix) && prefix != "exec") {
      _run_builtin(query, prefix);
      continue;
    }

    // not builtin -> exec builtin
    if (prefix != "exec") {
      query.prefix("exec");
    }

    if (auto res = run_builtin(query, "exec"); !res.has_value()) {
      res.error().log();
    }
  }
}

error::Result<void>
Repl::run_builtin(parser::ParseQuery& query, const std::string& name)
{

  if (has_builtin(name)) {
    return _builtins.at(name)->execute(*this, query);
  }

  return error::err(error::ErrorCode::BUILTIN_NOT_FOUND);
}

error::Result<void>
Repl::run_proc(
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
      std::exit(EXIT_SUCCESS);
    }
  } else if (pid > 0) {
    waitpid(pid, nullptr, 0);
    return {};
  } else {
    return error::err(error::ErrorCode::BUILTIN_FORK_FAILED);
  }
}

std::optional<std::string>
Repl::find_command(std::string_view command)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;
  namespace fs = std::filesystem;

  if (fs::exists(command)) {
    return std::string(command);
  }

  auto* envpath_cstr = std::getenv("PATH");
  auto envpath =
    envpath_cstr == nullptr ? std::string() : std::string(envpath_cstr);

  auto path_list = envpath | views::split(':') |
                   views::transform([](const auto& item) {
                     return std::string(item.begin(), item.end());
                   }) |
                   ranges::to<std::vector<std::string>>();

  for (auto& path : path_list) {
    auto full_path = fs::path(path) / command;

    if (fs::exists(full_path)) {
      return full_path;
    }
  }

  return std::nullopt;
}

void
Repl::_run_builtin(parser::ParseQuery& query, const std::string& name)
{
  // no such builtin -> exit
  if (!has_builtin(name)) {
    return;
  }

  // get redirects
  bool err = false;

  for (auto& redirect : query.redirects()) {
    if (auto res = redirect->apply(); !res.has_value()) {
      res.error().log();

      err = true;
      break;
    }
  }

  if (!err) {
    if (auto res = _builtins.at(name)->execute(*this, query);
        !res.has_value()) {
      res.error().log();
    }
  }

  // restore redirects
  for (auto& redirect : query.redirects()) {
    if (auto res = redirect->restore(); !res.has_value()) {
      res.error().log();
    }
  }
}

}
