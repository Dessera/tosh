#pragma once

#include "tosh/builtins/base.hpp"
#include "tosh/common.hpp"
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

class TOSH_EXPORT Repl
{
private:
  std::map<std::string, std::shared_ptr<builtins::BaseCommand>> _builtins;
  parser::TokenParser _parser{};
  pid_t _subpid{ -1 };

public:
  Repl();

  /**
   * @brief Repl entry point
   */
  [[noreturn]] void run();

  /**
   * @brief Run callback in another process
   *
   * @param query Arguments to pass to callback
   * @param callback Callback to run in another process
   * @return error::Result<int> Process result
   */
  error::Result<int> run_proc(
    parser::ParseQuery& query,
    const std::function<error::Result<int>(parser::ParseQuery&)>& callback);

  /**
   * @brief Get token parser
   *
   * @return constexpr auto& Token parser
   */
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
   * @return error::Result<int> Builtin result
   */
  error::Result<int> run_builtin_no_ops(parser::ParseQuery& query,
                                        const std::string& name);

  /**
   * @brief Expand command to full path
   *
   * @param command Command to expand
   * @return std::vector<std::string> Expanded commands
   */
  std::vector<std::string> find_command_full(std::string_view command);

  /**
   * @brief Find command name with fuzzy matching
   *
   * @param command Command to find
   * @return std::vector<std::string> Command names
   */
  std::vector<std::string> find_command_fuzzy(std::string_view command);

  /**
   * @brief Find builtin name with fuzzy matching
   *
   * @param command Command to find
   * @return std::vector<std::string> Builtin names
   */
  std::vector<std::string> find_builtin_fuzzy(std::string_view command);

  /**
   * @brief Find symbol name with fuzzy matching
   *
   * @param command Command to find
   * @return std::vector<std::string> Symbol names
   */
  std::vector<std::string> find_fuzzy(std::string_view command);

  /**
   * @brief Handle SIGINT signal
   */
  void sigint_handler();

private:
  /**
   * @brief Run builtin command with redirections
   *
   * @param query Arguments to pass to builtin
   * @param name Name of builtin to run
   */
  void run_builtin(parser::ParseQuery& query, const std::string& name);

  /**
   * @brief Get the shell prompt
   *
   * @return std::string Shell prompt
   */
  std::string get_prompt();
};

}
