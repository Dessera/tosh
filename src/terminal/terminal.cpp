#include "tosh/terminal/terminal.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace tosh::terminal {

// NOLINTNEXTLINE
Terminal::Terminal(std::FILE* out, std::FILE* in)
  : _out(out)
  , _in(in)
  , _reader(in)
{
  assert(out != nullptr);
  assert(in != nullptr);

  setvbuf(_out, nullptr, _IONBF, 0);
}

Terminal::~Terminal()
{
  setvbuf(_out, nullptr, _IOLBF, BUFSIZ);
}

error::Result<TermCursor>
Terminal::cursor()
{
  using namespace std::chrono_literals;

  RETERR(puts_impl("\x1b[6n"));

  while (true) {
    auto res = _reader.read(EventFilter<EventGetCursor>(), 2s);
    if (!res.has_value() &&
        res.error().code() == error::ErrorCode::EVENT_TIMEOUT) {
      continue;
    }

    auto event = UNWRAPERR(res);
    if (auto* eptr = std::get_if<EventGetCursor>(&event); eptr != nullptr) {
      return TermCursor{ .x = eptr->x, .y = eptr->y };
    }
  }
}

error::Result<TermCursor>
Terminal::winsize()
{
  int _out_fd = fileno(_out);

  // NOLINTNEXTLINE
  struct winsize w;
  if (ioctl(_out_fd, TIOCGWINSZ, &w) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ .x = w.ws_col, .y = w.ws_row };
}

TermCursor
Terminal::unsafe_winsize()
{
  int _out_fd = fileno(_out);

  // NOLINTNEXTLINE
  struct winsize w;
  if (ioctl(_out_fd, TIOCGWINSZ, &w) == -1) {
    throw error::raw_err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ .x = w.ws_col, .y = w.ws_row };
}

error::Result<void>
Terminal::cursor(const TermCursor& cursor)
{
  return print_impl("\x1b[{};{}H", cursor.y + 1, cursor.x + 1);
}

error::Result<void>
Terminal::backward(std::size_t n)
{
  if (n != 0) {
    return print_impl("\x1b[{}D", n);
  }
  return {};
}

error::Result<void>
Terminal::forward(std::size_t n)
{
  if (n != 0) {
    return print_impl("\x1b[{}C", n);
  }
  return {};
}

error::Result<void>
Terminal::up(std::size_t n, bool set_to_start)
{
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
Terminal::down(std::size_t n, bool set_to_start)
{
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
Terminal::hide()
{
  return puts_impl("\x1b[?25l");
}

error::Result<void>
Terminal::show()
{
  return puts_impl("\x1b[?25h");
}

error::Result<void>
Terminal::cleanline(CleanType type)
{
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
Terminal::clean(CleanType type)
{
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
Terminal::putc(char c)
{
  return putc_impl(c);
}

error::Result<void>
Terminal::puts(std::string_view str)
{
  return puts_impl(str);
}

error::Result<std::string>
Terminal::gets()
{
  auto event = UNWRAPERR(_reader.read(EventFilter<EventGetString>()));
  if (auto* eptr = std::get_if<EventGetString>(&event); eptr != nullptr) {
    return eptr->str;
  }

  return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
}

error::Result<void>
Terminal::enable()
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
Terminal::disable()
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
Terminal::putc_impl(char c)
{
  if (std::fputc(c, _out) == EOF) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return {};
}

error::Result<void>
Terminal::puts_impl(std::string_view str)
{
  if (std::fwrite(str.data(), str.size(), 1, _out) == 0) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return {};
}

ANSIHideGuard::ANSIHideGuard(Terminal& port)
  : _port(&port)
{
  auto _ = _port->hide();
}

ANSIHideGuard::~ANSIHideGuard()
{
  auto _ = _port->show();
}

}
