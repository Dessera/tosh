#pragma once

#include "tosh/parser/token/root.hpp"

#include <memory>

namespace tosh::parser {

/**
 * @brief Parser query result
 */
class ParseQuery
{
private:
  std::unique_ptr<RootToken> _root;

public:
  ParseQuery(std::unique_ptr<RootToken> root);

  RootToken& root() { return *_root; }
};

}
