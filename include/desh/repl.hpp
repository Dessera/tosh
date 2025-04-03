#pragma once

#include "desh/builtins/base.hpp"
#include <map>
#include <memory>
#include <string>
namespace desh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

public:
  Repl();

  int run();

private:
  void _split_command(std::vector<std::string_view>& buffer,
                      std::string_view command);
};

}