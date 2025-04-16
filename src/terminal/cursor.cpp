#include "tosh/terminal/cursor.hpp"
namespace tosh::terminal {

// NOLINTNEXTLINE
TermCursor::TermCursor(std::size_t x, std::size_t y)
  : _x(x)
  , _y(y)
{
}

}