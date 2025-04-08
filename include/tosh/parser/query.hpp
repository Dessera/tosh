#pragma once

#include "tosh/parser/ast/root.hpp"

#include <memory>

namespace tosh::parser {

/**
 * @brief Parser query result
 */
class ParseQuery
{
private:
  std::shared_ptr<ast::RootToken> _root{ nullptr };

public:
  ParseQuery() = default;
  ParseQuery(std::shared_ptr<ast::RootToken> root);

  [[nodiscard]] const ast::RootToken& ast() const { return *_root; }
};

}
