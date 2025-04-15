#pragma once

#include "tosh/error.hpp"
#include "tosh/parser/ast/root.hpp"
#include "tosh/parser/query.hpp"
#include "tosh/utils/buffer.hpp"

namespace tosh::repl {

class Repl;

}

namespace tosh::parser {

class TokenParser
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
  void handle_completion(repl::Repl& repl,
                         ast::Root::Ptr& root,
                         utils::CommandBuffer& buffer);

  /**
   * @brief Rebuilds the AST
   *
   * @param root The root node of the AST
   * @param buffer The command buffer
   */
  void handle_rebuild_ast(ast::Root::Ptr& root, utils::CommandBuffer& buffer);
};

}
