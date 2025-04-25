#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/terminal.hpp"

#include <cassert>
#include <cstddef>
#include <expected>
#include <limits>
#include <ranges>
#include <utility>
#include <vector>

namespace tosh::terminal {

Document::Document(Terminal term, std::string prompt)
  : _prompt(std::move(prompt))
  , _term(std::move(term))
  , _wsize(_term.unsafe_winsize())
{
}

Document::~Document() = default;

error::Result<void>
Document::insert(char c)
{
  // x & y should inside the buffer
  assert(_cpos.y < _buffer.size());
  assert(_cpos.x <= _buffer.at(_cpos.y).size());

  // c should be printable
  assert((c >= ' ' && c <= '~') || c == '\n');

  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto precur = UNWRAPERR(_term.cursor());

  _buffer.at(_cpos.y).insert(_cpos.x, 1, c);
  rebuild_buffer(_cpos.y);

  // real line size to be printed
  auto bufline_sz = _buffer.size() - _cpos.y;

  TermCursorHideGuard guard{ _term };

  RETERR(_term.putc('\r'));
  RETERR(_term.clean(CleanType::TOEND));

  auto str =
    _buffer | views::drop(_cpos.y) | views::join | ranges::to<std::string>();
  RETERR(_term.puts(str));

  auto endcur = UNWRAPERR(_term.cursor());
  auto [x_of, y_of] = fixup_cursor(endcur);

  // printed line size (maybe shrinked because of scrolling)
  auto ioline_sz = endcur.y - precur.y + 1;
  // fixup line offset
  auto loffs = bufline_sz - ioline_sz;

  _cpos.x++;
  precur.x++;
  if (_cpos.x >= _wsize.x || _buffer.at(_cpos.y).at(_cpos.x - 1) == '\n') {
    _cpos.x = 0;
    _cpos.y++;

    precur.x = 0;
    precur.y++;
  }

  // vbuf overflow -> new line
  if (_cpos.y >= _buffer.size()) {
    _buffer.emplace_back();
  }

  precur.y -= loffs;

  /**
   * if the cursor is at the end of the screen, we should create a new line
   * manually.
   */
  if (precur.x == endcur.x && precur.y == endcur.y && y_of) {
    RETERR(_term.putc('\n'));
  }

  RETERR(_term.cursor(precur));

  return {};
}

error::Result<void>
Document::remove()
{
  // x & y should inside the buffer
  assert(_cpos.y < _buffer.size());
  assert(_cpos.x <= _buffer.at(_cpos.y).size());

  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  if (prompt_cursor() == _cpos) {
    return {};
  }

  auto pcur = UNWRAPERR(_term.cursor());
  if (pcur.y == 0 && pcur.x == 0) {
    // refuse to deletion because the implementation cannot handle it :(
    return {};
  }

  if (_cpos.y > 0 && _cpos.x == 0) {
    _cpos.y--;
    pcur.y--;

    _cpos.x = _buffer.at(_cpos.y).size();
  }

  _cpos.x--;

  pcur.x = _cpos.x;

  _buffer.at(_cpos.y).erase(_cpos.x, 1);
  rebuild_buffer(_cpos.y);

  RETERR(_term.cursor(pcur));
  RETERR(_term.putc('\r'));
  RETERR(_term.clean(CleanType::TOEND));

  auto str =
    _buffer | views::drop(_cpos.y) | views::join | ranges::to<std::string>();
  RETERR(_term.puts(str));

  RETERR(_term.cursor(pcur));

  return {};
}

error::Result<void>
Document::backward()
{
  // x & y should inside the buffer
  assert(_cpos.y < _buffer.size());
  assert(_cpos.x <= _buffer.at(_cpos.y).size());

  if (prompt_cursor() == _cpos) {
    return {};
  }

  auto pcur = UNWRAPERR(_term.cursor());
  if (pcur.y == 0 && pcur.x == 0) {
    return {};
  }

  if (_cpos.x != 0) {
    _cpos.x--;
  } else if (_cpos.y != 0) {
    _cpos.y--;
    pcur.y--;

    auto& line = _buffer.at(_cpos.y);

    /**
     * any line should have at least one character
     *   normal line    -> XXXX\n
     *   overflow line  -> XXXXXX
     *   blank line     -> \n
     *   last line      -> XXXX
     */
    assert(line.size() >= 1);

    _cpos.x = line.size();
    if (line.at(_cpos.x - 1) == '\n') {
      _cpos.x--;
    }
  }

  pcur.x = _cpos.x;

  RETERR(_term.cursor(pcur));

  return {};
}

error::Result<void>
Document::forward()
{
  // x & y should inside the buffer
  assert(_cpos.y < _buffer.size());
  assert(_cpos.x <= _buffer.at(_cpos.y).size());

  auto pcur = UNWRAPERR(_term.cursor());

  auto& line = _buffer.at(_cpos.y);

  if (_cpos.x < line.size() && line.at(_cpos.x) != '\n') {
    _cpos.x++;
  } else if (_cpos.y < _buffer.size() - 1) {
    _cpos.y++;
    pcur.y++;

    _cpos.x = 0;
  }

  pcur.x = _cpos.x;

  RETERR(_term.cursor(pcur));

  return {};
}

error::Result<void>
Document::enter()
{
  RETERR(_term.enable());
  RETERR(_term.puts(_prompt));

  _buffer.clear();
  _buffer.push_back(_prompt);
  _cpos = prompt_cursor();

  return {};
}

error::Result<void>
Document::leave()
{
  RETERR(_term.putc('\n'));
  RETERR(_term.disable());

  return {};
}

error::Result<void>
Document::resize()
{
  auto precur = UNWRAPERR(_term.cursor());
  if (precur.y >= _cpos.y) {
    precur.y -= _cpos.y;
  } else {
    precur.y = 0;
  }

  precur.x = 0;

  RETERR(_term.cursor(precur));

  _wsize = UNWRAPERR(_term.winsize());
  rebuild_buffer(0);

  _cpos.y = _buffer.size() - 1;
  _cpos.x = _buffer.back().size();

  return {};
}

error::Result<Document>
Document::create(std::FILE* out, std::FILE* in, std::string prompt)
{
  auto term = Terminal::create(out, in);
  if (!term.has_value()) {
    return std::unexpected(term.error());
  }

  return Document{ std::move(term.value()), std::move(prompt) };
}

void
Document::rebuild_buffer(size_t start)
{
  assert(start < _buffer.size() &&
         start <= std::numeric_limits<ssize_t>::max());

  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto str =
    _buffer | views::drop(start) | views::join | ranges::to<std::string>();

  _buffer.erase(_buffer.begin() + static_cast<ssize_t>(start), _buffer.end());

  std::string line{};
  for (auto c : str) {
    line.push_back(c);
    if (line.size() >= _wsize.x) {
      _buffer.push_back(line);
      line.clear();
      continue;
    }

    if (c == '\n') {
      _buffer.push_back(line);
      line.clear();
    }
  }

  _buffer.push_back(line);
}

std::pair<bool, bool>
Document::fixup_cursor(TermCursor& cursor) const
{
  bool x_of = false;
  bool y_of = false;

  if (cursor.x >= _wsize.x) {
    cursor.y += cursor.x / _wsize.x;
    cursor.x %= _wsize.x;
    x_of = true;
  }

  if (cursor.y >= _wsize.y) {
    cursor.y = _wsize.y - 1;
    cursor.x = 0;
    y_of = true;
  }

  return { x_of, y_of };
}

TermCursor
Document::prompt_cursor() const
{
  size_t x = 0;
  size_t y = 0;

  for (auto c : _prompt) {
    if (c == '\n') {
      y++;
      x = 0;
    } else {
      x++;
    }

    if (x >= _wsize.x) {
      y++;
      x = 0;
    }
  }

  return { .x = x, .y = y };
}

// error::Result<void>
// Document::resize(const TermCursor& size)
// {
//   auto vcur = get_vcursor_from_pos(_cursor);

//   RETERR(_term.up(vcur.y, true));
//   RETERR(_term.forward(_prompt.size()));

//   RETERR(_term.puts(_buffer));
//   auto _ = _term.cleanline(CleanType::TOEND);

//   _wsize = size;

//   return {};
// }
}
