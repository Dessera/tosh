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
  return utils::getenv("PATH");
}

}
