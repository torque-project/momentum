#include "core.hpp"
#include "list.hpp"
#include "iterated.hpp"
#include "vector.hpp"
#include "array_map.hpp"

#include <cassert>
#include <iostream>

using namespace imu;

void test_list_0() {

  auto lst = list();
  assert(!is_seq(lst));
  assert(is_empty(lst));
}

void test_list_1() {

  auto lst = list(1, 2, 3);

  assert(is_seq(lst));
  assert(first<int>(lst) == 1);
  assert(second<int>(lst) == 2);
  assert(first<int>(nthrest(2, lst)) == 3);
  assert(last<int>(lst) == 3);
  assert(is_empty(nthrest(3, lst)));
}

void test_list_2() {

  auto lst = list();

  lst = conj(lst, 1);

  assert(first<int>(lst) == 1);
  assert(is_empty(rest(lst)));

  lst = conj(lst, 0);

  assert(first<int>(lst) == 0);
  assert(second<int>(lst) == 1);
  assert(is_empty(nthrest(2, lst)));
}

void test_list_3() {

  auto lst = list();
  assert(count(lst) == 0);

  lst = conj(lst, 1);
  assert(count(lst) == 1);

  lst = conj(lst, 2);
  assert(count(lst) == 2);
}

void test_list_4() {

  assert(list(1, 2, 3) == list(1, 2, 3));
}

void test_vector_0() {

  auto v = vector();
  assert(is_empty(v));
  assert(!seq(v));

  v = conj(v, 1);
  assert(first<int>(seq(v)) == 1);

  auto s = seq(conj(v, 2));
  assert(first<int>(s) == 1);
  assert(second<int>(s) == 2);
}

void test_vector_1() {

  auto s = seq(vector(1, 2, 3));

  assert(first<int>(s) == 1);
  assert(second<int>(s) == 2);
  assert(first<int>(nthrest(2, s)) == 3);
  assert(last<int>(s) == 3);
  assert(is_empty(nthrest(3, s)));
}

void test_vector_2() {

  auto v = vector(1, 2, 3);

  assert(nth<int>(v, 0) == 1);
  assert(nth<int>(v, 1) == 2);
  assert(nth<int>(v, 2) == 3);

  try {
    nth<int>(v, 3);
    assert(false);
  }
  catch(out_of_bounds& e) {
    assert(e.idx == 3);
  }
}

void test_vector_3() {

  auto v = vector();
  assert(count(v) == 0);

  v = conj(v, 1);
  assert(count(v) == 1);

  v = conj(v, 2);
  assert(count(v) == 2);
}

void test_vector_4() {

  assert(!is_seq(seq(vector())));
  assert(is_seq(seq(vector(1, 2, 3))));
}

void test_vector_5() {

  auto v = vector({1, 2, 3});

  assert(count(v) == 3);

  auto s = seq(v);

  assert(first<int>(s) == 1);
  assert(second<int>(s) == 2);
  assert(first<int>(nthrest(2, s)) == 3);
  assert(last<int>(s) == 3);
  assert(is_empty(nthrest(3, s)));
}

void test_vector_6() {

  auto v = vector();

  for (int i=0; i<128; ++i) {
    v = conj(v, i);
  }

  assert(count(v) == 128);

  for (int i=0; i<128; ++i) {
    assert(v->nth<int>(i) == i);
  }
}

void test_vector_7() {

  auto v = vector(1, 2, 3);

  v = assoc(v, 1, 5);
  assert(nth<int>(v, 1) == 5);
}

void test_array_map_0() {

  std::string foo("foo");
  std::string bar("bar");

  auto m = array_map(foo, 1, bar, 2);

  assert(count(m) == 2);
  assert(get<int>(m, foo) == 1);
  assert(get<int>(m, bar) == 2);
}

void test_array_map_1() {

  auto m = array_map(std::string("foo"), 1, std::string("bar"), 2);
  auto s = seq(m);

  assert(is_seq(s));

  assert(first<std::string>(*first(s)) == "foo");
  assert(second<int>(*first(s)) == 1);

  assert(first<std::string>(*second(s)) == "bar");
  assert(second<int>(*second(s)) == 2);
}

