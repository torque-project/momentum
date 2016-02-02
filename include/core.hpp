#pragma once

#include "exceptions.hpp"
#include "list.hpp"
#include "util.hpp"
#include "value.hpp"

#include <memory>

namespace imu {

  template<typename T>
  inline const std::shared_ptr<T>& nil() {
    static const std::shared_ptr<T> value;
    return value;
  }

  template<typename S>
  inline bool is_empty(const std::shared_ptr<S>& s) {
    return !s || s->is_empty();
  }

  template<typename S>
  inline auto is_seq(const std::shared_ptr<S>& s)
    -> decltype(s->first(), s->rest(), bool()) {

    return (bool) s;
  }
  
  template<typename S>
  inline auto seq(const std::shared_ptr<S>& s)
    -> decltype(s->first(), s->rest(), typename S::p()) {

    return s;
  }

  template<typename T, typename S>
  inline T first(const std::shared_ptr<S>& s) {
    return s ? s->template first<T>() : T();
  }

  template<typename S>
  inline typename S::value_type first(const std::shared_ptr<S>& s) {
    return s ? s->first() : typename S::value_type();
  }

  template<typename T, typename S>
  inline T ffirst(const std::shared_ptr<S>& s) {
    return first<T, S>(first<T, S>(s));
  }

  template<typename S>
  inline typename S::value_type ffirst(const std::shared_ptr<S>& s) {
    return first<S>(first<S>(s));
  }

  namespace sfinae {

    template<typename S>
    inline auto rest_impl_seq(const std::shared_ptr<S>& s)
      -> decltype(s->rest()) {

      return s ? s->rest() : std::shared_ptr<S>();
    }

    template<typename S>
    inline auto rest_impl(const std::shared_ptr<S>& s, int)
      -> decltype(rest_impl_seq(s)) {

      return rest_impl_seq(s);
    }

    template<typename S>
    inline auto rest_impl(const std::shared_ptr<S>& s, long)
      -> decltype(rest_impl_seq(seq(s), 0)) {

      return rest_impl_seq(seq(s));
    }
  }

  template<typename S>
  inline auto rest(const std::shared_ptr<S>& s)
    -> decltype(sfinae::rest_impl(s, 0)) {

    return sfinae::rest_impl(s, 0);
  }

  template<typename S>
  inline std::shared_ptr<S> nthrest(uint64_t n, const std::shared_ptr<S>& s) {

    auto head = s;
    auto cnt  = n;

    while (!is_empty(head) && cnt-- > 0) {
      head = rest(head);
    }

    return head;
  }

  template<typename T, typename S>
  inline T second(const std::shared_ptr<S>& s) {
    return first<T, S>(rest(s));
  }

  template<typename T, typename S>
  inline T last(const std::shared_ptr<S>& s) {

    auto head = s;
    auto next = rest(head);

    while (!is_empty(next)) {
      head = next;
      next = rest(next);
    }

    return first<T, S>(head);
  }


  template<typename T>
  inline const T& identity(const T& x) {
    return x;
  }

  template<typename S>
  inline uint64_t count(const std::shared_ptr<S>& s) {
    return s ? s->count() : 0;
  }

  template<typename T, typename S>
  inline T nth(std::shared_ptr<S>& s, uint64_t idx) {
    return s->template nth<T>(idx);
  }

  template<typename F, typename T, typename S>
  inline T reduce(
    const F& f, const T& x, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<1>::decayed arg_t;

    auto head = seq(s);
    auto out  = x;

    while (!is_empty(head)) {
      out  = f(out, first<arg_t>(head));
      head = rest(head);
    }

    return out;
  }

  template<typename F, typename S>
  inline ty::list::p map(
    const F& f, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    return reduce([=](const ty::list::p& s, const arg_t& x) {
        return conj(s, f(x));
      },
      list(),
      seq(s));
  }

  template<typename F, typename S>
  inline ty::list::p filter(
    const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    return reduce([=](const ty::list::p& s, const arg_t& x) {
        return pred(x) ? conj(s, x) : s;
      },
      list(),
      seq(s));
  }

  template<typename T, typename S>
  inline std::shared_ptr<T> into(
    const std::shared_ptr<T>& to, const std::shared_ptr<S>& from) {

    typedef typename T::value_type value_type;

    return reduce([](const std::shared_ptr<T>& s, const value_type& x) {
        return conj(s, x);
      },
      to,
      from);
  }

  template<typename S>
  inline std::shared_ptr<S> take(uint64_t n, const std::shared_ptr<S>& s) {
    return conj(take(n-1, rest(s), first(s)));
  }

  template<typename F, typename S>
  inline std::shared_ptr<S> take_while(
    const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto f = first<arg_t>(s);
    if (pred(f)) {
      return conj(take(pred, rest(s), first(s)));
    }

    return std::shared_ptr<S>();
  }

  template<typename S>
  inline std::shared_ptr<S> drop(uint64_t n, const std::shared_ptr<S>& s) {

    auto head = seq(s);
    auto m    = n;

    while (!is_empty(head) && m-- > 0) {
      head = rest(head);
    }

    return head;
  }

  template<typename F, typename S>
  inline std::shared_ptr<S> drop_while(
    const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(s);

    while (!is_empty(head) && pred(first<arg_t>(head))) {
      head = rest(head);
    }

    return head;
  }

  template<typename F, typename S>
  inline bool is_every(
    const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(s);

    while (!is_empty(head) && pred(first<arg_t>(head))) {
      head = rest(head);
    }

    return is_empty(head);
  }
}
