
#include "tosh/builtins/type.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <format>
#include <print>

namespace tosh::builtins {

error::Result<void>
Type::execute(repl::Repl& repl, parser::ParseQuery& query)
{
  auto args = query.args();

  if (args.size() == 1) {
    return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                      std::format("usage: {} <symbol>", args[0]));
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

  return {};
}

}
