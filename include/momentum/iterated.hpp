#pragma once

#include "util.hpp"

namespace imu {

  namespace ty {

    template<typename T, typename mixin = no_mixin>
    struct basic_iterated_seq : public mixin {

      typedef std::shared_ptr<basic_iterated_seq> p;

      typedef typename std::iterator_traits<T>::value_type value_type;

      mixin _mixin;

      T     _begin;
      T     _end;

      inline basic_iterated_seq()
      {}

      inline basic_iterated_seq(const T& b, const T& e)
        : _begin(b)
        , _end(e)
      {}

      inline bool is_empty() const {
        return _begin == _end;
      }

      template<typename V>
      inline const V& first() const {
        return *_begin;
      }

      inline const value_type& first() const {
        return *_begin;
      }

      inline p rest() const {
        if (!is_empty()) {
          return nu<basic_iterated_seq>(_begin + 1, _end);
        }
        return p();
      }
    };

    template<typename T>
    using iterated_seq = basic_iterated_seq<T>;
  }

  template<typename T>
  inline typename ty::iterated_seq<T>::p iterated(const T& b, const T& e) {
    return nu<ty::iterated_seq<T>>(b, e);
  }

  template<typename T>
  inline decltype(auto) iterated(const T& coll) {

    auto b = std::begin(coll);
    auto e = std::end(coll);

    return nu<ty::iterated_seq<decltype(b)>>(b, e);
  }
}
