
#include "tosh/builtins/echo.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <iostream>
#include <print>

namespace tosh::builtins {

error::Result<void>
Echo::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

  if (args.size() == 1) {
    std::println("usage: {} [string ...]", args[0]);
    return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS);
  }

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    std::cout << *it;
    if (it + 1 != args.end()) {
      std::cout << " ";
    }
  }

  std::cout << '\n';

  return {};
}

}