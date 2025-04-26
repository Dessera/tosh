#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/terminal.hpp"

#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>
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
  Document(Terminal term, std::string prompt, TermCursor wsize);
  ~Document();

  TOSH_DELETE_COPY(Document)
  TOSH_DEFAULT_MOVE(Document)

  error::Result<Event> get_op();

  /**
   * @brief Insert a character at the cursor position
   *
   * @param c Character to insert
   * @return error::Result<void> Operation result
   */
  error::Result<void> insert(char c);

  /**
   * @brief Remove a character at the cursor position
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> remove();

  /**
   * @brief Move the cursor backward
   *
   * @return error::Result<void>
   * @warning This function cannot handle scrolling
   */
  error::Result<void> backward();

  /**
   * @brief Move the cursor forward
   *
   * @return error::Result<void> Operation result
   * @warning This function cannot handle scrolling
   */
  error::Result<void> forward();

  /**
   * @brief Start a new command
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> enter();

  /**
   * @brief Resize the terminal with current win size
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> resize();

  [[nodiscard]] std::string string() const;

  [[nodiscard]] constexpr bool end() const
  {
    return _cpos.y == _buffer.size() - 1 && _cpos.x == _buffer.back().size();
  }

  static error::Result<Document> create(std::FILE* out,
                                        std::FILE* in,
                                        std::string prompt);

private:
  /**
   * @brief Rebuild the buffer starting from a given position
   *
   * @param start Position to start from
   */
  void rebuild_buffer(size_t start);

  /**
   * @brief Fixup the cursor position
   *
   * @param cursor Cursor to fixup
   * @return std::pair<bool, bool> If the cursor was fixupped in x and y
   */
  std::pair<bool, bool> fixup_cursor(TermCursor& cursor) const;

  /**
   * @brief Get the cursor position for the prompt
   *
   * @return TermCursor Cursor position for the prompt
   */
  [[nodiscard]] TermCursor prompt_cursor() const;
};

}
