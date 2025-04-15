#pragma once

#include "tosh/common.hpp"
#include <filesystem>
#include <string>
#include <string_view>

namespace tosh::utils {

/**
 * @brief Get the PATH environment if the path is not relative, otherwise return
 * the current working directory
 *
 * @param path Path to check
 * @return std::string Path result
 */
TOSH_EXPORT std::string
get_path_env(std::string_view path);

TOSH_EXPORT std::filesystem::path
remove_home_prefix(const std::filesystem::path& path);

}
