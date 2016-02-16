#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

namespace imu {

  template<typename T, typename... Args>
  inline auto nu(Args... args)
    -> decltype(typename T::p()) {
    typedef typename T::template semantics<T> sem;
    return sem::allocate(args...);
  }

  /**
   * An empty type tag. This is used to indicate that no
   * mixins are used for a type
   *
   */
  struct no_mixin {

    template<typename T>
    struct semantics {

      typedef std::shared_ptr<T> p;

      template<typename... TS>
      static inline p allocate(TS... args) {
        return std::make_shared<T>(args...);
      }
    };
  };

  /**
   * Type analysis utilities that are not in the stl type traits
   * library
   *
   */
  namespace type_traits {

    /**
     * This provides signature analysis for a given lambda function.
     *
     */
    template<typename T>
    struct lambda_traits :
      public lambda_traits<decltype(&T::operator())>
    {};

    template <typename T, typename R, typename... Args>
    struct lambda_traits<R(T::*)(Args...) const> {

      typedef R result_type;

      template <size_t i>
      struct arg {
        typedef std::tuple<Args...> args;
        typedef typename std::tuple_element<i,args>::type type;
        typedef typename std::remove_reference<type>::type val;
        typedef typename std::decay<type>::type decayed;
        // typedef typename noref::element_type val;
      };
    };
  }
}
