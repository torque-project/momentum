#pragma once

#include "seq.hpp"
#include "util.hpp"
#include "value.hpp"

#include <memory>

namespace imu {

  namespace ty {

    template<typename Value = value, typename mixin = no_mixin>
    struct basic_list {

      typedef std::shared_ptr<basic_list> p;

      // the type store in this list. by default this is
      // a generic type that can store any value
      typedef Value value_type;

      // a mixin can extra hold user data in this type
      mixin      _mixin;

      uint64_t   _count;
      value_type _first;
      p          _rest;

      inline basic_list()
        : _count(0)
      {}

      template<typename T>
      inline basic_list(const T& v, const p& l = p())
        : _count(l ? l->_count + 1 : 1)
        , _first(v)
        , _rest(l)
      {}

      inline bool is_empty() const {
        return _count == 0;
      }

      inline uint64_t count() const {
        return _count;
      }

      template<typename T>
      inline const T& first() const {
        return value_cast<T>(_first);
      };

      inline const value_type& first() const {
        return _first;
      };

      inline p rest() const {
        return _rest;
      }

      template<typename S>
      inline friend bool operator== (
        const p& self, const std::shared_ptr<S>& x) {

        return seqs::equiv(self, x);
      }
    };

    typedef basic_list<> list;
  }

  inline ty::list::p list() {
    return ty::list::p();
  }

  template<typename Val, typename... Vals>
  inline ty::list::p list(const Val& val, Vals... vals) {
    return nu<ty::list>(val, list(vals...));
  }

  /**
   * @namespace fxd
   * @brief Functions in this namespace create sequences
   * with a fixed type.
   *
   */
  namespace fxd {

    template<typename T>
    inline typename ty::basic_list<T>::p list() {
      return typename ty::basic_list<T>::p();
    }

    template<typename Val, typename... Vals>
    inline typename ty::basic_list<Val>::p
    list(const Val& val, Vals... vals) {
      return nu<ty::basic_list<Val>>(val, list<Val>(vals...));
    }
  }

  template<typename T>
  inline ty::list::p list(std::initializer_list<T> l) {
    auto out = nu<ty::list>();
    for (auto& val : l) {
      out = nu<ty::list>(val, out);
    }
    return out;
  }

  // @cond HIDE
  template<typename T, typename X>
  inline typename ty::basic_list<T>::p
  conj(const std::shared_ptr<ty::basic_list<T>>& l, const X& v) {
    return nu<ty::basic_list<T>>(v, l);
  }
  // @endcond
}
