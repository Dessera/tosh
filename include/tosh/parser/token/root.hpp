#pragma once

#include "tosh/parser/token/base.hpp"

namespace tosh::parser {

class RootToken : public TreeToken
{
public:
  RootToken();

private:
  TokenState create_new_token(char c) override;
};

}
