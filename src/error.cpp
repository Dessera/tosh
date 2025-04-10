#include <iostream>
#include <print>
#include <utility>

#include "magic_enum/magic_enum.hpp"
#include "tosh/error.hpp"
namespace tosh::error {

Error::Error(ErrorCode code, std::string message)
  : _code(code)
  , _msg(std::move(message))
{
}

void
Error::log() const
{
  std::println(std::cerr,
               "Exception occurred: {} [{}]",
               _msg,
               magic_enum::enum_name(_code));
}

}
