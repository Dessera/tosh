#include "tosh/repl.hpp"
#include "tosh/builtins/base.hpp"
#include "tosh/builtins/cd.hpp"
#include "tosh/builtins/check_args.hpp"
#include "tosh/builtins/echo.hpp"
#include "tosh/builtins/env.hpp"
#include "tosh/builtins/exec.hpp"
#include "tosh/builtins/exit.hpp"
#include "tosh/builtins/pwd.hpp"
#include "tosh/builtins/type.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/utils/path.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <print>
#include <ranges>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

namespace {

namespace fs = std::filesystem;

constexpr auto
cmdpath_fixup(const fs::path& cmd)
{
  return [&cmd](const auto& p) {
    auto cmdcopy = cmd;
    cmdcopy.replace_filename(p.filename());
    auto cmdstr = cmdcopy.string();
    if (fs::is_directory(p) && cmdstr.back() != '/') {
      cmdstr.append("/");
    }
    return cmdstr;
  };
}

}

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
          std::shared_ptr<builtins::BaseCommand>(new builtins::Exec()) },
        { "sete",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Sete()) },
        { "unsete",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Unsete()) } })
{
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
}

void
Repl::run()
{
  while (true) {
    std::print("{}", get_prompt());

    auto res = _parser.parse(*this);
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
      run_builtin(query, prefix);
      continue;
    }

    // not builtin -> exec builtin
    if (prefix != "exec") {
      query.prefix("exec");
    }

    if (auto res = run_builtin_no_ops(query, "exec"); !res.has_value()) {
      res.error().log();
    }
  }
}

error::Result<int>
Repl::run_builtin_no_ops(parser::ParseQuery& query, const std::string& name)
{

  if (has_builtin(name)) {
    return _builtins.at(name)->execute(*this, query);
  }

  return error::err(error::ErrorCode::BUILTIN_NOT_FOUND);
}

error::Result<int>
Repl::run_proc(
  parser::ParseQuery& query,
  const std::function<error::Result<int>(parser::ParseQuery&)>& callback)
{
  if (auto pid = fork(); pid == 0) {
    for (auto& redirect : query.redirects()) {
      LOGERR_EXIT(redirect->apply());
    }
    if (auto res = callback(query); !res.has_value()) {
      res.error().log();
      std::exit(EXIT_FAILURE);
    } else {
      std::exit(res.value());
    }
  } else if (pid > 0) {
    _subpid = pid;

    // NOLINTNEXTLINE
    int ret;
    waitpid(pid, &ret, 0);
    ret = WEXITSTATUS(ret);

    _subpid = -1;

    return { ret };
  } else {
    return error::err(error::ErrorCode::BUILTIN_FORK_FAILED);
  }
}

std::vector<std::string>
Repl::find_command_full(std::string_view command)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  fs::path cmd = command;
  fs::path cmdpath = utils::remove_home_prefix(cmd);

  return utils::get_path_env(command) | views::split(':') |
         views::transform([&cmdpath](const auto& ep) {
           return fs::path(std::string_view(ep)) / cmdpath;
         }) |
         views::filter([](const auto& p) { return fs::is_regular_file(p); }) |
         views::transform([](const auto& p) { return p.string(); }) |
         ranges::to<std::vector<std::string>>();
}

std::vector<std::string>
Repl::find_command_fuzzy(std::string_view command)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  fs::path cmd = command;
  fs::path cmdpath = utils::remove_home_prefix(cmd);

  return utils::get_path_env(command) | views::split(':') |
         views::transform([&cmdpath](const auto& ep) {
           return (fs::path(std::string_view(ep)) / cmdpath).parent_path();
         }) |
         views::filter([](const auto& p) { return fs::is_directory(p); }) |
         views::transform(
           [](const auto& p) { return fs::directory_iterator(p); }) |
         views::join |
         views::transform([](const auto& p) { return p.path(); }) |
         views::filter([&cmd](const auto& p) {
           return p.filename().string().starts_with(cmd.filename().string());
         }) |
         views::transform(cmdpath_fixup(cmd)) |
         ranges::to<std::vector<std::string>>();
}

std::vector<std::string>
Repl::find_builtin_fuzzy(std::string_view command)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return _builtins | views::keys | views::filter([&command](const auto& item) {
           return item.starts_with(command);
         }) |
         ranges::to<std::vector<std::string>>();
}

std::vector<std::string>
Repl::find_fuzzy(std::string_view command)
{
  auto commands = find_command_fuzzy(command);
  auto builtins = find_builtin_fuzzy(command);

  builtins.insert(builtins.end(), commands.begin(), commands.end());

  return builtins;
}

void
Repl::sigint_handler()
{
  if (_subpid != -1) {
    kill(_subpid, SIGINT);
    _subpid = -1;
  }
}

void
Repl::run_builtin(parser::ParseQuery& query, const std::string& name)
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

std::string
Repl::get_prompt()
{
  return "$ ";
}

}
