#pragma once

#include "tosh/parser/ast/base.hpp"
#include <cstddef>

namespace tosh::ast {

class TextToken : public BaseToken
{
private:
  std::string _str;
  char _quote;

public:
  TextToken(char quote = '\0', size_t level = 0);

  ParseState handle_char(char c) override;
  [[nodiscard]] std::string to_string() const override;
};

}
