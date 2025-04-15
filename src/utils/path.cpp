#include "tosh/utils/path.hpp"
#include "tosh/utils/env.hpp"

#include <filesystem>
#include <string>

namespace tosh::utils {

std::string
get_path_env(std::string_view path)
{
  namespace fs = std::filesystem;

  if (path.starts_with("./") || path.starts_with("../")) {
    return fs::current_path().string();
  }
  if (path.starts_with("~/")) {
    return utils::getenv("HOME");
  }
  return utils::getenv("PATH");
}

std::filesystem::path
remove_home_prefix(const std::filesystem::path& path)
{
  auto pstr = path.string();
  if (pstr.starts_with("~/")) {
    return pstr.substr(2);
  }

  return path;
}

}
