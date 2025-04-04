#pragma once

#include <span>
#include <string>

namespace desh::repl {

class Repl;

}

namespace desh::builtins {

class BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  virtual int execute(repl::Repl& repl, std::span<const std::string> args) = 0;
};

}