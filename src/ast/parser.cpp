#include "tosh/ast/parser.hpp"

#include <cstddef>
#include <span>
#include <string_view>
#include <vector>

namespace tosh::ast {

ParseStatus
ParseContext::parse(char c)
{
  return ParseStatus::CONTINUE;
}

}
