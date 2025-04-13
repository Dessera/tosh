#include "tosh/utils/buffer.hpp"
#include <algorithm>
#include <cstddef>
#include <ostream>
namespace tosh::utils {

CommandBuffer::CommandBuffer(std::ostream& out)
  : _out(&out)
{
}

size_t
CommandBuffer::backward(size_t n)
{
  auto step = std::min(n, _cursor);
  if (step > 0) {
    _cursor -= step;
    *_out << "\033[" << step << "D";
  }

  return step;
}

size_t
CommandBuffer::forward(size_t n)
{
  auto step = std::min(n, _buffer.size() - _cursor);
  if (step > 0) {
    _cursor += step;
    *_out << "\033[" << step << "C";
  }

  return step;
}

void
CommandBuffer::insert(char c)
{
  _buffer.insert(_cursor, 1, c);
  *_out << _buffer.substr(_cursor);

  ++_cursor;

  auto step = _buffer.size() - _cursor;
  if (step > 0) {
    *_out << "\033[" << _buffer.size() - _cursor << "D";
  }
}

void
CommandBuffer::stream_insert(char c)
{
  *_out << c;
}

void
CommandBuffer::remove(size_t n)
{
  auto bufsize = _buffer.size();
  auto step = backward(n);

  if (step > 0) {
    _buffer.erase(_cursor, step);
    *_out << _buffer.substr(_cursor)
          << std::string(bufsize - _buffer.size(), ' ') << "\033["
          << bufsize - _cursor << "D";
  }
}

}
