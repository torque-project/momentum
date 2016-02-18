#pragma once

#include <type_traits>

template<typename T>
struct maybe {

  typedef typename std::decay<T>::type type;

  inline maybe()
    : ref(nullptr)
  {}

  inline explicit maybe(const type& r)
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

  inline const type* operator->() const {
    return ref;
  }

  inline const type& get() const {
    return *ref;
  }

  const type* ref;
};

template<typename T>
inline bool operator== (const maybe<T>& l, const maybe<T>& r) {
  return *l.ref == *r.ref;
}
/*
template<typename T>
inline bool operator== (const maybe<T>& l, const typename maybe<T>::type& r) {
  return *l.ref == r;
}
*/
template<typename T>
inline bool operator== (const typename maybe<T>::type& l, const maybe<T>& r) {
  return *r.ref == l;
}

template<typename T, typename S>
inline bool operator== (const maybe<T>& l, const S& r) {
  return *l.ref == r;
}

template<typename T>
inline bool operator<= (const maybe<T>& l, const maybe<T>& r) {
  return *l.ref <= *r.ref;
}

template<typename T>
inline bool operator<= (const maybe<T>& l, const typename maybe<T>::type& r) {
  return *l.ref <= r;
}

template<typename T>
inline bool operator<= (const typename maybe<T>::type& l, const maybe<T>& r) {
  return *r.ref <= l;
}

template<typename T>
inline bool operator>= (const maybe<T>& l, const maybe<T>& r) {
  return *l.ref <= *r.ref;
}

template<typename T>
inline bool operator>= (const maybe<T>& l, const typename maybe<T>::type& r) {
  return *l.ref <= r;
}

template<typename T>
inline bool operator>= (const typename maybe<T>::type& l, const maybe<T>& r) {
  return *r.ref <= l;
}
