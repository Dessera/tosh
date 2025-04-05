#pragma once

#include "desh/parser/token/base.hpp"

namespace desh::parser {

class ExprToken : public TreeToken
{
public:
  ExprToken(char init);

private:
  TokenState create_new_token(char c) override;
};

}
