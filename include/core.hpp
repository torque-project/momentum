/**
 * @file
 * @brief Core functions for working with momentum data types.
 *
 */

#pragma once

#include "exceptions.hpp"
#include "list.hpp"
#include "maybe.hpp"
#include "util.hpp"
#include "value.hpp"

#include <memory>

/**
 * @namespace imu
 * @brief imu
 *
 */
namespace imu {

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
  inline bool is_empty(const std::shared_ptr<S>& s) {
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
  inline auto is_seq(const std::shared_ptr<S>& s)
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
  inline auto seq(const std::shared_ptr<S>& s)
    -> decltype(s->first(), s->rest(), typename S::p()) {

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
  inline maybe<T> first(const std::shared_ptr<S>& s) {
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
  inline maybe<typename S::value_type>
  first(const std::shared_ptr<S>& s) {
    return !is_empty(s) ?
      maybe<typename S::value_type>(s->first())
      :
      maybe<typename S::value_type>();
  }

  template<typename T, typename S>
  inline decltype(auto) ffirst(const std::shared_ptr<S>& s) {
    return first<T, S>(first<T, S>(s));
  }

  template<typename S>
  inline decltype(auto) ffirst(const std::shared_ptr<S>& s) {
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
  inline auto rest(const std::shared_ptr<S>& s)
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
  inline maybe<T> second(const std::shared_ptr<S>& s) {
    return first<T, S>(rest(s));
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
  inline auto nth(std::shared_ptr<S>& s, uint64_t idx)
    -> decltype(s->template nth<T>(idx)) {
    return s->template nth<T>(idx);
  }

  template<typename T, typename S>
  inline const T& nth(std::shared_ptr<S>& s, uint64_t idx) {
    first<T, S>(nthrest(idx, s));
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
   * @param x An initial value.
   * @param s Any value on which seq can be called.
   * @return Returns the result of the reduction or x if s is empty.
   */
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

  /**
   * @brief Maps a sequence of values to another sequence
   * The new sequence consists of the results of applying f to every value.
   + in the input sequence.
   *
   * @param f A function of one arguments The parameter can be any
   *          type as long as every value in the sequence is convertible
   *          to this type.
   * @param s Any value on which seq can be called.
   * @return Returns the result of the apply f to every value in the sequence.
   */
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

  /**
   * @brief Returns a new, filtered sequence of values.
   * The filtered sequence consists of all the values for which pred
   * returns true.
   *
   * @param pred A predicate function. The parameter can be any
   *             type as long as every value in the sequence is
   *             convertible to this type.
   * @param s Any value on which seq can be called.
   * @return Returns the filtered sequence.
   */
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
  inline std::shared_ptr<T> into(
    const std::shared_ptr<T>& to, const std::shared_ptr<S>& from) {

    typedef typename T::value_type value_type;

    return reduce([](const std::shared_ptr<T>& s, const value_type& x) {
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
  inline ty::list::p take(uint64_t n, const std::shared_ptr<T>& x) {
    auto s = seq(x);
    if (n > 0 && !is_empty(s)) {
        return conj(take(n-1, rest(s), first(s)));
    }
    return nil<ty::list>();
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

  /**
   * @brief Returns the input sequence with the first n elements dropped.
   * If less than n elements are in the sequence <b>nil</b> is returned.
   *
   * @param n The number of elements to drop.
   * @param s Any value on which seq can be called.
   * @return Returns the newly formed sequence.
   */
  template<typename S>
  inline std::shared_ptr<S> drop(uint64_t n, const std::shared_ptr<S>& x) {

    auto head = seq(s);
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
  inline bool is_every(const F& pred, const std::shared_ptr<S>& x) {

    typedef type_traits::lambda_traits<F> signature_t;
    typedef typename signature_t::template arg<0>::decayed arg_t;

    auto head = seq(s);

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
  inline decltype(auto) some(const F& pred, const std::shared_ptr<S>& s) {
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
  inline uint64_t count(const std::shared_ptr<S>& s)
    -> decltype(s->count(), uint64_t()) {

    return s ? s->count() : 0;
  }

  template<typename S>
  inline uint64_t count(const std::shared_ptr<S>& s) {
    return reduce(
      [](uint64_t s, const typename S::value_type&) {
        return s + 1;
      }, 0, s);
  }
}
