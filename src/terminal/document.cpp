#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/event/parser.hpp"
#include "tosh/terminal/terminal.hpp"

#include <cassert>
#include <cstddef>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>

namespace tosh::terminal {

Document::Document(std::FILE* out, std::FILE* in, std::string prompt)
  : _prompt(std::move(prompt))
  , _term(out, in)
  , _wsize(_term.unsafe_winsize())
{
}

Document::~Document() {}

error::Result<Event>
Document::get_op()
{
  return _term.get_op();
}

error::Result<void>
Document::insert(char c)
{
  assert(_cpos.y < _buffer.size());
  assert(_cpos.x <= _buffer.at(_cpos.y).size());
  assert((c >= ' ' && c <= '~') || c == '\n');

  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto precur = UNWRAPERR(_term.cursor());

  _buffer.at(_cpos.y).insert(_cpos.x, 1, c);
  rebuild_buffer(_cpos.y);

  // auto& line = _buffer.at(_cpos.y);

  auto bufline_sz = _buffer.size() - _cpos.y;

  RETERR(_term.putc('\r'));
  RETERR(_term.clean(CleanType::TOEND));

  auto str =
    _buffer | views::drop(_cpos.y) | views::join | ranges::to<std::string>();
  RETERR(_term.puts(str));

  auto endcur = UNWRAPERR(_term.cursor());
  fixup_cursor(endcur);

  auto ioline_sz = endcur.y - precur.y + 1;
  auto loffs = bufline_sz - ioline_sz;

  _cpos.x++;
  precur.x++;
  if (_cpos.x >= _wsize.x || _buffer.at(_cpos.y).at(_cpos.x - 1) == '\n') {
    _cpos.x = 0;
    _cpos.y++;

    if (precur.x == endcur.x && precur.y == endcur.y) {
      _term.putc('\n');
    }
    precur.x = 0;
    precur.y++;
  }

  if (_cpos.y >= _buffer.size()) {
    _buffer.emplace_back();
  }

  precur.y -= loffs;

  RETERR(_term.cursor(precur));

  return {};
}

error::Result<void>
Document::backward()
{
  // assert(_cursor <= _buffer.size());
  auto pcur = UNWRAPERR(_term.cursor());
  if (_cpos.x == 0) {
    if (_cpos.y != 0) {
      _cpos.y--;
      pcur.y--;

      _cpos.x = _buffer.at(_cpos.y).size();
      // if this is a '\n', then we need to move one more back
      if (_buffer.at(_cpos.y).at(_cpos.x - 1) == '\n') {
        _cpos.x--;
      }
    }
  } else {
    _cpos.x--;
  }

  pcur.x = _cpos.x;

  RETERR(_term.cursor(pcur));

  return {};
}

error::Result<void>
Document::forward(std::size_t n)
{
  // assert(_cursor <= _buffer.size());

  // auto vcur = calc_vcursor(0, _cursor);

  // ANSIHideGuard hide(_term);

  // RETERR(_term.up(vcur.y, true));
  // RETERR(_term.puts(_prompt));

  // _cursor = _cursor + n > _buffer.size() ? _buffer.size() : _cursor + n;
  // RETERR(_term.puts(_buffer.substr(0, _cursor)));

  // auto pcur = UNWRAPERR(_term.cursor());
  // cursor_fixup(pcur);

  // RETERR(_term.cursor(pcur));

  // return {};
}

error::Result<void>
Document::enter()
{
  RETERR(_term.enable());

  // auto _ = _term.puts(_prompt);

  _buffer.clear();
  _buffer.emplace_back();

  _cpos.x = 0;
  _cpos.y = 0;

  return {};
}

error::Result<void>
Document::leave()
{
  auto _ = _term.putc('\n');

  RETERR(_term.disable());

  return {};
}

void
Document::rebuild_buffer(size_t start)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto str =
    _buffer | views::drop(start) | views::join | ranges::to<std::string>();

  // erase old buffer
  _buffer.erase(_buffer.begin() + start, _buffer.end());

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

bool
Document::fixup_cursor(TermCursor& cursor) const
{
  if (cursor.x >= _wsize.x) {
    cursor.y += cursor.x / _wsize.x;
    cursor.x %= _wsize.x;
  }

  if (cursor.y >= _wsize.y) {
    cursor.y = _wsize.y - 1;
    cursor.x = 0;

    return true;
  }

  return false;
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
