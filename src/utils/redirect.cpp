
#include "tosh/utils/redirect.hpp"
#include <cerrno>
#include <cstdio>
#include <exception>
#include <expected>
#include <string>
#include <unistd.h>
namespace tosh::utils {

RedirectOp::RedirectOp(int src, std::string dst, RedirectOpType type)
  : _src(src)
  , _dst(std::move(dst))
  , _type(type)
{
}

std::expected<void, RedirectException>
RedirectOp::apply()
{
  switch (_type) {
    case RedirectOpType::OUT:
      return file_reopen("w");
    case RedirectOpType::APPEND:
      return file_reopen("a");
    case RedirectOpType::IN:
      return file_reopen("r");
    case RedirectOpType::OUT_MERGE:
    case RedirectOpType::IN_MERGE:
      return file_merge();
    default:
      return std::unexpected(RedirectException::EINVALID_REDIRECT);
  }
}

std::expected<void, RedirectException>
RedirectOp::file_reopen(const std::string& mode)
{
  if (!IO_MAP.contains(_src)) {
    return std::unexpected(RedirectException::EINVALID_SOURCE);
  }
  auto* srcfile = IO_MAP.at(_src);

  // NOLINTNEXTLINE
  auto* file = std::freopen(_dst.c_str(), mode.data(), srcfile);

  if (file == nullptr) {
    switch (errno) {
      case EACCES:
        return std::unexpected(RedirectException::EFILE_PERMISSION_DENIED);
      default:
        return std::unexpected(RedirectException::EFILE_NOT_FOUND);
    }
  }

  return {};
}

std::expected<void, RedirectException>
RedirectOp::file_merge()
{
  int dstfd = 0;
  try {
    dstfd = std::stoi(_dst);
  } catch (const std::exception&) {
    return std::unexpected(RedirectException::EINVALID_DESTINATION);
  }

  if (!IO_MAP.contains(_src)) {
    return std::unexpected(RedirectException::EINVALID_SOURCE);
  }

  if (!IO_MAP.contains(dstfd)) {
    return std::unexpected(RedirectException::EINVALID_DESTINATION);
  }

  if (dup2(_src, dstfd) == -1) {
    return std::unexpected(RedirectException::EUNKOWN);
  }

  return {};
}
}
