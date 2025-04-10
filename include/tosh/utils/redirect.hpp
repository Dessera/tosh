#pragma once

#include "magic_enum/magic_enum.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <map>
#include <string>
#include <string_view>

namespace tosh::utils {

enum class RedirectException : uint8_t
{
  EFILE_NOT_FOUND,
  EFILE_PERMISSION_DENIED,
  EINVALID_DESTINATION,
  EINVALID_SOURCE,
  EINVALID_REDIRECT,
  EUNKOWN
};

enum class RedirectOpType : uint8_t
{
  OUT,       // >
  APPEND,    // >>
  IN,        // <
  OUT_MERGE, // >&
  IN_MERGE,  // <&
  HEREDOC,   // << (not implemented)

  UNKNOWN
};

constexpr inline std::array<std::string_view, 6> REDIRECT_OP_STRS = {
  ">", ">>", "<", ">&", "<&", "<<"
};

constexpr RedirectOpType
redirect_index_to_type(size_t index)
{
  return magic_enum::enum_cast<RedirectOpType>(index).value_or(
    RedirectOpType::UNKNOWN);
}

constexpr std::string_view
redirect_type_to_str(RedirectOpType type)
{
  return REDIRECT_OP_STRS.at(static_cast<size_t>(type));
}

constexpr RedirectOpType
redirect_str_to_type(std::string_view str)
{
  return redirect_index_to_type(std::distance(
    REDIRECT_OP_STRS.begin(), std::ranges::find(REDIRECT_OP_STRS, str)));
}

class RedirectOp
{
private:
  int _src;
  std::string _dst;
  RedirectOpType _type;

  const inline static std::map<int, FILE*> IO_MAP = { { 0, stdin },
                                                      { 1, stdout },
                                                      { 2, stderr } };

public:
  RedirectOp(int src, std::string dst, RedirectOpType type);

  [[nodiscard]] constexpr auto src() const { return _src; }
  [[nodiscard]] constexpr std::string_view dst() const { return _dst; }
  [[nodiscard]] constexpr auto type() const { return _type; }

  std::expected<void, RedirectException> apply();

private:
  std::expected<void, RedirectException> file_reopen(const std::string& mode);
  std::expected<void, RedirectException> file_merge();
};

}

template<typename CharT>
struct std::formatter<tosh::utils::RedirectOp, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  constexpr auto format(const tosh::utils::RedirectOp& op,
                        std::format_context& ctx) const
  {
    return std::format_to(ctx.out(),
                          "{} -> {} {}",
                          op.src(),
                          op.dst(),
                          magic_enum::enum_name(op.type()));
  }
};