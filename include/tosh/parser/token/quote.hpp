#pragma once

#include "tosh/parser/token/base.hpp"
#include <cstddef>

namespace tosh::parser {

class QuoteToken : public TreeToken
{
private:
  char _quote;

public:
  QuoteToken(char quote, size_t level = 0);

private:
  TokenState create_new_token(char c) override;
};

}
