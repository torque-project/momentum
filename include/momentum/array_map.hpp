#pragma once

#include "iterated.hpp"
#include "maybe.hpp"
#include "semantics.hpp"
#include "util.hpp"

#include <tuple>

namespace imu {

  namespace ty {

    template<typename M, int N, typename mixin = no_mixin>
    struct array_map_kv_seq : public mixin {

      typedef typename mixin::template semantics<array_map_kv_seq>::p p;

      typedef M map_type;
      typedef typename map_type::value_type kv_type;
      typedef typename std::tuple_element<N, kv_type>::type value_type;

      typename map_type::p _m;
      int64_t              _off;

      inline array_map_kv_seq(typename map_type::p& m, int64_t off)
        : _m(m), _off(off)
      {}

      inline array_map_kv_seq(typename map_type::p& m)
        : array_map_kv_seq(m, 0)
      {}

      inline bool is_empty() const {
        return (_off >= _m->_values.size());
      }

      inline uint64_t count() const {
        return _m->_values.size();
      }

      template<typename T>
      inline const T& first() {
        return value_cast<T>(std::get<N>(_m->_values[_off]));
      }

      inline const value_type& first() {
        return std::get<N>(_m->_values[_off]);
      }

      inline p rest() {
        if (!is_empty()) {
          return nu<array_map_kv_seq>(_m, _off + 1);
        }
        return p();
      }
    };

    template<typename K = value, typename V = value, typename mixin = no_mixin>
    struct basic_array_map : public mixin {

      typedef typename mixin::template semantics<basic_array_map>::p p;

      typedef K key_type;
      typedef V val_type;

      typedef std::tuple<K, V>        value_type;
      typedef std::vector<value_type> table_type;

      typedef array_map_kv_seq<basic_array_map, 0> key_seq;
      typedef array_map_kv_seq<basic_array_map, 1> val_seq;

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
          return maybe<T>(value_cast<T>(std::get<1>(_values[idx])));
        }
        return maybe<T>();
      }

      template<typename K0>
      inline maybe<val_type> get(const K0& k) {
        int64_t idx = find(k);
        if (idx != -1) {
          return maybe<val_type>(std::get<1>(_values[idx]));
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
  inline decltype(auto) seq(
    const std::shared_ptr<ty::basic_array_map<TS...>>& m) {

    return iterated(m->begin(), m->end());
  }

  template<typename... TS>
  inline decltype(auto) seq(const ty::basic_array_map<TS...>* const & m) {
    return iterated(m->begin(), m->end());
  }

  template<typename M>
  inline decltype(auto) keys(const M& m) {
    typedef typename semantics::real_type<M>::type type;
    return nu<typename type::key_seq>(m);
  }

  template<typename M>
  inline decltype(auto) vals(const M& m) {
    typedef typename semantics::real_type<M>::type type;
    return nu<typename type::val_seq>(m);
  }

  template<typename T, typename K, typename V>
  inline decltype(auto) assoc(const T& m, const K& k, const V& v) {
    typedef typename semantics::real_type<T>::type type;
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
