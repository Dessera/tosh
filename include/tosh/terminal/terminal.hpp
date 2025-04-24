#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/event/reader.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <format>
#include <string_view>

namespace tosh::terminal {

struct TermCursor
{
  std::size_t x;
  std::size_t y;
};

bool
operator==(const TermCursor& lhs, const TermCursor& rhs) noexcept;

enum class CleanType : uint8_t
{
  TOEND,
  TOBEGIN,
  ALL
};

class Terminal
{
private:
  std::FILE* _out;
  std::FILE* _in;

  EventReader _reader;

public:
  Terminal(std::FILE* out, std::FILE* in);
  ~Terminal();

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

  /**
   * @brief Get the terminal size
   *
   * @return TermCursor Terminal size
   */
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

  /**
   * @brief Move the cursor down, optionally redirecting to the start of the
   *
   * @param n Number of lines to move down
   * @param set_to_start Whether to redirect to the start of the line
   * @return error::Result<void> Operation result
   */
  error::Result<void> down(std::size_t n = 1, bool set_to_start = false);

  /**
   * @brief Move the cursor left, optionally redirecting to the end of the
   *
   * @param n Number of lines to move left
   * @return error::Result<void> Operation result
   */
  error::Result<void> backward(std::size_t n = 1);

  /**
   * @brief Move the cursor right, optionally redirecting to the end of the
   *
   * @param n Number of lines to move right
   * @return error::Result<void> Operation result
   */
  error::Result<void> forward(std::size_t n = 1);

  /**
   * @brief Hide the cursor
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> hide();

  /**
   * @brief Show the cursor
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> show();

  /**
   * @brief Clear the current line
   *
   * @return error::Result<void> Operation result
   */
  error::Result<void> cleanline(CleanType type = CleanType::ALL);

  /**
   * @brief Clear the terminal
   *
   * @param type Clear type
   * @return error::Result<void> Operation result
   */
  error::Result<void> clean(CleanType type = CleanType::ALL);

  /**
   * @brief Print a character to the terminal
   *
   * @param c Character to print
   * @return error::Result<void> Operation result
   */
  error::Result<void> putc(char c);

  /**
   * @brief Print a string to the terminal
   *
   * @param str String to print
   * @return error::Result<void> Operation result
   */
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
    return print_impl(fmt, std::forward<Args>(args)...);
  }

  error::Result<Event> get_op();

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

private:
  error::Result<void> putc_impl(char c);
  error::Result<void> puts_impl(std::string_view str);

  template<typename... Args>
  error::Result<void> print_impl(std::format_string<Args...> fmt,
                                 Args&&... args)
  {
    if (std::fputs(std::format(fmt, std::forward<Args>(args)...).c_str(),
                   _out) == EOF) {
      return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
    }
    return {};
  }
};

class TOSH_EXPORT ANSIHideGuard
{
private:
  Terminal* _port;

public:
  ANSIHideGuard(Terminal& port);
  ~ANSIHideGuard();

  ANSIHideGuard(const ANSIHideGuard&) = delete;
  ANSIHideGuard& operator=(const ANSIHideGuard&) = delete;
  ANSIHideGuard(ANSIHideGuard&&) = delete;
  ANSIHideGuard& operator=(ANSIHideGuard&&) = delete;
};

}
