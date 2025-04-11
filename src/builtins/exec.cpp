
#include "tosh/builtins/exec.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <cstring>
#include <format>
#include <print>
#include <ranges>
#include <sys/wait.h>
#include <unistd.h>

namespace {

}

namespace tosh::builtins {

error::Result<void>
Exec::execute(repl::Repl& repl, parser::ParseQuery& query)
{
  return repl.run_proc(query, [](auto& query) -> error::Result<void> {
    namespace views = std::ranges::views;
    auto args = query.args();

    if (args.size() <= 1) {
      return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                        std::format("usage: {} <command>", args[0]));
    }

    auto exec_args = args | views::drop(1) |
                     views::transform([](const auto& str) {
                       return const_cast<char*>(str.c_str());
                     }) |
                     std::ranges::to<std::vector<char*>>();
    exec_args.push_back(nullptr);

    if (auto cmd = parser::detect_command(exec_args[0]); cmd.has_value()) {
      if (execvp(cmd.value().c_str(), exec_args.data()) == -1) {
        return error::err(error::ErrorCode::BUILTIN_EXEC_FAILED);
      }
    } else {
      return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                        std::format("{} command not found", exec_args[0]));
    }

    return {};
  });
}

}
