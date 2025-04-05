#include "desh/parser/token/base.hpp"

namespace desh::parser {

class BackslashToken : public BaseToken
{
private:
  std::string _str;
  char _quote;

public:
  BackslashToken(char quote = '\0');

  TokenState parse_next(char c) override;
  [[nodiscard]] std::string to_string() const override;
};

}
