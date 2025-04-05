
#include "tosh/parser/query.hpp"
#include "tosh/parser/token/root.hpp"

#include <utility>

namespace tosh::parser {

ParseQuery::ParseQuery(std::unique_ptr<RootToken> root)
  : _root(std::move(root))
{
}

}
