#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

namespace tosh::repl {

class Repl;

}

namespace tosh::builtins {

class TOSH_EXPORT BaseCommand
{
public:
  virtual ~BaseCommand() = default;

  /**
   * @brief Execute the builtin command
   *
   * @param repl the repl instance
   * @param query the query to execute
   * @return error::Result<void> execution result
   */
  virtual error::Result<void> execute(repl::Repl& repl,
                                      parser::ParseQuery& query) = 0;
};

}
