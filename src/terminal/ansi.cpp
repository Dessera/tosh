#include "tosh/terminal/ansi.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/cursor.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace tosh::terminal {

// NOLINTNEXTLINE
ANSIPort::ANSIPort(std::FILE* out, std::FILE* in)
  : _out(out)
  , _in(in)
{
  assert(out != nullptr);
  assert(in != nullptr);

  setvbuf(_in, nullptr, _IONBF, 0);
  setvbuf(_out, nullptr, _IONBF, 0);
}

ANSIPort::~ANSIPort()
{
  setvbuf(_in, nullptr, _IOLBF, BUFSIZ);
  setvbuf(_out, nullptr, _IOLBF, BUFSIZ);
}

error::Result<TermCursor>
ANSIPort::cursor()
{
  std::size_t x = 0;
  std::size_t y = 0;

  RETERR(print("\x1b[6n"));

  if (std::fscanf(_in, "\x1b[%zu;%zuR", &y, &x) != 2) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ x - 1, y - 1 };
}

error::Result<TermCursor>
ANSIPort::winsize()
{
  int _out_fd = fileno(_out);

  // NOLINTNEXTLINE
  struct winsize w;
  if (ioctl(_out_fd, TIOCGWINSZ, &w) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ w.ws_col, w.ws_row };
}

TermCursor
ANSIPort::unsafe_winsize()
{
  int _out_fd = fileno(_out);

  // NOLINTNEXTLINE
  struct winsize w;
  if (ioctl(_out_fd, TIOCGWINSZ, &w) == -1) {
    throw error::raw_err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ w.ws_col, w.ws_row };
}

error::Result<void>
ANSIPort::cursor(const TermCursor& cursor)
{
  return print("\x1b[{};{}H", cursor.y() + 1, cursor.x() + 1);
}

error::Result<void>
ANSIPort::backward(std::size_t n)
{
  if (n != 0) {
    return print("\x1b[{}D", n);
  }
  return {};
}

error::Result<void>
ANSIPort::forward(std::size_t n)
{
  if (n != 0) {
    return print("\x1b[{}C", n);
  }
  return {};
}

error::Result<void>
ANSIPort::up(std::size_t n, bool set_to_start)
{
  if (n != 0 && set_to_start) {
    return print("\x1b[{}F", n);
  }

  if (n != 0 && !set_to_start) {
    return print("\x1b[{}A", n);
  }

  if (n == 0 && set_to_start) {
    return print("\r");
  }

  return {};
}

error::Result<void>
ANSIPort::down(std::size_t n, bool set_to_start)
{
  if (n != 0 && set_to_start) {
    return print("\x1b[{}E", n);
  }

  if (n != 0 && !set_to_start) {
    return print("\x1b[{}B", n);
  }

  if (n == 0 && set_to_start) {
    return print("\r");
  }

  return {};
}

error::Result<void>
ANSIPort::hide()
{
  return print("\x1b[?25l");
}

error::Result<void>
ANSIPort::show()
{
  return print("\x1b[?25h");
}

error::Result<void>
ANSIPort::clear_eol()
{
  return print("\x1b[K");
}

error::Result<void>
ANSIPort::putc(char c)
{
  if (std::fputc(c, _out) == EOF) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }
  return {};
}

error::Result<void>
ANSIPort::puts(const std::string& str)
{
  if (std::fputs(str.c_str(), _out) == EOF) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }
  return {};
}

error::Result<void>
ANSIPort::enable()
{
  int _in_fd = fileno(_in);

  // NOLINTNEXTLINE
  termios term;
  if (tcgetattr(_in_fd, &term) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  term.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(_in_fd, TCSANOW, &term) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return {};
}

error::Result<void>
ANSIPort::disable()
{
  int _in_fd = fileno(_in);

  // NOLINTNEXTLINE
  termios term;
  if (tcgetattr(_in_fd, &term) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  term.c_lflag |= (ICANON | ECHO);
  if (tcsetattr(_in_fd, TCSANOW, &term) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return {};
}

ANSIHideGuard::ANSIHideGuard(ANSIPort& port)
  : _port(&port)
{
  auto _ = _port->hide();
}

ANSIHideGuard::~ANSIHideGuard()
{
  auto _ = _port->show();
}

}
