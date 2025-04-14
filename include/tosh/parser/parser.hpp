#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

namespace tosh::parser {

class TokenParser
{
public:
  error::Result<ParseQuery> parse();
};

}
