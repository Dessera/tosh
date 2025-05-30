#pragma once

#include "tosh/common.hpp"
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

  /**
   * @brief Execute the builtin command
   *
   * @param repl the repl instance
   * @param query the query to execute
   * @return error::Result<int> execution result
   */
  virtual error::Result<int> execute(repl::Repl& repl,
                                     parser::ParseQuery& query) = 0;
};

}
