#pragma once

#include "magic_enum/magic_enum.hpp"
#include "tosh/error.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <memory>
#include <string>
#include <string_view>

namespace tosh::utils {

enum class RedirectType : uint8_t
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

constexpr RedirectType
redirect_index_to_type(size_t index)
{
  return magic_enum::enum_cast<RedirectType>(index).value_or(
    RedirectType::UNKNOWN);
}

constexpr std::string_view
redirect_type_to_str(RedirectType type)
{
  return REDIRECT_OP_STRS.at(static_cast<size_t>(type));
}

constexpr RedirectType
redirect_str_to_type(std::string_view str)
{
  return redirect_index_to_type(std::distance(
    REDIRECT_OP_STRS.begin(), std::ranges::find(REDIRECT_OP_STRS, str)));
}

class Redirect
{
private:
  int _src;
  std::string _dst;
  RedirectType _type;

public:
  Redirect(int src, std::string dst, RedirectType type);

  [[nodiscard]] constexpr auto src() const { return _src; }
  [[nodiscard]] constexpr auto& dst() const { return _dst; }
  [[nodiscard]] constexpr auto type() const { return _type; }
};

class RedirectOperation
{
private:
  Redirect _redirect;

public:
  RedirectOperation(Redirect redirect);
  virtual ~RedirectOperation() = default;

  virtual error::Result<void> apply();

  [[nodiscard]] constexpr auto& data() const { return _redirect; }
};

class RedirectFactory
{
public:
  static std::shared_ptr<RedirectOperation> create(const Redirect& redirect);
};

class RedirectBasicOperation : public RedirectOperation
{
private:
  bool _append;
  bool _in;

public:
  RedirectBasicOperation(Redirect redirect,
                         bool append = false,
                         bool in = false);

  error::Result<void> apply() override;
};

class RedirectMergeOperation : public RedirectOperation
{
public:
  RedirectMergeOperation(Redirect redirect);

  error::Result<void> apply() override;
};

}

template<typename CharT>
struct std::formatter<tosh::utils::Redirect, CharT>
{
  constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

  constexpr auto format(const tosh::utils::Redirect& op,
                        std::format_context& ctx) const
  {
    return std::format_to(ctx.out(),
                          "{} -> {} {}",
                          op.src(),
                          op.dst(),
                          magic_enum::enum_name(op.type()));
  }
};