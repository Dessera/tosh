#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class RootToken : public BaseToken
{
public:
  RootToken();

private:
  ParseState on_continue(char c) override;
};

}
