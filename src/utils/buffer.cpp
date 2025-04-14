#include "tosh/utils/buffer.hpp"

#include <cstddef>
#include <cstdio>
#include <ostream>
#include <termios.h>
#include <unistd.h>

namespace tosh::utils {

bool
is_command(char c, CommandType type)
{
  return static_cast<char>(type) == c;
}

bool
is_ascii(char c)
{
  return c >= ' ' && c <= '~';
}

CommandBuffer::CommandBuffer(std::ostream& out)
  : _out(&out)
{
  enable_raw_mode();
}

CommandBuffer::~CommandBuffer()
{
  disable_raw_mode();
}

size_t
CommandBuffer::backward()
{
  if (_cursor > 0 && _buffer[_cursor - 1] != '\n') {
    --_cursor;
    *_out << "\033[1D";
    return 1;
  }
  return 0;
}

size_t
CommandBuffer::forward()
{
  if (_cursor < _buffer.size()) {
    ++_cursor;
    *_out << "\033[1C";
    return 1;
  }
  return 0;
}

void
CommandBuffer::insert(char c)
{
  if (c == '\n') {
    *_out << std::string(_buffer.size() - _cursor, ' ') << "\033["
          << _buffer.size() - _cursor << "D";
  }

  _buffer.insert(_cursor, 1, c);
  *_out << _buffer.substr(_cursor);

  ++_cursor;

  auto step = _buffer.size() - _cursor;
  if (step > 0) {
    *_out << "\033[" << _buffer.size() - _cursor << "D";
  }
}

void
CommandBuffer::insert(std::string_view str)
{
  for (auto c : str) {
    insert(c);
  }
}

void
CommandBuffer::stream_insert(char c)
{
  *_out << c;
}

void
CommandBuffer::remove()
{
  auto bufsize = _buffer.size();
  auto step = backward();

  if (step > 0) {
    _buffer.erase(_cursor, step);
    *_out << _buffer.substr(_cursor)
          << std::string(bufsize - _buffer.size(), ' ') << "\033["
          << bufsize - _cursor << "D";
  }
}

char
CommandBuffer::getchar()
{
  auto c = static_cast<char>(std::getchar());
  while (true) {
    if (is_ascii(c) || is_command(c, CommandType::NEXT_LINE)) {
      insert(c);
      return c;
    }

    if (is_command(c, CommandType::BACKSPACE)) {
      remove();
      return c;
    }

    if (is_command(c, CommandType::END) || is_command(c, CommandType::TAB)) {
      return c;
    }

    if (is_command(c, CommandType::ESCAPE)) {
      c = static_cast<char>(std::getchar());
      if (!is_command(c, CommandType::LWIN)) {
        continue;
      }
      c = static_cast<char>(std::getchar());
      if (c == 'D') {
        backward();
      } else if (c == 'C') {
        forward();
      }
    }

    c = static_cast<char>(std::getchar());
  }
}

void
CommandBuffer::enable_raw_mode()
{
  // NOLINTNEXTLINE
  termios trim;
  tcgetattr(STDIN_FILENO, &trim);
  trim.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &trim);
}

void
CommandBuffer::disable_raw_mode()
{
  // NOLINTNEXTLINE
  termios trim;
  tcgetattr(STDIN_FILENO, &trim);
  trim.c_lflag |= (ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &trim);
}

}
