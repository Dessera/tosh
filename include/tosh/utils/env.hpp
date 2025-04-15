#pragma once

#include <string>

namespace tosh::utils {

/**
 * @brief Get environment variable
 *
 * @param name Name of the environment variable
 * @return constexpr std::string Environment variable value
 */
constexpr std::string
getenv(const std::string& name)
{
  char* env = std::getenv(name.data());
  return env ? env : "";
}

}