void test_array_map_2() {

  std::string foo("foo");
  std::string bar("bar");

  auto m = array_map();
  m = assoc(m, foo, 1);
  m = assoc(m, bar, 2);

  assert(count(m) == 2);
  assert(get<int>(m, foo) == 1);
  assert(get<int>(m, bar) == 2);
}

void test_array_map_3() {

  std::string foo("foo");
  std::string bar("bar");

  auto m = array_map(foo, 1, bar, 2);
  m = dissoc(m, bar);

  assert(count(m) == 1);
  assert(get<int>(m, foo) == 1);
}

void test_array_map_4() {

  std::string foo("foo");
  std::string bar("bar");

  std::vector<value> v = {foo, 1, bar, 2};

  auto m = array_map(v);

  assert(count(m) == 2);
  assert(get<int>(m, foo) == 1);
  assert(get<int>(m, bar) == 2);
}

void test_array_map_5() {

  std::string foo("foo");
  std::string bar("bar");

  std::vector<value> v = {foo, 1, bar, 2};

  auto m  = array_map(v);
  auto ks = keys(m);

  assert(count(m) == 2);
  assert(first<std::string>(ks)  == foo);
  assert(second<std::string>(ks) == bar);

  auto vs = vals(m);

  assert(count(m) == 2);
  assert(first<int>(vs)  == 1);
  assert(second<int>(vs) == 2);
}

void test_array_map_6() {

  std::string foo("foo");
  std::string bar("bar");

  auto m = update(array_map(foo, 1, bar, 2), bar, [](int x) {
      return x + 6;
    });

  assert(get<int>(m, foo) == 1);
  assert(get<int>(m, bar) == 8);
}

void test_iterated_0() {

  int foo[3] = {1, 2, 3};

  auto s = iterated(std::begin(foo), std::end(foo));

  assert(is_seq(s));
  assert(first<int>(s) == 1);
  assert(second<int>(s) == 2);
  assert(first<int>(nthrest(2, s)) == 3);

  auto s2 = iterated(foo);

  assert(first<int>(s2) == 1);
  assert(second<int>(s2) == 2);
  assert(first<int>(nthrest(2, s2)) == 3);
}

void test_for_each_0() {

  auto lst = list(1, 2, 3);

  int s = 0;
  for_each([&](int x) {
    s += x;
  }, lst);

  assert(s == 6);
}

void test_reduce_0() {

  auto lst = list(1, 2, 3);

  int sum = reduce([](int s, int x){
    return s + x;
  }, 0, lst);

  assert(sum == 6);
}

void test_reduce_1() {

  std::vector<int> v({1, 2, 3});

  int sum = reduce([](int s, int x){
      return s + x;
    }, 0, iterated(v));

  assert(sum == 6);
}

void test_reduce_2() {

  auto lst = list(1, 2, 3);

  try {

    int sum = reduce([](int s, float x) {
        return s + x;
      }, 0, lst);

    assert(false);
  }
  catch (value::bad_value_cast& e) {
  }
}

void test_reduce_3() {

  auto lst = list(1, 2, 3, 4);
  auto prt = partition(2, lst);

  assert(imu::first(prt)  == list(1, 2));
  assert(imu::second(prt) == list(3, 4));

  int sum = reduce([](int s, const imu::ty::cons& x) {
      return (s + *imu::first<int>(x) + *imu::second<int>(x));
    }, 0, prt);

  assert(sum == 10);
}

void test_map_0() {

  auto v = vector(1, 2, 3);

  auto s = map([](int x){
      return x + 1;
    }, v);

  assert(reduce([](int s, int x) {return s+x;}, 0, s) == 9);
}

void test_filter_0() {

  auto v = vector(1, 2, 3, 4, 5);

  auto s = filter([](int x){
      return (x & 1) == 0;
    }, v);

  assert(is_every([](int x) { return (x & 1) == 0; }, s));
}

void test_some_0() {

  auto s = list(1, 2, 3, 4, 5);

  auto n = some<int>([](int x){
      return x > 3;
    }, s);

  assert(n && n == 4);

  auto m = some([](int x){
      return x > 5;
    }, s);

  assert(!m);
}

void test_into_0() {

  auto v   = vector();
  auto lst = list(1, 2, 3, 4, 5);

  assert(into(v, lst) == lst);
}

