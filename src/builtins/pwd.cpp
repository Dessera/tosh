
#include "desh/builtins/pwd.hpp"
#include "desh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <unistd.h>

namespace desh::builtins {

int
Pwd::execute(repl::Repl& /*repl*/, const std::vector<std::string>& /*args*/)
{
  std::println("{}", std::filesystem::current_path().string());

  return EXIT_SUCCESS;
}

}