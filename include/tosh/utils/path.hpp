#pragma once

#include <string>
#include <string_view>

namespace tosh::utils {

std::string
get_path_env(std::string_view path);

}