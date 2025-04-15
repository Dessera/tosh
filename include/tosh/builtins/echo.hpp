#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class TOSH_EXPORT Echo : public BaseCommand
{
public:
  error::Result<int> execute(repl::Repl& repl,
                             parser::ParseQuery& query) override;
};

}
