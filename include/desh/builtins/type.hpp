#pragma once

#include "desh/builtins/base.hpp"

namespace desh::builtins {

class Type : public BaseCommand
{
public:
  int execute(repl::Repl& repl, const std::vector<std::string>& args) override;
};

}