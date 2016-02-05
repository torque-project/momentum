#pragma once

#include <experimental/optional>

/**
 * Additional oeprators to compare optional reference types
 *
 */
template<typename T>
inline bool operator== (
  const std::experimental::optional<std::reference_wrapper<const T>>& l,
  const typename std::decay<T>::type& r) {

  return *l == r;
}

