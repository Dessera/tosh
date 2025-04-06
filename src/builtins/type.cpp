
#include "tosh/builtins/type.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <print>

namespace tosh::builtins {

int
Type::execute(repl::Repl& repl, std::span<const std::string> args)
{
  if (args.size() == 1) {
    std::println("usage: {} <symbol>", args[0]);
    return EXIT_FAILURE;
  }

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    const auto& symbol = *it;
    if (repl.has_builtin(symbol)) {
      std::println("{} is a shell builtin", symbol);
    } else if (auto cmd = parser::detect_command(symbol); cmd.has_value()) {
      std::println("{} is {}", symbol, cmd.value());
    } else {
      std::println("{} not found", symbol);
    }
  }

  return EXIT_SUCCESS;
}

}