#pragma once

#include "maybe.hpp"

namespace imu {

  namespace seqs {

    template<typename S, typename T>
    inline bool equiv(const S& s, const T& x) {

      auto h1 = s;
      auto h2 = seq(x);

      while (!is_empty(h1) && !is_empty(h2) && first(h1) == first(h2)) {
        h1 = rest(h1); h2 = rest(h2);
      }

      return is_empty(h1) && is_empty(h2);
    }
  }
}
