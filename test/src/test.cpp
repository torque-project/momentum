#include "core.hpp"
#include "list.hpp"
#include "iterated.hpp"
#include "vector.hpp"

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

void test_vector_0() {

  auto v = vector();
  assert(is_empty(v));
  assert(seq(v) == nullptr);

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

int main() {

  test_list_0();
  test_list_1();
  test_list_2();
  test_list_3();

  std::cout << "All list tests passed" << std::endl;

  test_vector_0();
  test_vector_1();
  test_vector_2();
  test_vector_3();
  test_vector_4();
  test_vector_5();
  test_vector_6();

  std::cout << "All vector tests passed" << std::endl;

  test_iterated_0();

  std::cout << "All iterated seq tests passed" << std::endl;

  test_reduce_0();
  test_reduce_1();
  test_reduce_2();
  test_map_0();
  test_filter_0();
  test_some_0();
  test_into_0();

  std::cout << "All core tests passed" << std::endl;

  std::cout << "All tests passed" << std::endl;

  return 0;
}