void test_into_1() {

  typedef std::pair<int, int> pair;

  auto lst =
    fxd::list(
      std::make_pair(1, 1),
      std::make_pair(3, 2),
      std::make_pair(5, 3));

  auto m = into(array_map(), lst);

  assert(count(m) == 3);

  assert(get<int>(m, 1) == 1);
  assert(get<int>(m, 3) == 2);
  assert(get<int>(m, 5) == 3);
}

void test_into_2() {

  typedef ty::array_map::value_type pair;

  auto lst = list(pair(1, 1), pair(3, 2), pair(5, 3));
  auto m   = into(array_map(), lst);

  assert(count(m) == 3);

  assert(get<int>(m, 1) == 1);
  assert(get<int>(m, 3) == 2);
  assert(get<int>(m, 5) == 3);
}

void test_into_3() {

  auto m0 = array_map(1, 1, 3, 2, 5, 3);
  auto m1 = into(array_map(), seq(m0));

  assert(count(m1) == 3);

  assert(get<int>(m1, 1) == 1);
  assert(get<int>(m1, 3) == 2);
  assert(get<int>(m1, 5) == 3);
}

void test_take_0() {

  auto lst  = list(1, 2, 3, 4);
  auto lst2 = imu::take(2, lst);

  auto fst = imu::first<int>(lst2);
  auto snd = imu::second<int>(lst2);

  assert(fst && snd);
  assert(fst == 1);
  assert(snd == 2);
}

void test_partition_0() {

  auto lst  = list(1, 2, 3, 4);
  auto lst2 = imu::partition(2, lst);

  auto fst = imu::first<imu::ty::cons, imu::ty::cons>(lst2);
  auto snd = imu::second<imu::ty::cons, imu::ty::cons>(lst2);

  assert(fst && snd);
  assert(imu::first<int>(fst) == 1 && imu::second<int>(fst) == 2);
  assert(imu::first<int>(snd) == 3 && imu::second<int>(snd) == 4);
}

void test_partition_by_0() {

  auto lst = imu::partition_by([](int x) {
      return x < 3;
    },
    list(1, 2, 3, 4));

  auto fst = imu::first<imu::ty::cons, imu::ty::cons>(lst);
  auto snd = imu::second<imu::ty::cons, imu::ty::cons>(lst);

  assert(fst && snd);
  assert(imu::first<int>(fst) == 1 && imu::second<int>(fst) == 2);
  assert(imu::first<int>(snd) == 3 && imu::second<int>(snd) == 4);
}

void test_merge_0() {

  auto m0 = array_map(1, 3, 2, 5);
  auto m1 = array_map(8, 0, 12, 23);

  auto m  = merge(m0, m1);

  assert(count(m) == 4);
  assert(get<int>(m, 1)  == 3);
  assert(get<int>(m, 2)  == 5);
  assert(get<int>(m, 8)  == 0);
  assert(get<int>(m, 12) == 23);
}

int main() {

  test_list_0();
  test_list_1();
  test_list_2();
  test_list_3();
  test_list_4();

  std::cout << "All list tests passed" << std::endl;

  test_vector_0();
  test_vector_1();
  test_vector_2();
  test_vector_3();
  test_vector_4();
  test_vector_5();
  test_vector_6();
  test_vector_7();

  std::cout << "All vector tests passed" << std::endl;

  test_array_map_0();
  test_array_map_1();
  test_array_map_2();
  test_array_map_3();
  test_array_map_4();
  test_array_map_5();
  test_array_map_6();

  std::cout << "All array_map tests passed" << std::endl;

  test_iterated_0();

  std::cout << "All iterated seq tests passed" << std::endl;

  test_for_each_0();
  test_reduce_0();
  test_reduce_1();
  test_reduce_2();
  test_reduce_3();
  test_map_0();
  test_filter_0();
  test_some_0();
  test_into_0();
  test_into_1();
  test_into_2();
  test_into_3();
  test_take_0();
  test_partition_0();
  test_partition_by_0();
  test_merge_0();

  std::cout << "All core tests passed" << std::endl;

  std::cout << "All tests passed" << std::endl;

  return 0;
}
