#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/parser/query.hpp"

#include <map>
#include <memory>
#include <string>

namespace tosh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

  parser::TokenParser _parser{};
  parser::ParseQuery _query;

public:
  Repl();

  [[noreturn]] void run();

  [[nodiscard]] bool has_builtin(const std::string& name) const;
  int execute_builtin(const std::string& name,
                      std::span<const std::string> args);

  constexpr parser::TokenParser& get_parser() { return _parser; }
  constexpr parser::ParseQuery& get_query() { return _query; }
};

}