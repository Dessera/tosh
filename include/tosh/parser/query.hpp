#pragma once

#include "tosh/parser/ast/root.hpp"
#include "tosh/utils/redirect.hpp"

#include <memory>
#include <string>
#include <vector>

namespace tosh::parser {

/**
 * @brief Parser query result
 */
class ParseQuery
{
private:
  ast::Root::Ptr _root;
  std::vector<std::shared_ptr<utils::RedirectOperation>> _redirects;

public:
  ParseQuery(ast::Root::Ptr root,
             std::vector<std::shared_ptr<utils::RedirectOperation>> redirects);

  /**
   * @brief Get AST root
   *
   * @return constexpr const auto& AST root
   */
  [[nodiscard]] constexpr const auto& ast() const { return *_root; }

  /**
   * @brief Get AST root
   *
   * @return constexpr auto& AST root
   */
  [[nodiscard]] constexpr auto& ast() { return *_root; }

  /**
   * @brief Get the redirect operations
   *
   * @return constexpr const auto& Redirect operations
   */
  [[nodiscard]] constexpr const auto& redirects() const noexcept
  {
    return _redirects;
  }

  /**
   * @brief Get the redirect operations
   *
   * @return constexpr auto& Redirect operations
   */
  [[nodiscard]] constexpr auto& redirects() noexcept { return _redirects; }

  /**
   * @brief Convert the AST to sequence of strings for `exec`
   *
   * @return std::vector<std::string> Sequence of args
   */
  [[nodiscard]] std::vector<std::string> args() const;

  /**
   * @brief Returns the prefix of the AST
   *
   * @return std::string Prefix string
   */
  [[nodiscard]] std::string prefix() const;

  /**
   * @brief Inserts a prefix to the AST
   *
   * @param prefix String to insert
   */
  void prefix(std::string_view prefix);
};

}
