
#include "tosh/builtins/check_args.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <print>

namespace tosh::builtins {

int
CheckArgs::execute(repl::Repl& repl, std::span<const std::string> args)
{
  std::println("Input args:");
  for (const auto& arg : args) {
    std::println("  {}", arg);
  }

  std::println("\nInput ast:\n{}", repl.get_query().ast());

  return EXIT_SUCCESS;
}

}