#pragma once

#include <type_traits>

namespace tosh::utils {

template<typename T>
  requires std::is_default_constructible_v<T>
class Singleton
{
public:
  static T& instance()
  {
    static T instance;
    return instance;
  }
};

}