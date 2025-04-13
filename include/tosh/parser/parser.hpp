#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/query.hpp"

#include <cstdint>
#include <sys/types.h>

namespace tosh::parser {

enum class InputCommand : uint8_t
{
  NEXTLINE = 10,
  ESCAPE = 27,
  LWIN = 91,
  BACKSPACE = 127,

};

class TokenParser
{
public:
  error::Result<ParseQuery> parse();

private:
  void enable_raw_stdin();
  void disable_raw_stdin();

  std::string read_line();
};

}
