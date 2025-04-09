#pragma once

#include "tosh/ast/base.hpp"

#include <expected>
#include <istream>

namespace tosh::ast {

class ParseContext
{
private:
  IToken::WeakPtr _curr;
  IToken::Ptr _ast;

public:
  ParseContext();

  /**
   * @brief Parses the stream.
   *
   * @return std::expected<void, ParseException> The result of the parse.
   */
  ParseStatus parse(char c);

  [[nodiscard]] constexpr auto curr() const { return _curr.lock(); }
  [[nodiscard]] constexpr auto ast() const { return _ast; }
  [[nodiscard]] constexpr bool eop() const { return _curr.expired(); };
};

}