
#include "desh/builtins/cd.hpp"
#include "desh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>

namespace desh::builtins {

int
Cd::execute(repl::Repl& /*repl*/, const std::vector<std::string>& args)
{
  if (args.size() == 1) {
    std::println("usage: {} <directory>", args[0]);
    return EXIT_FAILURE;
  }

  const auto& path = args[1];
  const auto result = std::filesystem::current_path() / path;

  if (!std::filesystem::exists(result)) {
    std::println("no such file or directory: {}", path);
    return EXIT_FAILURE;
  }

  std::filesystem::current_path(result);
  return EXIT_SUCCESS;
}

}