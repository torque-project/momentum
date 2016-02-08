#pragma once

#include <exception>
#include <string>

namespace imu {

  struct not_implemented : public std::exception {

    std::string msg;

    inline not_implemented(const std::string& meth )
      : msg("Operation not implemented: " + meth)
    {}

    virtual const char* what() const noexcept {
      return msg.c_str();
    }
  };

  struct out_of_bounds : public std::exception {

    uint64_t    idx;
    std::string msg;

    inline out_of_bounds(uint64_t i)
      : idx(i)
      , msg("Index out of bounds: " +
            std::to_string(i))
    {}

    inline out_of_bounds(uint64_t i, uint64_t size)
      : idx(i)
      , msg("Index out of bounds: " +
            std::to_string(i) +
            ". Collection had size: " +
            std::to_string(size))
    {}

    virtual const char* what() const noexcept {
      return msg.c_str();
    }
  };
}
