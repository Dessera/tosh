#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/utils/redirect.hpp"

namespace tosh::repl {

class Repl;

}

namespace tosh::parser {

error::Result<ParseQuery>
parse(repl::Repl& repl);

}
