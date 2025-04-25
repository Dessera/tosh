#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/utils/redirect.hpp"
#include <memory>
#include <vector>

namespace tosh::repl {

class Repl;

}

namespace tosh::parser {

class TOSH_EXPORT TokenParser
{
public:
  /**
   * @brief Parses the input from the stdin
   *
   * @param repl The repl object
   * @return error::Result<ParseQuery> Parse query if successful, error
   * otherwise
   */
  error::Result<ParseQuery> parse(repl::Repl& repl);

private:
  /**
   * @brief Completes the current token
   *
   * @param repl The repl object
   * @param root The root node of the AST
   * @param buffer The command buffer
   */
  // void handle_completion(repl::Repl& repl,
  //                        ast::Root::Ptr& root,
  //                        utils::CommandBuffer& buffer);

  /**
   * @brief Rebuilds the AST
   *
   * @param root The root node of the AST
   * @param buffer The command buffer
   */
  void handle_rebuild_ast(repl::Repl& repl, ast::Root::Ptr& root);

  /**
   * @brief Handles the end of the input
   *
   * @param root The root node of the AST
   * @param buffer The command buffer
   */
  // void handle_cin_eof(ast::Root::Ptr& root, utils::CommandBuffer& buffer);

  /**
   * @brief Creates the redirects from the AST
   *
   * @param root The root node of the AST
   * @return std::vector<std::shared_ptr<utils::RedirectOperation>> Redirect
   * operations
   */
  std::vector<std::shared_ptr<utils::RedirectOperation>> make_redirects(
    ast::Root::Ptr& root);
};

}
