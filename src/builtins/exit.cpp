
#include "tosh/builtins/exit.hpp"

#include <cstdlib>

namespace tosh::builtins {

error::Result<void>
Exit::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

  int status = EXIT_SUCCESS;
  if (args.size() > 1) {
    try {
      status = std::stoi(args[1]);
    } catch (...) {
      status = EXIT_FAILURE;
    }
  }

  std::exit(status);

  return {};
}

}