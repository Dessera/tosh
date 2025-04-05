#pragma once

#include <span>
#include <string>

namespace tosh::repl {

class Repl;

}

namespace tosh::builtins {

class BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  virtual int execute(repl::Repl& repl, std::span<const std::string> args) = 0;
};

}