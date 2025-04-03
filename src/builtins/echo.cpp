
#include "desh/builtins/echo.hpp"

#include <cstdlib>
#include <iostream>
#include <print>

namespace desh::builtins {

int
Echo::execute(repl::Repl& /*repl*/, const std::vector<std::string>& args)
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