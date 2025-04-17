#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/cursor.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <string_view>

namespace tosh::terminal {

enum class CleanType : uint8_t
{
  TOEND,
  TOBEGIN,
  ALL
};

class TOSH_EXPORT ANSIPort
{
private:
  std::FILE* _out;
  std::FILE* _in;

public:
  ANSIPort(std::FILE* out, std::FILE* in);
  ~ANSIPort();

  /**
   * @brief Get the terminal cursor
   *
   * @return error::Result<TermCursor> Terminal cursor
   */
  error::Result<TermCursor> cursor();

  /**
   * @brief Get the terminal size
   *
   * @return error::Result<TermCursor> Terminal size
   */
  error::Result<TermCursor> winsize();
  TermCursor unsafe_winsize();

  /**
   * @brief Set the terminal cursor
   *
   * @param cursor Terminal cursor
   * @return error::Result<void> Operation result
   */
  error::Result<void> cursor(const TermCursor& cursor);

  /**
   * @brief Move the cursor up, optionally redirecting to the start of the
   * line.
   *
   * @param n Number of lines to move up
   * @param set_to_start Whether to redirect to the start of the line
   * @return error::Result<void> Operation result
   */
  error::Result<void> up(std::size_t n = 1, bool set_to_start = false);
  error::Result<void> down(std::size_t n = 1, bool set_to_start = false);
  error::Result<void> backward(std::size_t n = 1);
  error::Result<void> forward(std::size_t n = 1);

  error::Result<void> hide();
  error::Result<void> show();

  /**
   * @brief Clear the current line
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> cleanline(CleanType type = CleanType::ALL);
  error::Result<void> clean(CleanType type = CleanType::ALL);

  /**
   * @brief Print a character to the terminal
   *
   * @param c Character to print
   * @return error::Result<void> Operation result
   */
  error::Result<void> putc(char c);
  error::Result<void> puts(const std::string& str);
  error::Result<void> puts(std::string_view str);

  /**
   * @brief Print a formatted string to the terminal
   *
   * @tparam Args Format arguments
   * @param fmt Format string
   * @param args Format arguments
   * @return constexpr error::Result<void> Operation result
   */
  template<typename... Args>
  constexpr error::Result<void> print(std::format_string<Args...> fmt,
                                      Args&&... args)
  {
    if (std::fputs(std::format(fmt, std::forward<Args>(args)...).c_str(),
                   _out) == EOF) {
      return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
    }
    return {};
  }

  /**
   * @brief Enable raw mode
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> enable();

  /**
   * @brief Disable raw mode
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> disable();
};

class TOSH_EXPORT ANSIHideGuard
{
private:
  ANSIPort* _port;

public:
  ANSIHideGuard(ANSIPort& port);
  ~ANSIHideGuard();
};

}
