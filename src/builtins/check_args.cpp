
#include "desh/builtins/check_args.hpp"

#include <cstdlib>
#include <print>

namespace desh::builtins {

int
CheckArgs::execute(repl::Repl& /*repl*/, const std::vector<std::string>& args)
{
  std::println("Checking arguments:");
  for (const auto& arg : args) {
    std::println("  {}", arg);
  }

  return EXIT_SUCCESS;
}

}