
#include "tosh/builtins/type.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <format>
#include <print>

namespace tosh::builtins {

error::Result<int>
Type::execute(repl::Repl& repl, parser::ParseQuery& query)
{
  auto args = query.args();

  if (args.size() == 1) {
    std::println("usage: {} <symbol>", args[0]);
    return EXIT_FAILURE;
  }

  for (auto it = args.begin() + 1; it != args.end(); ++it) {
    const auto& symbol = *it;
    if (repl.has_builtin(symbol)) {
      std::println("{} is a shell builtin", symbol);
    } else if (auto cmd = repl.find_command_full(symbol); !cmd.empty()) {
      std::print("{}:", symbol);
      for (auto& c : cmd) {
        std::print(" {}", c);
      }
      std::println("");
    } else {
      std::println("{} not found", symbol);
    }
  }

  return EXIT_SUCCESS;
}

}
