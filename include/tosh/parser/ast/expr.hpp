#pragma once

#include "tosh/parser/ast/base.hpp"
#include <cstddef>

namespace tosh::ast {

class ExprToken : public BaseToken
{
public:
  ExprToken(size_t level = 0);

private:
  ParseState on_continue(char c) override;
  ParseState on_invalid(char c) override;
  ParseState on_end() override;
};

}
