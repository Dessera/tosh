
#include "tosh/builtins/cd.hpp"
#include "tosh/repl.hpp"

#include <cstdlib>
#include <filesystem>
#include <print>
#include <ranges>

namespace {

std::filesystem::path
get_home_directory()
{
  const auto* home = std::getenv("HOME");
  if (home) {
    return home;
  }
  return std::filesystem::current_path();
}

}

namespace tosh::builtins {

int
Cd::execute(repl::Repl& /*repl*/, std::span<const std::string> args)
{
  namespace views = std::ranges::views;

  if (args.size() == 1) {
    std::println("usage: {} <directory>", args[0]);
    return EXIT_FAILURE;
  }

  std::filesystem::path path{ args[1] };
  std::filesystem::path path_to{};

  if ((*path.begin() == "~")) {
    path_to = get_home_directory();

    for (const auto& p : path | views::drop(1)) {
      path_to /= p;
    }
  } else {
    path_to = std::filesystem::current_path() / path;
  }

  if (!std::filesystem::exists(path_to)) {
    std::println("no such file or directory: {}", path.string());
    return EXIT_FAILURE;
  }

  std::filesystem::current_path(path_to);
  return EXIT_SUCCESS;
}

}