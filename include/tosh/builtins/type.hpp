#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class Type : public BaseCommand
{
public:
  error::Result<void> execute(repl::Repl& repl,
                              parser::ParseQuery& query) override;
};

}
