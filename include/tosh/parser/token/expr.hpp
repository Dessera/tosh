#pragma once

#include "tosh/parser/token/base.hpp"

namespace tosh::parser {

class ExprToken : public TreeToken
{
public:
  ExprToken(char init);

private:
  TokenState create_new_token(char c) override;
};

}
