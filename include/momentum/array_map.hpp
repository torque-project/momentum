#pragma once

#include "iterated.hpp"
#include "maybe.hpp"
#include "util.hpp"

namespace imu {

  namespace ty {

    template<typename K = value, typename V = value, typename mixin = no_mixin>
    struct basic_array_map : public mixin {

      typedef typename mixin::template semantics<basic_array_map>::p p;

      typedef K key_type;
      typedef V val_type;

      typedef std::pair<K, V>         value_type;
      typedef std::vector<value_type> table_type;

      typedef typename table_type::iterator       iterator;
      typedef typename table_type::const_iterator const_iterator;

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

      template<typename T>
      static inline p from_std(const T& coll) {
        p out = nu<basic_array_map>();
        auto i = coll.begin();
        while (i != coll.end()) {
          auto k = i++;
          auto v = i++;
          out->assoc(*k, *v);
        }
        return out;
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
          return maybe<T>(value_cast<T>(_values[idx].second));
        }
        return maybe<T>();
      }

      template<typename K0>
      inline maybe<val_type> get(const K0& k) {
        int64_t idx = find(k);
        if (idx != -1) {
          return maybe<val_type>(_values[idx].second);
        }
        return maybe<val_type>();
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

  template<typename T>
  inline auto array_map(const T& coll)
    -> decltype(std::begin(coll), std::end(coll), ty::array_map::p()) {
    return ty::array_map::from_std(coll);
  }

  template<typename... TS>
  inline decltype(auto) seq(const typename ty::basic_array_map<TS...>::p& m) {
    return iterated(m->begin(), m->end());
  }

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

  template<typename T, typename K, typename V>
  inline decltype(auto) assoc(const T& m, const K& k, const V& v) {
    typedef typename real_type<T>::type type;
    return nu<type>(*m, k, v);
  }

  template<typename K, typename... TS>
  inline typename ty::basic_array_map<TS...>::p dissoc(
    const typename ty::basic_array_map<TS...>::p& m, const K& k) {

    auto idx = m->find(k);
    if(idx != -1) {
      auto ret = nu<ty::basic_array_map<TS...>>(*m);
      ret->dissoc(idx);
      return ret;
    }
    return m;
  }

  template<typename T>
  struct conjer {
    template<typename... TS>
    static inline decltype(auto) conj(
      const typename ty::basic_array_map<TS...>::p& m, const T& x) {

      return assoc(m, first(x), second(x));
    }
  };

  template<>
  struct conjer<value> {
    template<typename... TS>
    static inline decltype(auto) conj(
      const typename ty::basic_array_map<TS...>::p& m, const value& x) {

      auto& p = x.get<typename ty::array_map::value_type>();
      return assoc(m, first(p), second(p));
    }
  };

  template<typename T, typename... TS>
  inline decltype(auto) conj(
    const typename ty::basic_array_map<TS...>::p& m, const T& x) {

    return conjer<T>::conj(m, x);
  }
}
