#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class TOSH_EXPORT Cd : public BaseCommand
{
public:
  error::Result<void> execute(repl::Repl& repl,
                              parser::ParseQuery& query) override;
};

}
