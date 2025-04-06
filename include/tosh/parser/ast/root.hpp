#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class RootToken : public BaseToken
{
public:
  RootToken();

private:
  ParseState handle_char(char c) override;
};

}
