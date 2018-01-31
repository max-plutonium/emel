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

#include <string>
#include <memory>

#include <boost/pool/pool_alloc.hpp>

namespace emel EMEL_EXPORT { namespace runtime EMEL_EXPORT {

class object;
using reference = std::shared_ptr<object>;
using const_reference = std::shared_ptr<const object>;

template <typename... Args>
  inline reference make_object(Args &&...args) {
      return std::allocate_shared<object>(boost::pool_allocator<object>(),
          std::forward<Args>(args)...);
  }

class object : public std::enable_shared_from_this<object>
{
    using base = std::enable_shared_from_this<object>;

public:
    enum type {
        is_empty, is_string, is_number, is_boolean, is_ref
    };

protected:
    type t = is_empty;

    union {
        std::string s;
        double d;
        bool b;
        reference ref;
    };

public:
    object();
    object(empty_value_type);
    object(const std::string &s);
    object(double d);
    object(bool b);
    object(reference ref);

    object(const object &other);
    object &operator =(const object &other);
    object(object &&other);
    object &operator =(object &&other);

    void swap(object &other);

    object &operator =(const std::string &s);
    object &operator =(double d);
    object &operator =(bool b);
    object &operator =(reference ref);

    type get_type() const { return t; }
    bool empty() const { return t == is_empty; }

    virtual ~object() { }
    virtual operator std::string() const;
    virtual operator double() const;
    virtual operator bool() const;
    virtual operator reference();
    virtual operator const_reference() const;

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
};

} // namespace runtime

} // namespace emel

#endif // OBJECT_H
