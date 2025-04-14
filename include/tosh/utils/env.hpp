#pragma once

#include <string>

namespace tosh::utils {

constexpr std::string
getenv(const std::string& name)
{
  char* env = std::getenv(name.data());
  return env ? env : "";
}

}
