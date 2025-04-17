#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/cursor.hpp"

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

error::Result<void>
Document::insert(char c)
{
  assert(_cursor <= _buffer.size());

  _buffer.insert(_cursor, 1, c);
  _cursor++;

  RETERR(_term.putc(c));
  auto fcur = UNWRAPERR(_term.cursor());
  auto vcursor = get_vcursor_from_pos(_cursor);

  ANSIHideGuard hide(_term);

  RETERR(_term.up(vcursor.y() - 1, true));
  RETERR(_term.forward(_prompt.size()));

  RETERR(_term.puts(_buffer));
  RETERR(_term.cursor(fcur));

  return {};
}

error::Result<void>
Document::backward(std::size_t n)
{
  assert(_cursor <= _buffer.size());

  auto vcursor = get_vcursor_from_pos(_cursor);

  ANSIHideGuard hide(_term);

  RETERR(_term.up(vcursor.y() - 1, true));
  RETERR(_term.forward(_prompt.size()));

  _cursor = _cursor < n ? 0 : _cursor - n;
  RETERR(_term.puts(_buffer.substr(0, _cursor)));

  return {};
}

TermCursor
Document::get_vcursor_from_pos(std::size_t pos)
{
  namespace views = std::ranges::views;
  namespace ranges = std::ranges;

  auto res =
    _buffer | views::take(pos) | views::split('\n') |
    views::transform([&](const auto& substr) -> std::pair<size_t, size_t> {
      return { substr.size() / _wsize.x(), substr.size() % _wsize.x() };
    }) |
    ranges::to<std::vector<std::pair<size_t, size_t>>>();

  auto y = std::transform_reduce(
    res.begin(), res.end(), res.size(), std::plus(), [](const auto& p) {
      return p.first;
    });

  auto x = res.back().second;
  if (y > 1) {
    x += _prompt.size();
  }

  return { x, y };
}

}
