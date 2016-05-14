#pragma once

#include "maybe.hpp"

namespace imu {

  namespace seqs {

    template<typename S, typename T, typename F>
    inline bool equiv(const S& s, const T& x, const F& eq) {

      auto h1 = s;
      auto h2 = seq(x);

      while (!is_empty(h1) && !is_empty(h2) && eq(*first(h1), *first(h2))) {
        h1 = rest(h1); h2 = rest(h2);
      }

      return is_empty(h1) && is_empty(h2);
    }

    template<typename S, typename T>
    inline bool equiv(const S& s, const T& x) {
      typedef typename semantics::real_type<S>::type::value_type value_type;
      return equiv(s, x, std::equal_to<value_type>());
    }
  }
}
