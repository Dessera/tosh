#pragma once

#include "tosh/common.hpp"

#include <cstddef>

namespace tosh::terminal {

class TOSH_EXPORT TermCursor
{
private:
  std::size_t _x{ 0 };
  std::size_t _y{ 0 };

public:
  TermCursor() = default;
  TermCursor(std::size_t x, std::size_t y);

  [[nodiscard]] constexpr auto x() const noexcept { return _x; }
  [[nodiscard]] constexpr auto& x() noexcept { return _x; }
  [[nodiscard]] constexpr auto y() const noexcept { return _y; }
  [[nodiscard]] constexpr auto& y() noexcept { return _y; }
};

}