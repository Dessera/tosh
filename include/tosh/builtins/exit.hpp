#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class TOSH_EXPORT Exit : public BaseCommand
{
public:
  error::Result<void> execute(repl::Repl& repl,
                              parser::ParseQuery& query) override;
};

}
