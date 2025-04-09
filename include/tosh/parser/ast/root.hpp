#pragma once

#include "tosh/parser/ast/base.hpp"

namespace tosh::ast {

class Root : public IToken
{
public:
  Root();

  parser::ParseState on_continue(parser::TokenParseContext& ctx,
                                 char c) override;
};

}
