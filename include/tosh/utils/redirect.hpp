#pragma once

#include "magic_enum/magic_enum.hpp"
#include "tosh/common.hpp"
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

namespace tosh::parser {

class ParseQuery;

}

namespace tosh::utils {

enum class TOSH_EXPORT RedirectType : uint8_t
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

/**
 * @brief Convert index to redirect type
 *
 * @param index Redirect type index
 * @return constexpr RedirectType Redirect type
 */
constexpr RedirectType
redirect_index_to_type(size_t index)
{
  return magic_enum::enum_cast<RedirectType>(index).value_or(
    RedirectType::UNKNOWN);
}

/**
 * @brief Convert a redirect type to a string
 *
 * @param type Redirect type
 * @return constexpr std::string_view String representation of the redirect type
 */
constexpr std::string_view
redirect_type_to_str(RedirectType type)
{
  return REDIRECT_OP_STRS.at(static_cast<size_t>(type));
}

/**
 * @brief Convert a string to a redirect type
 *
 * @param str String to convert
 * @return constexpr RedirectType Redirect type
 */
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

  /**
   * @brief Get the source
   *
   * @return constexpr auto Source file descriptor
   */
  [[nodiscard]] constexpr auto src() const { return _src; }

  /**
   * @brief Get the destination
   *
   * @return constexpr auto& Destination string
   */
  [[nodiscard]] constexpr auto& dst() const { return _dst; }

  /**
   * @brief Get the redirect type
   *
   * @return constexpr auto Redirect type
   */
  [[nodiscard]] constexpr auto type() const { return _type; }
};

class RedirectOperation
{
private:
  Redirect _redirect;

public:
  RedirectOperation(Redirect redirect);
  virtual ~RedirectOperation() = default;

  /**
   * @brief Apply the redirect
   *
   * @return error::Result<void> Returns an error if the redirect could not be
   * applied
   */
  virtual error::Result<void> apply();

  /**
   * @brief Restore the original file descriptors
   *
   * @return error::Result<void> Returns an error if the file descriptors could
   * not be restored
   */
  virtual error::Result<void> restore();

  /**
   * @brief Get the redirect
   *
   * @return constexpr auto& Redirect
   */
  [[nodiscard]] constexpr auto& data() const { return _redirect; }
};

class RedirectFactory
{
public:
  /**
   * @brief Create a redirect operation
   *
   * @param redirect Redirect to create the operation for
   * @return std::shared_ptr<RedirectOperation> Redirect operation
   */
  static std::shared_ptr<RedirectOperation> create(const Redirect& redirect);
};

class RedirectBasicOperation : public RedirectOperation
{
private:
  bool _append;
  bool _in;

  int _fd{ -1 };

public:
  RedirectBasicOperation(Redirect redirect,
                         bool append = false,
                         bool in = false);

  error::Result<void> apply() override;
  error::Result<void> restore() override;
};

class RedirectMergeOperation : public RedirectOperation
{
private:
  int _fd{ -1 };

public:
  RedirectMergeOperation(Redirect redirect);

  error::Result<void> apply() override;
  error::Result<void> restore() override;
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
