#pragma once

#include "desh/parser/token/base.hpp"

namespace desh::parser {

class RootToken : public TreeToken
{
public:
  RootToken();

private:
  TokenState create_new_token(char c) override;
};

}
