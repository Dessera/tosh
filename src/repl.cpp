#include "desh/repl.hpp"
#include "desh/builtins/base.hpp"
#include "desh/builtins/exit.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <print>
#include <string>

namespace desh::repl {

Repl::Repl()
  : _builtins(
      { { "exit",
          std::shared_ptr<builtins::BaseCommand>(new builtins::Exit()) } })
{
}

int
Repl::run()
{
  std::string buffer{};

  while (true) {
    std::print("$ ");
    std::getline(std::cin, buffer);

    if (_builtins.contains(buffer)) {
      // _builtins[buffer]->execute("");
    } else {
      std::println("{}: invalid command", buffer);
    }
  }

  return EXIT_SUCCESS;
}

void
Repl::_split_command(std::vector<std::string_view>& buffer,
                     std::string_view command)
{
}

}