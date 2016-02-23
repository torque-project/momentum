/**
 * @file
 * @brief Core functions for working with momentum data types.
 *
 */

#pragma once

#include "exceptions.hpp"
#include "maybe.hpp"
#include "semantics.hpp"
#include "util.hpp"
#include "value.hpp"

#include <memory>

/**
 * @namespace imu
 * @brief imu
 *
 */
namespace imu {

  namespace ty {

    template<typename V = value, typename M = no_mixin>
    struct basic_list;

    typedef std::shared_ptr<basic_list<>> cons;
  }

  /**
   * Gets the nil instance for a given momentum type
   *
   * @brief nil generator
   * @return nil
   *
   */
  template<typename T>
  inline const std::shared_ptr<T>& nil() {
    static const std::shared_ptr<T> value;
    return value;
  }

  /**
   * @brief Empty checker
   * Checks if a sequence is empty
   *
   * @param s Any momentum sequence
   * @return true if the sequence is empty, otherwise false
   *
   */
  template<typename S>
  inline bool is_empty(const S& s) {
    return !s || s->is_empty();
  }

  /**
   * @brief Sequence checker
   * Checks if its input is a momentum sequence
   *
   * @param s Any input
   * @return true if the input is a sequence, otherwise false
   *
   */
  template<typename S>
  inline auto is_seq(const S& s)
    -> decltype(s->first(), s->rest(), bool()) {

    return (bool) s;
  }

  /**
   * @brief Cast to sequence
   * Converts its input into a momentum sequence if possible.
   * If the parameter is already a sequence but empty, nil is returned.
   * <b>nil</b> is a valid as input.
   *
   * @param s Convertible to seq
   * @return A new seq representing the input parameter
   *
   */
  template<typename S>
  inline auto seq(const S& s)
    -> decltype(s->first(), s->rest(), s) {
    return s;
  }

  /**
   * @brief Returns the first element of any sequence.
   * Result is wrapped as a maybe instance, which will be empty in case
   * the sequence is empty. The value will be cast to the specified
   * type <b>T</b>.
   *
   * @param s Any momentum sequence
   * @return The first element in the sequence (or an empty imu::maybe)
   *
   */
  template<typename T, typename S>
  inline maybe<T> first(const S& s) {
    return !is_empty(s) ?
      maybe<T>(s->template first<T>())
      :
      maybe<T>();
  }

  /**
   * @brief Returns the first element of any sequence.
   * Result is wrapped as a maybe instance, which will be empty in case the
   * sequence is empty.
   *
   * @param s Any momentum sequence
   * @return The first element in the sequence (or an empty imu::maybe)
   *
   */
  template<typename S>
  inline decltype(auto) first(const S& s) {
    typedef decltype(s->first()) value_type;
    return !is_empty(s) ?
      maybe<value_type>(s->first())
      :
      maybe<value_type>();
  }

  template<typename T, typename S>
  inline maybe<T> first(const maybe<S>& s) {
    return s ? first<T, S>(*s) : maybe<T>();
  }

  template<typename F, typename S>
  inline const F& first(const std::pair<F, S>& p) {
    return p.first;
  }

  template<typename T, typename F, typename S>
  inline const T& first(const std::pair<F, S>& p) {
    return value_cast<T>(p.first);
  }

  template<typename... T>
  inline decltype(auto) first(const std::tuple<T...>& t) {
    return std::get<0>(t);
  }

  template<typename T, typename... TS>
  inline decltype(auto) first(const std::tuple<TS...>& t) {
    return value_cast<T>(std::get<0>(t));
  }

  /**
   * @brief Same as first(rest(s))
   *
   */
  template<typename T, typename S>
  inline decltype(auto) ffirst(const std::shared_ptr<S>& s) {
    return first<T, S>(first<T, S>(s));
  }

  template<typename S>
  inline decltype(auto) ffirst(const std::shared_ptr<S>& s) {
    return first<S>(first<S>(s));
  }

