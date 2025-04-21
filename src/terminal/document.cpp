#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
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

error::Result<std::string>
Document::gets()
{
  return _term.gets();
}

error::Result<void>
Document::insert(char c)
{
  // assert(_cursor <= _buffer.size());

  ANSIHideGuard hide{ _term };

  // auto precur = UNWRAPERR(_term.cursor());
  // auto poscur = precur;

  // _buffer.insert(_cursor, 1, c);
  // _cursor++;

  // RETERR(_term.putc(c));

  // auto fcur = UNWRAPERR(_term.cursor());
  // cursor_fixup(fcur);

  // RETERR(_term.cursor(fcur));

  // auto vcur = get_vcursor_from_pos(_cursor);

  // RETERR(_term.up(vcur.y, true));
  // RETERR(_term.puts(_prompt));

  // RETERR(_term.clean(CleanType::TOEND));
  // RETERR(_term.puts(_buffer));

  // RETERR(_term.cursor(fcur));

  // return {};
}

error::Result<void>
Document::backward(std::size_t n)
{
  // assert(_cursor <= _buffer.size());

  // auto vcur = calc_vcursor(0, _cursor);

  // ANSIHideGuard hide(_term);

  // RETERR(_term.up(vcur.y, true));
  // RETERR(_term.puts(_prompt));

  // _cursor = _cursor < n ? 0 : _cursor - n;
  // RETERR(_term.puts(_buffer.substr(0, _cursor)));

  // auto pcur = UNWRAPERR(_term.cursor());
  // cursor_fixup(pcur);

  // RETERR(_term.cursor(pcur));

  // return {};
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

  auto _ = _term.puts(_prompt);

  _buffer.clear();
  _cpos = 0;

  return {};
}

error::Result<void>
Document::leave()
{
  auto _ = _term.putc('\n');

  RETERR(_term.disable());

  return {};
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
