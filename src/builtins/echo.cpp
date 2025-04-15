
#include "tosh/builtins/echo.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <iostream>
#include <print>

namespace tosh::builtins {

error::Result<int>
Echo::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

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
