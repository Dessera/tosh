#include "tosh/terminal/terminal.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/event/reader.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <expected>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace tosh::terminal {

bool
operator==(const TermCursor& lhs, const TermCursor& rhs) noexcept
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

// NOLINTNEXTLINE
Terminal::Terminal(std::FILE* out, std::FILE* in, EventReader reader)
  : _out(out)
  , _in(in)
  , _reader(std::move(reader))
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
  RETERR(puts_impl("\x1b[6n"));
  auto event =
    UNWRAPERR(_reader.read(EventFilter<EventGetCursor>(), TERM_TIMEOUT));

  if (auto* eptr = std::get_if<EventGetCursor>(&event); eptr != nullptr) {
    return TermCursor{ .x = eptr->x, .y = eptr->y };
  }

  [[unlikely]] return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
}

error::Result<TermCursor>
Terminal::winsize()
{
  // NOLINTNEXTLINE
  struct winsize w;
  if (ioctl(fileno(_out), TIOCGWINSZ, &w) == -1) {
    return error::err(error::ErrorCode::UNEXPECTED_IO_STATUS);
  }

  return TermCursor{ .x = w.ws_col, .y = w.ws_row };
}

TermCursor
Terminal::unsafe_winsize()
{
  auto res = winsize();
  if (!res.has_value()) {
    throw error::Error(res.error());
  }

  return res.value();
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

error::Result<Event>
Terminal::get_op()
{
  auto event = UNWRAPERR(_reader.read(
    EventFilter<EventGetString, EventSpecialKey, EventMoveCursor>()));
  return event;
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

  RETERR(_reader.start());

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

  RETERR(_reader.stop());

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

error::Result<Terminal>
Terminal::create(std::FILE* out, std::FILE* in)
{
  assert(out != nullptr);
  assert(in != nullptr);

  auto reader = EventReader::create(in);
  if (!reader.has_value()) {
    return std::unexpected(reader.error());
  }

  return Terminal{ out, in, std::move(reader.value()) };
}

TermCursorHideGuard::TermCursorHideGuard(Terminal& port)
  : _port(&port)
{
  auto _ = _port->hide();
}

TermCursorHideGuard::~TermCursorHideGuard()
{
  auto _ = _port->show();
}

}
