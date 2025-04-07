#pragma once

#include "tosh/parser/ast/base.hpp"

#include <cstddef>

namespace tosh::ast {

class BackslashToken : public BaseToken
{
private:
  std::string _str;
  char _quote;

public:
  BackslashToken(char quote = '\0', size_t level = 0);

  ParseState on_continue(char c) override;
  [[nodiscard]] std::string string() const override;
};

}
