
#include "desh/builtins/exit.hpp"

#include <cstdlib>

namespace desh::builtins {

int
Exit::execute(repl::Repl& /*repl*/, const std::vector<std::string>& args)
{
  int status = EXIT_SUCCESS;
  if (args.size() > 1) {
    try {
      status = std::stoi(args[1]);
    } catch (...) {
      status = EXIT_FAILURE;
    }
  }

  std::exit(status);
}

}