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

  std::string _home;

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

  constexpr auto& parser() noexcept { return _parser; }

  [[nodiscard]] constexpr auto& home() const noexcept { return _home; }

  /**
   * @brief Check if builtin exists
   *
   * @param name Name of builtin to check
   * @return true Builtin exists
   * @return false Builtin does not exist
   */
  [[nodiscard]] constexpr bool has_builtin(const std::string& name) const
  {
    return _builtins.find(name) != _builtins.end();
  }

  /**
   * @brief Run builtin command
   *
   * @param query Arguments to pass to builtin
   * @param name Name of builtin to run
   * @return error::Result<void> Builtin result
   */
  error::Result<void> run_builtin(parser::ParseQuery& query,
                                  const std::string& name);

  std::optional<std::string> find_command(std::string_view command);

private:
  /**
   * @brief Run builtin command with redirections
   *
   * @param query Arguments to pass to builtin
   * @param name Name of builtin to run
   */
  void _run_builtin(parser::ParseQuery& query, const std::string& name);
};
}