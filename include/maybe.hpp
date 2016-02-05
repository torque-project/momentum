#pragma once

#include <experimental/optional>
#include <type_traits>

template<typename T>
struct maybe {

  typedef typename std::decay<T>::type type;

  inline maybe()
    : ref(nullptr)
  {}

  inline maybe(const type& r)
    : ref(std::addressof(r))
  {}

  inline maybe(const maybe& cpy)
    : ref(cpy.ref)
  {}

  inline operator bool() const {
    return (bool) ref;
  }

  inline explicit operator const type&() const {
    return *ref;
  }

  inline const type& operator*() const {
    return *ref;
  }

  inline const type& operator->() const {
    return *ref;
  }

  inline const type& get() const {
    return *ref;
  }

  inline friend bool operator== (const maybe& l, const maybe& r) {
    return *l.ref == *r.ref;
  }

  inline friend bool operator== (const maybe& l, const type& r) {
    return *l.ref == r;
  }

  inline friend bool operator== (const type& l, const maybe& r) {
    return *r.ref == l;
  }


  const type* ref;
};
