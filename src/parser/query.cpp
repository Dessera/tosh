
#include "tosh/parser/query.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/utils/redirect.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace tosh::parser {

ParseQuery::ParseQuery(
  std::shared_ptr<ast::Root> root,
  std::vector<std::shared_ptr<utils::RedirectOperation>> redirects)
  : _root(std::move(root))
  , _redirects(std::move(redirects))
{
}

std::vector<std::string>
ParseQuery::args() const
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  return ast().nodes() |
         views::transform([](const auto& node) { return node->string(); }) |
         ranges::to<std::vector<std::string>>();
}

}
