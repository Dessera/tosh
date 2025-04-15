#pragma once

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
std::string
get_path_env(std::string_view path);

}
