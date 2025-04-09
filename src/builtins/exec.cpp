
#include "tosh/builtins/exec.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <print>
#include <ranges>
#include <sys/wait.h>
#include <unistd.h>

namespace {

char*
to_cstr(const std::string& str)
{
  // NOLINTNEXTLINE
  return const_cast<char*>(str.c_str());
}

}

namespace tosh::builtins {

int
Exec::execute(repl::Repl& /*repl*/, std::span<const std::string> args)
{
  // namespace views = std::ranges::views;

  // if (args.size() == 1) {
  //   std::println("usage: {} <command>", args[0]);
  //   return EXIT_FAILURE;
  // }

  // auto exec_args = args | views::drop(1) | views::transform(to_cstr) |
  //                  std::ranges::to<std::vector<char*>>();
  // exec_args.push_back(nullptr);

  // if (auto cmd = parser::detect_command(exec_args[0]); cmd.has_value()) {
  //   if (auto pid = fork(); pid == 0) {
  //     if (execvp(cmd.value().c_str(), exec_args.data()) == -1) {
  //       std::println(
  //         std::cerr, "failed to execute command: {}", std::strerror(errno));
  //       exit(EXIT_FAILURE);
  //     }
  //   } else if (pid > 0) {
  //     waitpid(pid, nullptr, 0);
  //   } else {
  //     std::println(std::cerr, "failed to fork: {}", std::strerror(errno));
  //   }
  // } else {
  //   std::println(std::cerr, "{}: command not found", exec_args[0]);
  //   return EXIT_FAILURE;
  // }

  return EXIT_SUCCESS;
}

}