#include "tosh/terminal/ansi.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/cursor.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <mutex>
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

  // setvbuf(_in, nullptr, _IONBF, 0);
  setvbuf(_out, nullptr, _IONBF, 0);
}

ANSIPort::~ANSIPort()
{
  // setvbuf(_in, nullptr, _IOLBF, BUFSIZ);
  setvbuf(_out, nullptr, _IOLBF, BUFSIZ);
}

error::Result<TermCursor>
ANSIPort::cursor()
{
  std::lock_guard lock(_mutex);

  std::size_t x = 0;
  std::size_t y = 0;

  // use ioctl?
  // RETERR(puts_impl("\x1b[6n"));

  // if (std::fscanf(_in, "\x1b[%zu;%zuR", &y, &x) != 2) {
  //   return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  // }

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
  std::lock_guard lock(_mutex);

  return print_impl("\x1b[{};{}H", cursor.y() + 1, cursor.x() + 1);
}

error::Result<void>
ANSIPort::backward(std::size_t n)
{
  std::lock_guard lock(_mutex);

  if (n != 0) {
    return print_impl("\x1b[{}D", n);
  }
  return {};
}

error::Result<void>
ANSIPort::forward(std::size_t n)
{
  std::lock_guard lock(_mutex);

  if (n != 0) {
    return print_impl("\x1b[{}C", n);
  }
  return {};
}

error::Result<void>
ANSIPort::up(std::size_t n, bool set_to_start)
{
  std::lock_guard lock(_mutex);

  if (n != 0 && set_to_start) {
    return print_impl("\x1b[{}F", n);
  }

  if (n != 0 && !set_to_start) {
    return print_impl("\x1b[{}A", n);
  }

  if (n == 0 && set_to_start) {
    return putc_impl('\r');
  }

  return {};
}

error::Result<void>
ANSIPort::down(std::size_t n, bool set_to_start)
{
  std::lock_guard lock(_mutex);

  if (n != 0 && set_to_start) {
    return print_impl("\x1b[{}E", n);
  }

  if (n != 0 && !set_to_start) {
    return print_impl("\x1b[{}B", n);
  }

  if (n == 0 && set_to_start) {
    return putc_impl('\n');
  }

  return {};
}

error::Result<void>
ANSIPort::hide()
{
  std::lock_guard lock(_mutex);

  return puts_impl("\x1b[?25l");
}

error::Result<void>
ANSIPort::show()
{
  std::lock_guard lock(_mutex);

  return puts_impl("\x1b[?25h");
}

error::Result<void>
ANSIPort::cleanline(CleanType type)
{
  std::lock_guard lock(_mutex);

  switch (type) {
    case CleanType::TOEND:
      return puts_impl("\x1b[K");
    case CleanType::TOBEGIN:
      return puts_impl("\x1b[1K");
    case CleanType::ALL:
      return puts_impl("\x1b[2K");
    default:
      return {};
  }
}

error::Result<void>
ANSIPort::clean(CleanType type)
{
  std::lock_guard lock(_mutex);

  switch (type) {
    case CleanType::TOEND:
      return puts_impl("\x1b[0J");
    case CleanType::TOBEGIN:
      return puts_impl("\x1b[1J");
    case CleanType::ALL:
      return puts_impl("\x1b[2J");
    default:
      return {};
  }
}

error::Result<void>
ANSIPort::putc(char c)
{
  std::lock_guard lock(_mutex);

  return putc_impl(c);
}

error::Result<void>
ANSIPort::puts(std::string_view str)
{
  std::lock_guard lock(_mutex);

  return puts_impl(str);
}

char
ANSIPort::getchar()
{
  std::lock_guard lock(_mutex);

  return static_cast<char>(std::fgetc(_in));
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

error::Result<void>
ANSIPort::putc_impl(char c)
{
  if (std::fputc(c, _out) == EOF) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return {};
}

error::Result<void>
ANSIPort::puts_impl(std::string_view str)
{
  if (std::fwrite(str.data(), str.size(), 1, _out) == 0) {
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
