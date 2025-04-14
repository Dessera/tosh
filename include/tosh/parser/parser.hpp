#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

namespace tosh::repl {

class Repl;

}

namespace tosh::parser {

class TokenParser
{
public:
  error::Result<ParseQuery> parse(repl::Repl& repl);
};

}
