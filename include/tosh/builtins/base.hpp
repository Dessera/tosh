#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

namespace tosh::repl {

class Repl;

}

namespace tosh::builtins {

class BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  virtual error::Result<void> execute(repl::Repl& repl,
                                      parser::ParseQuery& query) = 0;
};

}