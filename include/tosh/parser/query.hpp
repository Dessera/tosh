#pragma once

#include "tosh/parser/ast/root.hpp"
#include "tosh/utils/redirect.hpp"

#include <memory>
#include <vector>

namespace tosh::parser {

/**
 * @brief Parser query result
 */
class ParseQuery
{
private:
  std::shared_ptr<ast::Root> _root{ nullptr };
  std::vector<std::shared_ptr<utils::RedirectOperation>> _redirects;

public:
  ParseQuery() = default;
  ParseQuery(std::shared_ptr<ast::Root> root,
             std::vector<std::shared_ptr<utils::RedirectOperation>> redirects);

  [[nodiscard]] constexpr const auto& ast() const { return *_root; }
  [[nodiscard]] constexpr auto& ast() { return *_root; }
  [[nodiscard]] constexpr const auto& redirects() const { return _redirects; }
  [[nodiscard]] constexpr auto& redirects() { return _redirects; }
};

}
