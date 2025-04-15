
#include "tosh/builtins/env.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <format>
#include <iostream>
#include <print>

namespace tosh::builtins {

error::Result<int>
Sete::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

  if (args.size() < 3) {
    std::println(std::cerr, "usage: {} <name> <value>", args[0]);
    return EXIT_FAILURE;
  }

  if (setenv(args[1].c_str(), args[2].c_str(), 1) != 0) {
    // return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,)
  }

  return EXIT_SUCCESS;
}

}
