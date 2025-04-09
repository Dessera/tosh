
#include "tosh/parser/query.hpp"
#include "tosh/parser/ast/root.hpp"

#include <memory>
#include <utility>

namespace tosh::parser {

ParseQuery::ParseQuery(std::shared_ptr<ast::Root> root)
  : _root(std::move(root))
{
}

}
