#pragma once

#include "tosh/common.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/cursor.hpp"
#include <cstddef>
#include <cstdio>
#include <format>
#include <string>

namespace tosh::terminal {

class TOSH_EXPORT ANSIPort
{
public:
  constexpr static size_t RD_BUFSIZ = 32;

private:
  std::FILE* _out;
  std::FILE* _in;

public:
  ANSIPort(std::FILE* out, std::FILE* in);
  ~ANSIPort();

  error::Result<TermCursor> cursor();
  error::Result<TermCursor> size();
  void cursor(const TermCursor& cursor);

  /**
   * @brief Move the cursor up, optionally redirecting to the start of the
   * line.
   *
   * @param n
   * @param wrap
   * @return error::Result<void>
   */
  error::Result<void> up(std::size_t n = 1, bool set_to_start = false);
  error::Result<void> down(std::size_t n = 1, bool set_to_start = false);
  error::Result<void> backward(std::size_t n = 1);
  error::Result<void> forward(std::size_t n = 1);

  error::Result<void> hide();
  error::Result<void> show();

  error::Result<void> clear_eol();

  constexpr void putc(char c) { std::fputc(c, _out); }
  constexpr void puts(const std::string& str) { std::fputs(str.c_str(), _out); }

private:
  void enable_raw_mode();
  void disable_raw_mode();

  template<typename... Args>
  constexpr error::Result<void> putcmd(std::format_string<Args...> fmt,
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
  ANSIPort* _port;

public:
  ANSIHideGuard(ANSIPort& port);
  ~ANSIHideGuard();
};

}
