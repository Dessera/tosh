#include "desh/repl.hpp"
#include "desh/builtins/base.hpp"
#include "desh/builtins/check_args.hpp"
#include "desh/builtins/echo.hpp"
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

namespace {

std::vector<char*>
token_to_args(const std::vector<std::string>& tokens)
{
  std::vector<char*> args{};
  args.reserve(tokens.size() + 1);

  for (const auto& token : tokens) {
    args.push_back(const_cast<char*>(token.c_str()));
  }

  args.push_back(nullptr);

  return args;
}

}

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
        { "pwd",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Pwd()) } })
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

    if (token_buffer.empty()) {
      continue;
    }

    if (auto it = _builtins.find(token_buffer[0]); it != _builtins.end()) {
      it->second->execute((*this), token_buffer);
      continue;
    }

    if (auto cmd = parser::detect_command(token_buffer[0]); cmd.has_value()) {
      if (auto pid = fork(); pid == 0) {
        auto token_buffer_cstr = token_to_args(token_buffer);

        if (execvp(cmd.value().c_str(), token_buffer_cstr.data()) == -1) {
          std::println("exec failed: {}", std::strerror(errno));
          exit(EXIT_FAILURE);
        }
      } else if (pid > 0) {
        wait(nullptr);
      } else {
        std::println(
          "{} fork failed: {}", token_buffer[0], std::strerror(errno));
      }

      continue;
    }

    std::println("{}: not found", token_buffer[0]);
  }
}

bool
Repl::has_builtin(const std::string& name) const
{
  return _builtins.find(name) != _builtins.end();
}

}
