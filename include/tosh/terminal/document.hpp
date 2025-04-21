#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/terminal.hpp"

#include <cstddef>
#include <cstdio>
#include <list>
#include <string>

namespace tosh::terminal {

class TOSH_EXPORT Document
{
private:
  std::string _prompt;

  Terminal _term;

  std::string _buffer;
  std::size_t _cpos;

  TermCursor _wsize;

public:
  Document(std::FILE* out, std::FILE* in, std::string prompt);
  ~Document();

  error::Result<std::string> gets();

  /**
   * @brief Insert a character at the cursor position
   *
   * @param c Character to insert
   * @return error::Result<void> Operation result
   */
  error::Result<void> insert(char c);

  /**
   * @brief Move the cursor backward
   *
   * @param n Number of positions to move backward
   * @return error::Result<void>
   */
  error::Result<void> backward(std::size_t n = 1);

  /**
   * @brief Move the cursor forward
   *
   * @param n Number of positions to move forward
   * @return error::Result<void> Operation result
   */
  error::Result<void> forward(std::size_t n = 1);

  /**
   * @brief Start a new command
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> enter();

  /**
   * @brief End the current command
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> leave();

  error::Result<void> resize(const TermCursor& size);
};

}
