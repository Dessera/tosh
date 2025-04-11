#pragma once

#include <cstdint>
#include <cstring>
#include <expected>
#include <string>

// macro to return when an error occurs
#define RETERR(expr)                                                           \
  ({                                                                           \
    auto _ret = (expr);                                                        \
    if (!_ret.has_value()) {                                                   \
      return std::unexpected(_ret.error());                                    \
    }                                                                          \
    _ret.value();                                                              \
  })

#define LOGERR(expr)                                                           \
  {                                                                            \
    auto _ret = (expr);                                                        \
    if (!_ret.has_value()) {                                                   \
      _ret.error().log();                                                      \
    }                                                                          \
  }

#define LOGERR_EXIT(expr)                                                      \
  {                                                                            \
    auto _ret = (expr);                                                        \
    if (!_ret.has_value()) {                                                   \
      _ret.error().log();                                                      \
      std::exit(EXIT_FAILURE);                                                 \
    }                                                                          \
  }

namespace tosh::error {

enum class ErrorCode : uint8_t
{
  BUILTIN_EXEC_FAILED,
  BUILTIN_FORK_FAILED,
  BUILTIN_INVALID_ARGS,
  BUILTIN_NOT_FOUND,

  REDIRECT_INVALID_SRC,
  REDIRECT_INVALID_DEST,
  REDIRECT_INVALID_TYPE,

  PARSER_SYNTAX_ERROR,

  UNKNOWN
};

class Error
{
private:
  ErrorCode _code;
  std::string _msg;

public:
  Error(ErrorCode code, std::string message);

  [[nodiscard]] constexpr ErrorCode code() const noexcept { return _code; }
  [[nodiscard]] constexpr const std::string& message() const noexcept
  {
    return _msg;
  }

  void log() const;
};

template<typename T>
using Result = std::expected<T, Error>;

constexpr auto
err(ErrorCode code, const std::string& message)
{
  return std::unexpected(Error(code, message));
}

constexpr auto
err(ErrorCode code)
{
  auto errno_copy = errno;
  return std::unexpected(
    Error(code, errno_copy == 0 ? "Unknown error" : std::strerror(errno_copy)));
}

}