#pragma once

#include "desh/builtins/base.hpp"

namespace desh::builtins {

class Echo : public BaseCommand
{
public:
  int execute(repl::Repl& repl, std::span<const std::string> args) override;
};

}