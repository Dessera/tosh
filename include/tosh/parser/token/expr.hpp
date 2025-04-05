#pragma once

#include "tosh/parser/token/base.hpp"
#include <cstddef>

namespace tosh::parser {

class ExprToken : public TreeToken
{
public:
  ExprToken(char init, size_t level = 0);

private:
  TokenState create_new_token(char c) override;
};

}
