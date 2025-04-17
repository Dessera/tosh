#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/cursor.hpp"

#include <cassert>
#include <cstddef>
#include <numeric>
#include <ranges>
#include <string_view>
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
  if (fcur.x() >= _wsize.x()) {
    fcur.x() = 0;
    fcur.y()++;
  }

  auto vcur = get_vcursor_from_pos(_cursor);

  // ANSIHideGuard hide(_term);

  RETERR(_term.up(vcur.y(), true));
  RETERR(_term.forward(_prompt.size()));

  RETERR(_term.clean(CleanType::TOEND));
  RETERR(_term.puts(_buffer));

  RETERR(_term.cursor(fcur));

  return {};
}

error::Result<void>
Document::backward(std::size_t n)
{
  assert(_cursor <= _buffer.size());

  auto vcur = get_vcursor_from_pos(_cursor);

  // ANSIHideGuard hide(_term);

  RETERR(_term.up(vcur.y(), true));
  RETERR(_term.forward(_prompt.size()));

  _cursor = _cursor < n ? 0 : _cursor - n;
  RETERR(_term.puts(_buffer.substr(0, _cursor)));

  return {};
}

error::Result<void>
Document::forward(std::size_t n)
{
  assert(_cursor <= _buffer.size());

  auto vcur = get_vcursor_from_pos(_cursor);

  ANSIHideGuard hide(_term);

  RETERR(_term.up(vcur.y(), true));
  RETERR(_term.forward(_prompt.size()));

  _cursor = _cursor > _buffer.size() - n ? _buffer.size() : _cursor + n;
  RETERR(_term.puts(_buffer.substr(0, _cursor)));

  return {};
}

error::Result<void>
Document::enter()
{
  RETERR(_term.enable());

  auto _ = _term.puts(_prompt);

  _buffer.clear();
  _cursor = 0;

  return {};
}

error::Result<void>
Document::leave()
{
  auto _ = _term.putc('\n');

  RETERR(_term.disable());

  return {};
}

error::Result<void>
Document::resize(const TermCursor& size)
{
  auto vcur = get_vcursor_from_pos(_cursor);

  RETERR(_term.up(vcur.y(), true));
  RETERR(_term.forward(_prompt.size()));

  RETERR(_term.puts(_buffer));
  auto _ = _term.cleanline(CleanType::TOEND);

  _wsize = size;

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

  assert(!res.empty());

  // prompt fixup
  auto& front = res.front();
  front.second += _prompt.size() - 1;
  if (front.second >= _wsize.x()) {
    front.first += front.second / _wsize.x();
    front.second = front.second % _wsize.x();
  }

  auto y = std::transform_reduce(res.begin(),
                                 res.end(),
                                 res.size(),
                                 std::plus(),
                                 [](const auto& p) { return p.first; }) -
           1;

  auto x = res.back().second;

  return { x, y };
}

}
