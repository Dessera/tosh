#include "tosh/terminal/document.hpp"
#include "tosh/error.hpp"
#include "tosh/terminal/ansi.hpp"
#include "tosh/terminal/cursor.hpp"
#include <cstddef>
#include <numeric>
#include <ranges>
#include <utility>
#include <vector>

namespace tosh::terminal {

Document::Document(std::FILE* out, std::FILE* in, std::string prompt)
  : _prompt(std::move(prompt))
  , _term(out, in)
  , _wsize(unsafe_get_wsize())
{
  _term.puts(_prompt);
}

error::Result<void>
Document::insert(char c)
{
  // auto vcursor = get_vcursor_from_pos(_cursor);
  // auto pcursor = _term.cursor();

  _buffer.insert(_cursor, 1, c);
  _cursor++;

  _term.putc(c);
  auto fcur = _term.cursor().value();
  auto vcursor = get_vcursor_from_pos(_cursor);

  ANSIHideGuard hide(_term);

  RETERR(_term.up(vcursor.y() - 1, true));
  RETERR(_term.forward(_prompt.size()));

  _term.puts(_buffer);
  _term.cursor(fcur);

  return {};
}

error::Result<void>
Document::backward(std::size_t n)
{
  auto vcursor = get_vcursor_from_pos(_cursor);

  ANSIHideGuard hide(_term);

  RETERR(_term.up(vcursor.y() - 1, true));
  RETERR(_term.forward(_prompt.size()));

  _cursor = _cursor < n ? 0 : _cursor - n;
  _term.puts(_buffer.substr(0, _cursor));

  return {};
}

TermCursor
Document::unsafe_get_wsize()
{
  auto res = _term.size();

  if (!res.has_value()) {
    throw std::runtime_error("Failed to get terminal size");
  }

  return res.value();
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
