#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

#include <istream>

namespace tosh::parser {

class TokenParser
{
public:
  error::Result<ParseQuery> parse(std::istream& input);
};

}
