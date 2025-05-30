
#include "tosh/parser/query.hpp"
#include "tosh/parser/ast/expr.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/utils/redirect.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tosh::parser {

ParseQuery::ParseQuery(
  ast::Root::Ptr root,
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

std::string
ParseQuery::prefix() const
{
  return ast().nodes().front()->string();
}

void
ParseQuery::prefix(std::string_view prefix)
{
  ast().nodes().push_front(std::make_shared<ast::Text>(std::string(prefix)));
}

}
