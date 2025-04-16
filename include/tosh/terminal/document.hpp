#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/cursor.hpp"

#include <cstddef>
#include <cstdio>
#include <string>

namespace tosh::terminal {

class TOSH_EXPORT Document
{
private:
  std::string _prompt;

  ANSIPort _term;

  /**
   * @brief User input buffer
   */
  std::string _buffer;

  /**
   * @brief Real position of the cursor in the buffer
   */
  std::size_t _cursor{ 0 };

  TermCursor _wsize;

public:
  Document(std::FILE* out, std::FILE* in, std::string prompt);

  error::Result<void> insert(char c);
  error::Result<void> backward(std::size_t n = 1);

private:
  TermCursor unsafe_get_wsize();
  TermCursor get_vcursor_from_pos(std::size_t pos);
};

}
