
#include "tosh/builtins/echo.hpp"

#include <cstdlib>
#include <iostream>
#include <print>

namespace tosh::builtins {

int
Echo::execute(repl::Repl& /*repl*/, std::span<const std::string> args)
{
  if (args.size() == 1) {
    std::println("usage: {} [string ...]", args[0]);
    return EXIT_FAILURE;
  }

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    std::cout << *it;
    if (it + 1 != args.end()) {
      std::cout << " ";
    }
  }

  std::cout << '\n';

  return EXIT_SUCCESS;
}

}