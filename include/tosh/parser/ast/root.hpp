#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class Root : public BaseToken
{
public:
  Root();

private:
  ParseState on_continue(char c) override;
};

}