  // @cond HIDE
  namespace sfinae {

    template<typename S>
    inline auto rest_impl_seq(const S& s)
      -> decltype(s->rest()) {

      return s ? s->rest() : S();
    }

    template<typename S>
    inline auto rest_impl(const S& s, int)
      -> decltype(rest_impl_seq(s)) {

      return rest_impl_seq(s);
    }

    template<typename S>
    inline auto rest_impl(const S& s, long)
      -> decltype(rest_impl_seq(seq(s), 0)) {

      return rest_impl_seq(seq(s));
    }
  }
  // @endcond

  /**
   * Returns its input sequence without the first element. If the sequence
   * is empty, an empty sequence of the smae type as the input sequence
   * is returned.
   *
   * @brief Returns the tail of a sequence
   * @param s Any momentum sequence
   * @return The input sequence without its first element or an empty sequence
   *
   */
  template<typename S>
  inline auto rest(const S& s)
    -> decltype(sfinae::rest_impl(s, 0)) {
    return sfinae::rest_impl(s, 0);
  }

  /**
   * @brief Returns the tail of a sequence
   * Returns its input sequence without the first n elements. If the sequence
   * is empty, an empty sequence of the smae type as the input sequence
   * is returned.
   *
   * @param n Number of elements to drop from the sequence
   * @param s Any momentum sequence
   * @return The input sequence without its first n elements or an
   *         empty sequence
   *
   */
  template<typename S>
  inline std::shared_ptr<S> nthrest(uint64_t n, const std::shared_ptr<S>& s) {

    auto head = s;
    auto cnt  = n;

    while (!is_empty(head) && cnt-- > 0) {
      head = rest(head);
    }

    return head;
  }

  /**
   * @brief Returns the second element of any sequence.
   * Result is wrapped as a maybe instance, which will be empty in case
   * the sequence is empty. The value will be cast to the specified
   * type <b>T</b>.
   *
   * @param s Any momentum sequence
   * @return The second element in the sequence (or an empty imu::maybe)
   *
   */
  template<typename T, typename S>
  inline maybe<T> second(const S& s) {
    return first<T, S>(rest(s));
  }

  template<typename S>
  inline decltype(auto) second(const S& s) {
    return first(rest(s));
  }

  template<typename T, typename S>
  inline maybe<T> second(const maybe<S>& s) {
    return s ? second<T, S>(*s) : maybe<T>();
  }

  template<typename F, typename S>
  inline const S& second(const std::pair<F, S>& p) {
    return p.second;
  }

  template<typename T, typename F, typename S>
  inline const T& second(const std::pair<F, S>& p) {
    return value_cast<T>(p.second);
  }

  template<typename... T>
  inline decltype(auto) second(const std::tuple<T...>& t) {
    return std::get<1>(t);
  }

  template<typename T, typename... TS>
  inline decltype(auto) second(const std::tuple<TS...>& t) {
    return value_cast<T>(std::get<1>(t));
  }

