#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/terminal.hpp"

#include <cstddef>
#include <cstdio>
#include <list>
#include <string>
#include <vector>

namespace tosh::terminal {

class TOSH_EXPORT Document
{
private:
  std::string _prompt;

  Terminal _term;

  std::vector<std::string> _buffer;
  TermCursor _cpos{ .x = 0, .y = 0 };

  TermCursor _wsize;

public:
  Document(std::FILE* out, std::FILE* in, std::string prompt);
  ~Document();

  error::Result<Event> get_op();

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
  error::Result<void> backward();

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

private:
  /**
   * @brief Rebuild the buffer starting from a given position
   *
   * @param start Position to start from
   */
  void rebuild_buffer(size_t start);
  bool fixup_cursor(TermCursor& cursor) const;
};

}
