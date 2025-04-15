
#include "tosh/builtins/check_args.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <print>

namespace tosh::builtins {

error::Result<int>
CheckArgs::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

  std::println("Args:");
  for (const auto& arg : args) {
    std::println("  {}", arg);
  }

  std::println("\nAst:\n{}", query.ast());

  std::println("\nRedirects:");
  for (const auto& redirect : query.redirects()) {
    std::println("  {}", redirect->data());
  }

  return EXIT_SUCCESS;
}

}
