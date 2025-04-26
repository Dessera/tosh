#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class Cd : public BaseCommand
{
public:
  error::Result<int> execute(repl::Repl& repl,
                             parser::ParseQuery& query) override;
};

}
