#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/parser.hpp"
#include "tosh/parser/query.hpp"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <vector>

namespace tosh::repl {

class Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;
  parser::TokenParser _parser{};
  pid_t _subpid{ -1 };

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
   * @brief Run builtin command with no fd operations
   *
   * @param query Arguments to pass to builtin
   * @param name Name of builtin to run
   * @return error::Result<void> Builtin result
   */
  error::Result<void> run_builtin_no_ops(parser::ParseQuery& query,
                                         const std::string& name);

  std::vector<std::string> find_command_full(std::string_view command);

  std::vector<std::string> find_command_fuzzy(std::string_view command);
  std::vector<std::string> find_builtin_fuzzy(std::string_view command);
  std::vector<std::string> find_fuzzy(std::string_view command);

  void sigint_handler();

private:
  /**
   * @brief Run builtin command with redirections
   *
   * @param query Arguments to pass to builtin
   * @param name Name of builtin to run
   */
  void run_builtin(parser::ParseQuery& query, const std::string& name);
};

}