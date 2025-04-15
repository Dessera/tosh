
#include "tosh/builtins/cd.hpp"
#include "tosh/error.hpp"
#include "tosh/repl.hpp"
#include "tosh/utils/env.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <system_error>

namespace {

namespace fs = std::filesystem;

}

namespace tosh::builtins {

error::Result<void>
Cd::execute(repl::Repl& /*repl*/, parser::ParseQuery& query)
{
  auto args = query.args();

  if (args.size() == 1) {
    args.emplace_back(utils::getenv("HOME"));
  }

  fs::path path{ args[1] };

  if (!fs::is_directory(path)) {
    return error::err(error::ErrorCode::BUILTIN_INVALID_ARGS,
                      std::format("{} is not a directory", path.string()));
  }

  std::error_code ec;
  fs::current_path(path, ec);

  if (ec) {
    return error::err(error::ErrorCode::UNKNOWN, ec.message());
  }

  return {};
}

}
