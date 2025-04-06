#pragma once

#include "tosh/parser/ast/base.hpp"
#include <cstddef>

namespace tosh::ast {

class ExprToken : public BaseToken
{
public:
  ExprToken(size_t level = 0);

private:
  ParseState handle_char(char c) override;
  // ParseState handle_invalid() override;
};

}