  /**
   * @brief Returns the last element of any sequence.
   * The value will be cast to the specified type <b>T</b>.
   *
   * @param s Any momentum sequence
   * @return The last element in the sequence (or an empty imu::maybe)
   *
   */
  template<typename T, typename S>
  inline const T& last(const S& s) {

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

  /**
   * @brief Returns the nth element of a sequence.
   * The value will be cast to the specified type <b>T</b>.
   * For sequences that don't support random acccess this is may be linear.
   * May throw out of bounds exceptions.
   *
   * @param s Any momentum sequence
   * @param idx The index of the requested element
   * @return The nth element in the sequence
   *
   */
  template<typename T, typename S>
  inline auto nth(const S& s, uint64_t idx)
    -> decltype(s->template nth<T>(idx)) {
    return s->template nth<T>(idx);
  }

  template<typename S>
  inline auto nth(const S& s, uint64_t idx)
    -> decltype(s->nth(idx)) {
    return s->nth(idx);
  }

  /*
  template<typename T, typename S>
  inline const T& nth(std::shared_ptr<S>& s, uint64_t idx) {
    first<T, S>(nthrest(idx, s));
  }
  */

  template<typename V, typename K, typename M>
  inline auto get(const M& m, const K& k)
    -> decltype(m->template get<V>(k)) {
    return m->template get<V>(k);
  }

  template<typename K, typename M>
  inline auto get(const M& m, const K& k)
    -> decltype(m->get(k)) {
    return m->get(k);
  }

  template<typename K, typename M>
  inline decltype(auto) get(
    const M& m, const K& k,
    const typename semantics::real_type<M>::type::val_type& d) {
    if (m) {
      if (auto v = m->get(k)) {
        return *v;
      }
    }
    return d;
  }

  /**
   * @brief Iterate a sequence of values
   * Calls a function on every value in seq
   *
   * @param f A function of one argument
   * @param x Any value on which seq can be called.
   * @return <b>void</b>
   */
  template<typename F, typename T>
  inline void for_each(const F& f, const T& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(x);
    while (!is_empty(head)) {
      f(value_cast<arg_t>(head->first()));
      head = rest(head);
    }
  }

  /**
   * @brief Reduces a sequence of values to a single value.
   * The values get reduced by iteratively computing
   * f(f(f(x, s0), s1), s2). Returns x in case s is empty.
   *
   * @param f A function of two arguments. The first parameter of
   *          the function must have the same type as T or at least be
   *          implicitly convertible. The second parameter can be any
   *          type as long as every value in the sequence is convertible
   *          to this type.
   * @param init An initial value.
   * @param x Any value on which seq can be called.
   * @return Returns the result of the reduction or x if s is empty.
   */
  template<typename F, typename T, typename S>
  inline T reduce(const F& f, const T& init, const S& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<1>::decayed arg_t;

    auto head = seq(x);
    auto out  = init;

    while (!is_empty(head)) {
      auto step = head->template first();
      out       = f(out, value_cast<arg_t>(step));
      head      = rest(head);
    }

    return out;
  }

  /**
   * @brief Maps a sequence of values to another sequence
   * The new sequence consists of the results of applying f to every value.
   + in the input sequence.
   *
   * @param f A function of one arguments The parameter can be any
   *          type as long as every value in the sequence is convertible
   *          to this type.
   * @param x Any value on which seq can be called.
   * @return Returns the result of the apply f to every value in the sequence.
   */
  template<typename F, typename S>
  inline ty::cons map(const F& f, const S& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    return reduce([=](const ty::cons& s, const arg_t& x) {
        return conj(s, f(x));
      },
      ty::cons(),
      seq(x));
  }

  /**
   * @brief Returns a new, filtered sequence of values.
   * The filtered sequence consists of all the values for which pred
   * returns true.
   *
   * @param pred A predicate function. The parameter can be any
   *             type as long as every value in the sequence is
   *             convertible to this type.
   * @param x Any value on which seq can be called.
   * @return Returns the filtered sequence.
   */
  template<typename F, typename S>
  inline ty::cons filter(const F& pred, const S& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    return reduce([=](const ty::cons& s, const arg_t& x) {
        return pred(x) ? conj(s, x) : s;
      },
      ty::cons(),
      seq(x));
  }

  /**
   * @brief <b>conj</b> the value of one sequence onto another.
   * Takes two sequences and calls conj on the first sequence
   * for every value in the second sequence.
   *
   * @param to   Any momentum sequence
   * @param from Any momentum sequence
   * @return Returns the newly formed sequence.
   */
  template<typename T, typename S>
  inline decltype(auto) into(const T& to, const S& from) {

    typedef decltype(from->first()) value_type;

    return reduce([](const T& s, const value_type& x) {
        return conj(s, x);
      },
      to,
      from);
  }

  /**
   * @brief Returns a new sequence of the first n values of x.
   * If less than n elements are in the sequence the entire input
   * sequence is returned.
   *
   * @param n The number of elements to take.
   * @param x Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename T>
  inline ty::cons take(uint64_t n, const T& x) {
    auto s = seq(x);
    if (n > 0 && !is_empty(s)) {
      return conj(take(n-1, rest(s)), *first(s));
    }
    return ty::cons();
  }

  /**
   * @brief Takes elements from the front of a sequence while a
   *        predicate applies.
   *
   * @param pred A predicate function of one argument.
   *             The parameter can be any  type as long as every value
   *             in the sequence is convertible to this type.
   * @param x    Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename F, typename S>
  inline ty::cons take_while(const F& pred, const S& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto f = first<arg_t>(s);
    if (f && pred(f)) {
      return conj(take_while(pred, rest(s), f));
    }

    return ty::cons();
  }

  /**
   * @brief Returns the input sequence with the first n elements dropped.
   * If less than n elements are in the sequence <b>nil</b> is returned.
   *
   * @param n The number of elements to drop.
   * @param s Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename S>
  inline decltype(auto) drop(uint64_t n, const S& x) {

    auto head = seq(x);
    auto m    = n;

    while (!is_empty(head) && m-- > 0) {
      head = rest(head);
    }

    return head;
  }

  /**
   * @brief Drops elements from the front of a sequence while a
   *        predicate applies.
   *
   * @param pred A predicate function of one argument.
   *             The parameter can be any  type as long as every value
   *             in the sequence is convertible to this type.
   * @param x    Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename F, typename S>
  inline decltype(auto) drop_while(const F& pred, const S& s) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(s);

    while (!is_empty(head) && pred(first<arg_t>(head))) {
      head = rest(head);
    }

    return head;
  }

  /**
   * @brief Returns true if a predicate applies to every value in a sequence.
   *
   * @param pred A predicate function of one argument.
   *             The parameter can be any  type as long as every value
   *             in the sequence is convertible to this type.
   * @param x    Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename F, typename S>
  inline bool is_every(const F& pred, const S& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(x);

    while (!is_empty(head) && pred(*first<arg_t>(head))) {
      head = rest(head);
    }

    return is_empty(head);
  }

  /**
   * @brief Returns the first element of a sequence for which a predicate
   * returns true.
   * The result gets converted to the type <b>T</b>. In case no value
   * passes the predicate, an emmpty maybe value is returned.
   *
   * @param pred A predicate function.
   * @param s Any momentum sequence.
   * @return A maybe value of type T, which may be empty.
   *
   */
  template<typename T, typename F, typename S>
  inline maybe<T>
  some(const F& pred, const S& s) {

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
  inline decltype(auto) some(const F& pred, const S& s) {
    return some<value>(pred, s);
  }

  /**
   * @brief Returns the number of elements in a sequence.
   * Maybe be linear if counting the sequence is not supported directly.
   *
   * @param s Any momentum sequence
   * @return The number of elements in the sequence
   *
   */
  template<typename S>
  inline auto count(const S& s)
    -> decltype(s->count(), uint64_t()) {

    return s ? s->count() : 0;
  }

  /*
  template<typename S>
  inline uint64_t count(const S& s) {
    return reduce(
      [](uint64_t s, const typename S::value_type&) {
        return s + 1;
      }, 0, s);
  }
  */

  template<typename T>
  inline ty::cons partition(uint64_t n, const T& x) {
    auto s = seq(x);
    if (!is_empty(s)) {
      return conj(partition(n, nthrest(n, s)), take(n, s));
    }
    return ty::cons();
  }

  template<typename M0, typename M1>
  inline decltype(auto) merge(const M0& m0, const M1& m1) {
    return into(m0, seq(m1));
  }
}
