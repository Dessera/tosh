#pragma once

#include "tosh/error.hpp"
#include <concepts>
#include <expected>
#include <type_traits>

namespace tosh::utils {

template<typename T>
concept HasDefaultCreator = requires {
  { T::create() } -> std::same_as<error::Result<T>>;
};

template<typename T>
class Singleton
{
public:
  static T& instance()
  {
    static T _instance;
    return _instance;
  }
};

template<typename T>
  requires(!std::is_default_constructible_v<T>) && HasDefaultCreator<T>
class Singleton<T>
{
public:
  static error::Result<T*> instance()
  {
    static auto _instance = T::create();
    if (!_instance.has_value()) {
      return std::unexpected(_instance.error());
    }
    return &_instance.value();
  }
};

}
