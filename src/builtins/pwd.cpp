
#include "tosh/builtins/pwd.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <unistd.h>

namespace tosh::builtins {

int
Pwd::execute(repl::Repl& /*repl*/, std::span<const std::string> /*args*/)
{
  std::println("{}", std::filesystem::current_path().string());

  return EXIT_SUCCESS;
}

}