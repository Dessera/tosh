#pragma once

#include "tosh/common.hpp"
#include <cstdint>
#include <cstring>
#include <exception>
#include <expected>
#include <format>
#include <string>

// macro to return when an error occurs
#define RETERR(expr)                                                           \
  if (auto res = (expr); !res.has_value()) {                                   \
    return std::unexpected(res.error());                                       \
  }

#define UNWRAPERR(expr)                                                        \
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

enum class TOSH_EXPORT ErrorCode : uint8_t
{
  BUILTIN_EXEC_FAILED,  // exec failed
  BUILTIN_FORK_FAILED,  // fork failed
  BUILTIN_INVALID_ARGS, // invalid arguments
  BUILTIN_NOT_FOUND,    // builtin not found

  REDIRECT_INVALID_SRC,
  REDIRECT_INVALID_DEST,
  REDIRECT_INVALID_TYPE,

  PARSER_SYNTAX_ERROR,

  ENV_INVALID,

  UNEXPECTED_IO_STATUS,
  EVENT_LOOP_FAILED,

  UNKNOWN
};

class TOSH_EXPORT Error : public std::exception
{
private:
  ErrorCode _code;
  std::string _msg;

public:
  Error(ErrorCode code, std::string message) noexcept;

  [[nodiscard]] constexpr ErrorCode code() const noexcept { return _code; }
  [[nodiscard]] constexpr const std::string& message() const noexcept
  {
    return _msg;
  }

  void log() const;
  [[nodiscard]] const char* what() const noexcept override
  {
    return _msg.c_str();
  }
};

template<typename T>
using Result = std::expected<T, Error>;

constexpr auto
err(ErrorCode code, const std::string& message) noexcept
{
  return std::unexpected(Error(code, message));
}

constexpr auto
raw_err(ErrorCode code) noexcept
{
  auto errno_copy = errno;
  return Error(code,
               errno_copy == 0 ? "Unknown error" : std::strerror(errno_copy));
}

constexpr auto
err(ErrorCode code) noexcept
{
  return std::unexpected(raw_err(code));
}

template<typename... Args>
constexpr auto
err(ErrorCode code, std::format_string<Args...> fmt, Args&&... args) noexcept
{
  return std::unexpected(
    Error(code, std::format(fmt, std::forward<Args>(args)...)));
}

}
