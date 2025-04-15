#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class TOSH_EXPORT Exec : public BaseCommand
{
public:
  error::Result<int> execute(repl::Repl& repl,
                             parser::ParseQuery& query) override;
};

}
