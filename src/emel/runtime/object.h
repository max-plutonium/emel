/*
 * Copyright (C) 2015 Max Plutonium <plutonium.max@gmail.com>
 *
 * This file is part of the EMEL library.
 *
 * The EMEL library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * The EMEL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the EMEL library. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef OBJECT_H
#define OBJECT_H

#include "../opcodes.h"
#include "array.h"

#include <memory>
#include <string>

#include <experimental/optional>
#include <boost/pool/pool_alloc.hpp>

namespace emel EMEL_EXPORT { namespace runtime EMEL_EXPORT {

class object;
using reference = std::shared_ptr<object>;
using const_reference = std::shared_ptr<const object>;

template <typename Tp>
  using optional = std::experimental::optional<Tp>;

template <typename... Args>
  inline reference make_object(Args &&...args) {
      return std::allocate_shared<object>(boost::pool_allocator<object>(),
          std::forward<Args>(args)...);
  }

class object
{
public:
    enum type {
        is_empty, is_array, is_string, is_number, is_boolean, is_ref
    };

protected:
    type t;

    union {
        array a;
        std::string s;
        double d;
        bool b;
        reference ref;
    };

public:
    object();
    object(empty_value_type);
    object(array a);
    object(object *ptr, std::size_t len);
    object(const std::vector<object> &vec);
    object(std::vector<object> &&vec);
    object(const std::string &s);
    object(const char *s);
    object(double d);
    object(bool b);
    object(reference ref);

    object(const object &other);
    object &operator =(const object &other);
    object(object &&other);
    object &operator =(object &&other);

    void swap(object &other);

    object &operator =(empty_value_type);
    object &operator =(array a);
    object &operator =(const std::string &s);
    object &operator =(const char *s);
    object &operator =(double d);
    object &operator =(bool b);
    object &operator =(reference ref);

    type get_type() const { return t; }
    bool empty() const { return t == is_empty; }

    virtual object share();
    virtual object clone();

    virtual ~object();
    virtual operator std::string() const;
    virtual operator double() const;
    virtual operator bool() const;
    virtual operator reference() const;

    virtual bool operator ==(const object &other) const;
    virtual bool operator !=(const object &other) const;
    virtual bool operator <(const object &other) const;
    virtual bool operator >(const object &other) const;
    virtual bool operator <=(const object &other) const;
    virtual bool operator >=(const object &other) const;
    virtual object operator +(const object &other) const;
    virtual object operator -(const object &other) const;
    virtual object operator *(const object &other) const;
    virtual object operator /(const object &other) const;

    std::size_t size() const;
    object operator[](std::size_t);
    object operator[](std::size_t) const;

    optional<array> as_array() const;
    optional<std::string> as_string() const;
    optional<double> as_number() const;
    optional<bool> as_bool() const;
    optional<reference> as_ref() const;
};

EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const object &arg);

} // namespace runtime

} // namespace emel

#endif // OBJECT_H
