#pragma once

#include "iterated.hpp"
#include "maybe.hpp"
#include "semantics.hpp"
#include "util.hpp"

#include <tuple>

namespace imu {

  namespace ty {

    struct set_tag {};

    template<typename K = value, typename mixin = no_mixin>
    struct basic_hash_set : public mixin, set_tag {

      typedef typename mixin::template semantics<basic_hash_set>::p p;

      typedef K value_type;
      typedef K val_type;
      typedef basic_array_map<K, K> store_type;

      typename store_type::p _store;

      inline basic_hash_set()
        : _store(nu<store_type>())
      {}

      inline basic_hash_set(const basic_hash_set& m)
        : _store(m._store)
      {}

      template<typename... T>
      inline basic_hash_set(const T&... ks)
        : _store(nu<store_type>(ks...))
      {}

      template<typename T>
      static inline p from_std(const T& b, const T& e) {
        auto out = nu<basic_hash_set>();
        for (auto i=b; i!=e; ++i) {
          out->_store->assoc(*i, *i);
        }
        return out;
      }

      template<typename T>
      static inline p from_std(const T& coll) {
        return from_std(std::begin(coll), std::end(coll));
      }

      inline bool is_empty() const {
        return _store->count() == 0;
      }

      inline uint64_t count() const {
        return _store->count();
      }

      template<typename T, typename K0>
      inline maybe<T> get(const K0& k) {
        int64_t idx = _store->find(k);
        if (idx != -1) {
          return maybe<T>(value_cast<T>(k));
        }
        return maybe<T>();
      }

      template<typename K0>
      inline maybe<value_type> get(const K0& k) {
        int64_t idx = _store->find(k);
        if (idx != -1) {
          return maybe<value_type>(k);
        }
        return maybe<value_type>();
      }

      template<typename K0>
      inline bool contains(const K0& k) const {
        return (bool) get(k);
      }

      template<typename K0>
      inline void conj(const K0 k) {
        _store->assoc(k, k);
      }

      template<typename K0>
      static inline void conj(const p& p, const K0 k) {
        auto ret = nu<basic_hash_set>(p);
        ret->conj(k);
        return ret;
      }

      inline void disj(int64_t idx) {
        _store->dissoc(idx);
      }

      template<typename K0>
      inline void disj(const K0& k) {
        _store->dissoc(k);
      }
    };

    typedef basic_hash_set<> hash_set;
  }

  template<typename... T>
  inline ty::hash_set::p hash_set(const T&... elements) {
    return nu<ty::hash_set>(elements...);
  }

  template<typename T>
  inline auto hash_set(const T& coll)
    -> decltype(std::begin(coll), std::end(coll), ty::hash_set::p()) {
    return ty::hash_set::from_std(coll);
  }

  template<typename... TS>
  inline decltype(auto) seq(
    const std::shared_ptr<ty::basic_hash_set<TS...>>& m) {
    return keys(m->_store);
  }

  template<typename... TS>
  inline decltype(auto) seq(const ty::basic_hash_set<TS...>* const & m) {
    return keys(m->_store);
  }

  template<typename T>
  struct set_conjer {
    template<typename M>
    static inline decltype(auto) conj(const M& m, const T& x) {
      typedef typename semantics::real_type<M>::type type;
      return type::conj(m, x);
    }
  };

  template<>
  struct set_conjer<value> {
    template<typename M>
    static inline decltype(auto) conj(const M& m, const value& x) {
      typedef typename semantics::real_type<M>::type type;
      auto& p = x.get<typename type::value_type>();
      return type::conj(m, p);
    }
  };

  template<typename M, typename T>
  inline typename std::enable_if<
    std::is_base_of<
      ty::set_tag
      , typename semantics::real_type<M>::type
      >::value,
    typename semantics::real_type<M>::type::p
    >::type
  conj(const M& m, const T& x) {
    return set_conjer<T>::conj(m, x);
  }
}
