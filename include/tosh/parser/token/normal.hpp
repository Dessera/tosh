#pragma once

#include "tosh/parser/token/base.hpp"

namespace tosh::parser {

class NormalToken : public BaseToken
{
private:
  std::string _str;

public:
  NormalToken(char init);

  TokenState parse_next(char c) override;
  [[nodiscard]] std::string to_string() const override;
};

}
