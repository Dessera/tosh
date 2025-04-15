
#include "tosh/builtins/pwd.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <unistd.h>

namespace tosh::builtins {

error::Result<int>
Pwd::execute(repl::Repl& /*repl*/, parser::ParseQuery& /*query*/)
{
  namespace fs = std::filesystem;

  std::println("{}", fs::current_path().string());
  return EXIT_SUCCESS;
}
}
