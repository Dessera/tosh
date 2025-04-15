#pragma once

#include "tosh/common.hpp"
#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class TOSH_EXPORT Root : public Token
{
public:
  Root();

private:
  ParseState on_continue(char c) override;
};

}
