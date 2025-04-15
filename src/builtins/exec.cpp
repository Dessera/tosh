
#include "tosh/builtins/exec.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <cstring>
#include <print>
#include <ranges>
#include <sys/wait.h>
#include <unistd.h>

namespace tosh::builtins {

error::Result<int>
Exec::execute(repl::Repl& repl, parser::ParseQuery& query)
{
  return repl.run_proc(query, [&repl](auto& query) -> error::Result<int> {
    namespace views = std::ranges::views;
    namespace ranges = std::ranges;

    auto args = query.args();

    if (args.size() < 2) {
      std::println("usage: {} <command> [args]", args[0]);
      return EXIT_FAILURE;
    }

    auto exec_args = args | views::drop(1) |
                     views::transform([](const auto& str) {
                       return const_cast<char*>(str.c_str());
                     }) |
                     ranges::to<std::vector<char*>>();
    exec_args.push_back(nullptr);

    if (auto cmd = repl.find_command_full(exec_args[0]); !cmd.empty()) {
      if (execvp(cmd[0].c_str(), exec_args.data()) == -1) {
        return error::err(error::ErrorCode::BUILTIN_EXEC_FAILED);
      }
    } else {
      return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                        "{} command not found",
                        exec_args[0]);
    }

    return EXIT_SUCCESS;
  });
}

}
