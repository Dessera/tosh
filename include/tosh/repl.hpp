#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/parser/query.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace tosh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;

  parser::TokenParser _parser{};

public:
  Repl();

  [[noreturn]] void run();

  /**
   * @brief Run callback in another process
   *
   * @param query Arguments to pass to callback
   * @param callback Callback to run in another process
   * @return error::Result<void> Process result
   */
  error::Result<void> run_proc(
    parser::ParseQuery& query,
    const std::function<error::Result<void>(parser::ParseQuery&)>& callback);

  constexpr auto& parser() { return _parser; }

  [[nodiscard]] constexpr bool has_builtin(const std::string& name) const
  {
    return _builtins.find(name) != _builtins.end();
  }
};

}