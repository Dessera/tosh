
#include "tosh/builtins/pwd.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <unistd.h>

namespace tosh::builtins {

error::Result<void>
Pwd::execute(repl::Repl& /*repl*/, parser::ParseQuery& /*query*/)
{
  std::println("{}", std::filesystem::current_path().string());
  return {};
}
}
