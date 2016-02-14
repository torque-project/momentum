#pragma once

#include "iterated.hpp"
#include "maybe.hpp"
#include "util.hpp"

namespace imu {

  namespace ty {

    template<typename K = value, typename V = value, typename mixin = no_mixin>
    struct basic_array_map {

      typedef std::shared_ptr<basic_array_map> p;

      typedef K key_type;
      typedef V val_type;

      typedef std::pair<K, V>         value_type;
      typedef std::vector<value_type> table_type;

      typedef typename table_type::iterator       iterator;
      typedef typename table_type::const_iterator const_iterator;

      mixin      _mixin;
      table_type _values;

      inline basic_array_map(const basic_array_map& m)
        : _values(m._values)
      {}

      template<typename K0, typename V0>
      inline basic_array_map(const basic_array_map& m, const K0& k, const V0& v)
        : basic_array_map(m) {
        assoc(k, v);
      }

      template<typename... T>
      inline basic_array_map(const T&... kvs) {
        assoc(kvs...);
      }

      inline bool is_empty() const {
        return _values.size() == 0;
      }

      inline uint64_t count() const {
        return _values.size();
      }

      template<typename T, typename K0>
      inline maybe<T> get(const K0& k) {
        int64_t idx = find(k);
        if (idx != -1) {
          return maybe<T>(_values[idx].second.template get<T>());
        }
        return maybe<T>();
      }

      template<typename K0>
      inline int64_t find(const K0& k) const {
        int64_t ret = 0;
        for(auto i = _values.begin(); i!=_values.end(); ++i, ++ret) {
          if (first(*i) == k) {
            return ret;
          }
        }
        return -1;
      }

      inline void assoc()
      {}

      template<typename K0, typename V0>
      inline void assoc(const K0& k, const V0& v) {
        int idx = find(k);
        if (idx != -1 ) {
          _values[idx] = value_type(key_type(k), val_type(v));
        }
        else {
          _values.emplace(_values.end(), key_type(k), val_type(v));
        }
      }

      template<typename K0, typename V0, typename... T>
      inline void assoc(const K0& k, const V0& v, const T&... kvs) {
        assoc(k, v);
        assoc(kvs...);
      }

      inline void dissoc(int64_t idx) {
        _values.erase(_values.begin() + idx);
      }

      template<typename K0>
      inline void dissoc(const K0& k) {
        int64_t idx = find(k);
        if (idx != -1) {
          dissoc(idx);
        }
      }

      inline const_iterator begin() const {
        return _values.begin();
      }

      inline const_iterator end() const {
        return _values.end();
      }
    };

    typedef basic_array_map<> array_map;
  }

  template<typename... T>
  inline ty::array_map::p array_map(const T&... elements) {
    return nu<ty::array_map>(elements...);
  }

  inline decltype(auto) seq(const ty::array_map::p& m) {
    return iterated(m->begin(), m->end());
  }

  template<typename K, typename V>
  inline ty::array_map::p assoc(
    const ty::array_map::p& m, const K& k, const V& v) {

    return nu<ty::array_map>(*m, k, v);
  }

  template<typename K>
  inline ty::array_map::p dissoc(const ty::array_map::p& m, const K& k) {
    auto idx = m->find(k);
    if(idx != -1) {
      auto ret = nu<ty::array_map>(*m);
      ret->dissoc(idx);
      return ret;
    }
    return m;
  }

  template<typename T>
  inline ty::array_map::p conj(const ty::array_map::p& m, const T& x) {
    return assoc(m, first(x), second(x));
  }

  inline ty::array_map::p conj(const ty::array_map::p& m, const value& x) {
    auto& p = x.get<typename ty::array_map::value_type>();
    return assoc(m, first(p), second(p));
  }
}
