
#include "tosh/utils/redirect.hpp"
#include "tosh/error.hpp"

#include <cerrno>
#include <cstdio>
#include <expected>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <unistd.h>

namespace {

std::optional<int>
str_to_fd(const std::string& str)
try {
  return std::stoi(str);
} catch (...) {
  return std::nullopt;
}

}

namespace tosh::utils {

Redirect::Redirect(int src, std::string dst, RedirectType type)
  : _src(src)
  , _dst(std::move(dst))
  , _type(type)
{
}

RedirectOperation::RedirectOperation(Redirect redirect)
  : _redirect(std::move(redirect))
{
}

error::Result<void>
RedirectOperation::apply()
{
  return {};
}

error::Result<void>
RedirectOperation::restore()
{
  return {};
}

RedirectBasicOperation::RedirectBasicOperation(Redirect redirect,
                                               bool append,
                                               bool in)
  : RedirectOperation(std::move(redirect))
  , _append(append)
  , _in(in)
{
}

error::Result<void>
RedirectBasicOperation::apply()
{
  // NOLINTNEXTLINE
  auto mode = _in ? "r" : (_append ? "a" : "w");

  auto* f = fdopen(data().src(), mode);
  if (f == nullptr) {
    return error::err(error::ErrorCode::REDIRECT_INVALID_DEST);
  }

  // dup original file descriptor
  _fd = dup(data().src());
  if (_fd == -1) {
    return error::err(error::ErrorCode::UNKNOWN);
  }

  // NOLINTNEXTLINE
  if (std::freopen(data().dst().c_str(), mode, f) == nullptr) {
    // close dupped fd
    close(_fd);
    _fd = -1;

    return error::err(error::ErrorCode::REDIRECT_INVALID_DEST);
  }

  return {};
}

error::Result<void>
RedirectBasicOperation::restore()
{
  if (_fd != -1 && dup2(_fd, data().src()) == -1) {
    return error::err(error::ErrorCode::UNKNOWN);
  }

  return {};
}

RedirectMergeOperation::RedirectMergeOperation(Redirect redirect)
  : RedirectOperation(std::move(redirect))
{
}

error::Result<void>
RedirectMergeOperation::apply()
{
  auto fd = str_to_fd(data().dst());
  if (!fd.has_value()) {
    return error::err(error::ErrorCode::REDIRECT_INVALID_DEST,
                      "Dest must be a valid fd");
  }

  _fd = dup(data().src());
  if (_fd == -1) {
    return error::err(error::ErrorCode::UNKNOWN);
  }

  if (dup2(fd.value(), data().src()) == -1) {
    // close dupped fd
    close(_fd);
    _fd = -1;

    return error::err(error::ErrorCode::UNKNOWN);
  }

  return {};
}

error::Result<void>
RedirectMergeOperation::restore()
{
  if (_fd != -1 && dup2(_fd, data().src()) == -1) {
    return error::err(error::ErrorCode::UNKNOWN);
  }

  return {};
}

std::shared_ptr<RedirectOperation>
RedirectFactory::create(const Redirect& redirect)
{
  if (redirect.type() == RedirectType::OUT) {
    return std::make_shared<RedirectBasicOperation>(redirect, false, false);
  }

  if (redirect.type() == RedirectType::APPEND) {
    return std::make_shared<RedirectBasicOperation>(redirect, true, false);
  }

  if (redirect.type() == RedirectType::IN) {
    return std::make_shared<RedirectBasicOperation>(redirect, false, true);
  }

  if (redirect.type() == RedirectType::OUT_MERGE ||
      redirect.type() == RedirectType::IN_MERGE) {
    return std::make_shared<RedirectMergeOperation>(redirect);
  }

  return std::make_shared<RedirectOperation>(redirect);
}
}
