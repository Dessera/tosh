#pragma once

#include "desh/builtins/base.hpp"

namespace desh::builtins {

class Exit : public BaseCommand
{
public:
  int execute(const std::vector<std::string_view>& args) override;
};

}