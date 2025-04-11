
#include "tosh/builtins/cd.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <ranges>
#include <system_error>

namespace {

namespace fs = std::filesystem;

}

namespace tosh::builtins {

error::Result<void>
Cd::execute(repl::Repl& repl, parser::ParseQuery& query)
{
  namespace views = std::ranges::views;

  auto args = query.args();

  if (args.size() == 1) {
    args.emplace_back("~");
  }

  fs::path path{ args[1] };
  fs::path path_to{};

  if ((*path.begin() == "~")) {
    path_to = repl.home();

    for (const auto& p : path | views::drop(1)) {
      path_to /= p;
    }
  } else {
    path_to = fs::current_path() / path;
  }

  if (!fs::exists(path_to) || !fs::is_directory(path_to)) {
    return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                      std::format("{} is not a directory", path_to.string()));
  }

  std::error_code ec;
  fs::current_path(path_to, ec);

  if (ec) {
    return error::err(error::ErrorCode::UNKNOWN, ec.message());
  }

  return {};
}

}
