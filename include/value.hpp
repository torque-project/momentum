#pragma once

#include "util.hpp"

#include <iostream>
#include <memory>
#include <type_traits>

namespace imu {

  struct value {

    inline value()
    {}

    template<typename T>
    inline value(const T& v)
      : pad(new value_pad<typename std::decay<const T>::type>(v))
    {}

    inline value(const value& cpy)
      : pad(cpy.pad ? cpy.pad->clone() : nullptr)
    {}

    template<typename T>
    inline const T& get() const {

      typedef typename std::decay<const T>::type value_type;
      typedef value_pad<value_type> pad_type;

      if (typeid(value_type) != type()) {
        throw bad_value_cast();
      }

      return dynamic_cast<pad_type*>(pad.get())->value;
    }

    inline bool operator== (const value& other) const {
      return pad->equiv(other.pad);
    }

    inline const std::type_info& type() const {
      return pad->type();
    }

    struct value_pad_base {

      typedef std::unique_ptr<value_pad_base> p;

      virtual value_pad_base* clone() const = 0;

      virtual bool equiv(const p& other) const = 0;

      virtual const std::type_info& type() const = 0;
    };

    template<typename T>
    struct value_pad : public value_pad_base {

      T value;

      value_pad(const T& v)
        : value(v)
      {}

      value_pad_base* clone() const {
        return new value_pad(value);
      }

      bool equiv(const value_pad_base::p& other) const {
        return value == dynamic_cast<value_pad*>(other.get())->value;
      }

      const std::type_info& type() const {
        return typeid(T);
      }
    };

    struct bad_value_cast : public std::bad_cast {

      virtual const char* what() const noexcept {
        return "Bad cast of imu::value to concrete type";
      }
    };

    std::unique_ptr<value_pad_base> pad;
  };

  template<typename T>
  inline const T& value_cast(const value& v) {
    return v.get<T>();
  }

  template<>
  inline const value& value_cast<value>(const value& v) {
    return v;
  }
}
