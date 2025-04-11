#pragma once

#include "tosh/parser/ast/root.hpp"
#include "tosh/utils/redirect.hpp"

#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace tosh::parser {

/**
 * @brief Parser query result
 */
class ParseQuery
{
private:
  ast::Root::NodePtr _root;
  std::vector<std::shared_ptr<utils::RedirectOperation>> _redirects;

  std::istream* _stdin{ &std::cin };
  std::ostream* _stdout{ &std::cout };
  std::ostream* _stderr{ &std::cerr };

public:
  ParseQuery(std::shared_ptr<ast::Root> root,
             std::vector<std::shared_ptr<utils::RedirectOperation>> redirects);

  [[nodiscard]] constexpr const auto& ast() const { return *_root; }
  [[nodiscard]] constexpr auto& ast() { return *_root; }
  [[nodiscard]] constexpr const auto& redirects() const { return _redirects; }
  [[nodiscard]] constexpr auto& redirects() { return _redirects; }

  /**
   * @brief Get the stdin stream
   *
   * @return constexpr std::istream& Current stdin stream
   */
  [[nodiscard]] constexpr std::istream& get_stdin() const noexcept
  {
    return *_stdin;
  }

  /**
   * @brief Set the stdin stream
   *
   * @param in Stream to set as stdin
   */
  constexpr void set_stdin(std::istream& in) noexcept { _stdin = &in; }

  /**
   * @brief Get the stdout stream
   *
   * @return constexpr std::ostream& Current stdout stream
   */
  [[nodiscard]] constexpr std::ostream& get_stdout() const noexcept
  {
    return *_stdout;
  }

  /**
   * @brief Set the stdout stream
   *
   * @param out Stream to set as stdout
   */
  constexpr void set_stdout(std::ostream* out) noexcept { _stdout = out; }

  /**
   * @brief Get the stderr stream
   *
   * @return constexpr std::ostream& Current stderr stream
   */
  [[nodiscard]] constexpr std::ostream& get_stderr() const noexcept
  {
    return *_stderr;
  }

  /**
   * @brief Set the stderr stream
   *
   * @param err Stream to set as stderr
   */
  constexpr void set_stderr(std::ostream& err) noexcept { _stderr = &err; }

  [[nodiscard]] std::vector<std::string> args() const;

  [[nodiscard]] std::string prefix() const;
  void prefix(std::string_view prefix);
};

}
