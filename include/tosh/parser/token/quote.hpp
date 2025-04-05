#pragma once

#include "tosh/parser/token/base.hpp"

namespace tosh::parser {

class QuoteToken : public TreeToken
{
private:
  char _quote;

public:
  QuoteToken(char quote);

private:
  TokenState create_new_token(char c) override;
};

}
