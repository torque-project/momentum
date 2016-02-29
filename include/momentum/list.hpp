#pragma once

#include "seq.hpp"
#include "util.hpp"
#include "value.hpp"

#include <memory>

namespace imu {

  namespace ty {

    struct cons_tag {};

    template<typename Value, typename mixin>
    struct basic_list : public mixin, cons_tag {

      typedef typename mixin::template semantics<basic_list>::p  p;
      typedef typename mixin::template semantics<basic_list>::cp cp;

      // the type store in this list. by default this is
      // a generic type that can store any value
      typedef Value value_type;

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

      static inline p factory() {
        return p();
      }

      template<typename Val, typename... Vals>
      static inline p factory(const Val& val, Vals... vals) {
        return nu<basic_list>(val, factory(vals...));
      }

      template<typename T>
      static inline p from_std(const T& coll) {
        auto out = p();
        for (auto i=coll.rbegin(); i!=coll.rend(); ++i) {
          out = nu<basic_list>(*i, out);
        }
        return out;
      }

      inline bool is_empty() const {
        return _count == 0;
      }

      inline uint64_t count() const {
        return _count;
      }

      template<typename T>
      inline decltype(auto) first() const {
        return value_cast<T>(_first);
      };

      inline const value_type& first() const {
        return _first;
      };

      inline p rest() const {
        return _count == 1 ? p() : _rest;
      }

      template<typename S>
      inline friend bool operator== (const p& self, const S& x) {
        return seqs::equiv(self, x);
      }

      template<typename S>
      inline friend bool operator== (const p& self, const std::shared_ptr<S>& x) {
        return seqs::equiv(self, x);
      }
    };

    typedef basic_list<> list;
  }

  inline ty::list::p list() {
    return ty::list::factory();
  }

  template<typename Val, typename... Vals>
  inline ty::list::p list(const Val& val, Vals... vals) {
    return ty::list::factory(val, vals...);
  }

  template<typename T>
  inline auto list(const T& coll)
    -> decltype(std::begin(coll), std::end(coll), ty::list::p()) {
    return ty::list::from_std(coll);
  }

  template<typename T>
  inline ty::list::p list(std::initializer_list<T> l) {
    return ty::list::from_std(l);
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

  // @cond HIDE
  template<typename S, typename T>
  inline typename std::enable_if<
    std::is_base_of<
      ty::cons_tag
      , typename semantics::real_type<S>::type
      >::value,
    typename semantics::real_type<S>::type::p
    >::type
  conj(const S& s, const T& x) {
    typedef typename semantics::real_type<S>::type type;
    return nu<type>(x, s);
  }
  // @endcond
}
