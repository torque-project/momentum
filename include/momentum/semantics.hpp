#pragma once

#include <memory>

namespace imu {

  /**
   * @ns semantics
   * @brief Defines various semantic helpers like pointer types
   *
   */
  namespace semantics {

    template<typename T>
    struct real_type {
      typedef T type;
    };

    template<typename T>
    struct real_type<T*> {
      typedef T type;
    };

    template<typename T>
    struct real_type<std::shared_ptr<T>> {
      typedef T type;
    };

  }
}
