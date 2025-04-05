#pragma once

#include "tosh/builtins/base.hpp"

namespace tosh::builtins {

class Pwd : public BaseCommand
{
public:
  int execute(repl::Repl& repl, std::span<const std::string> args) override;
};

}