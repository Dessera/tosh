
#include "tosh/builtins/check_args.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <print>

namespace tosh::builtins {

int
CheckArgs::execute(repl::Repl& repl, std::span<const std::string> args)
{
  std::println("Args:");
  for (const auto& arg : args) {
    std::println("  {}", arg);
  }

  std::println("\nAst:\n{}", repl.get_query().ast());

  std::println("\nRedirects:");
  for (const auto& redirect : repl.get_query().redirects()) {
    std::println("  {}", redirect);
  }

  return EXIT_SUCCESS;
}

}