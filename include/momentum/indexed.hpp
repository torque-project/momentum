#pragma once

#include "util.hpp"

namespace imu {

  namespace ty {

    template<typename T, typename mixin = no_mixin>
    struct basic_indexed_seq {

      typedef std::shared_ptr<basic_indexed_seq> p;

      mixin    _mixin;

      T        _indexed;
      uint64_t _cnt;
      uint64_t _idx;

      inline basic_indexed_seq(T i, uint64_t c)
        : _indexed(i)
        , _cnt(c)
      {}

      inline bool is_empty() const {
        return _cnt == 0;
      }

      inline auto first() const -> decltype(_indexed[_idx]) {
        return _indexed[_idx];
      }

      inline p rest() const {
        if (_idx + 1 < _cnt) {
          return nu<basic_indexed_seq>(_indexed, _cnt, _idx + 1);
        }
        return p();
      }
    };

    template<typename T>
    using indexed_seq = basic_indexed_seq<T>;
  }

  template<typename T, int n>
  inline typename ty::indexed_seq<const T(&)[n]>::p indexed(const T(&arr)[n]) {
    return nu<ty::indexed_seq<const T(&)[n]>>(arr, n);
  }

  template<typename T>
  inline auto seq(const typename ty::indexed_seq<T>::p& s) -> decltype(s) {
    return s;
  }
}
