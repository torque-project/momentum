#pragma once

#include "exceptions.hpp"
#include "list.hpp"
#include "maybe.hpp"
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
  inline maybe<T> first(const std::shared_ptr<S>& s) {
    return !is_empty(s) ?
      maybe<T>(s->template first<T>())
      :
      maybe<T>();
  }

  template<typename S>
  inline maybe<typename S::value_type>
  first(const std::shared_ptr<S>& s) {
    return !is_empty(s) ?
      maybe<typename S::value_type>(s->first())
      :
      maybe<typename S::value_type>();
  }

  template<typename T, typename S>
  inline auto ffirst(const std::shared_ptr<S>& s)
    -> decltype(first<T, S>(first<T, S>(s))) {
    return first<T, S>(first<T, S>(s));
  }

  template<typename S>
  inline auto ffirst(const std::shared_ptr<S>& s)
    -> decltype(first<S>(first<S>(s))) {
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
  inline maybe<T> second(const std::shared_ptr<S>& s) {
    return first<T, S>(rest(s));
  }

  template<typename T, typename S>
  inline const T& last(const std::shared_ptr<S>& s) {

    auto head = s;
    auto next = rest(head);

    while (!is_empty(next)) {
      head = next;
      next = rest(next);
    }

    return *first<T, S>(head);
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

    while (auto x = first<arg_t>(head)) {
      out  = f(out, *x);
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

  template<typename T>
  inline ty::list::p take(uint64_t n, const std::shared_ptr<T>& x) {
    auto s = seq(x);
    if (n > 0 && !is_empty(s)) {
        return conj(take(n-1, rest(s), first(s)));
    }
    return nil<ty::list>();
  }

  template<typename F, typename S>
  inline std::shared_ptr<S> take_while(
    const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto f = first<arg_t>(s);
    if (f && pred(f)) {
      return conj(take_while(pred, rest(s), f));
    }

    return nil<S>();
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

    while (!is_empty(head) && pred(*first<arg_t>(head))) {
      head = rest(head);
    }

    return is_empty(head);
  }

  template<typename T, typename F, typename S>
  inline maybe<T>
  some(const F& pred, const std::shared_ptr<S>& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    if (is_seq(s)) {

      auto head = s;

      while (auto fst = first<arg_t>(head)) {
        if (pred(*fst)) {
          break;
        }
        head = rest(head);
      }

      return first<T>(head);
    }

    return maybe<T>();
  }

  template<typename F, typename S>
  inline auto some(const F& pred, const std::shared_ptr<S>& s)
    -> decltype(some<value, F, S>(pred, s)) {

    return some<value>(pred, s);
  }
}
