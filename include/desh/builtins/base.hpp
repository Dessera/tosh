#pragma once

#include <string>
#include <vector>

namespace desh::repl {

class Repl;

}

namespace desh::builtins {

class BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  virtual int execute(repl::Repl& repl,
                      const std::vector<std::string>& args) = 0;
};

}