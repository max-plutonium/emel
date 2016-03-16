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
#pragma once

#include "../type-system/type.h"

#include <boost/optional.hpp>

namespace emel EMEL_EXPORT { namespace runtime EMEL_EXPORT {

class object;
using reference = object *;
using const_reference = const object *;

class object
{
protected:
	type::rep d;
    explicit object(type::rep d);

public:
    object();
    object(const std::vector<object> &vec);
    object(std::vector<object> &&vec);
    object(const std::string &s);
    object(const char *s);
    object(double num);
    object(std::int64_t i);
    object(bool b);
    object(reference ref);

    virtual ~object();

    object(const object &other);
    object &operator =(const object &other);
    object(object &&other);
    object &operator =(object &&other);

    void swap(object &other) noexcept;
	void detach();

    object &operator =(const std::string &s);
    object &operator =(const char *s);
    object &operator =(double num);
    object &operator =(bool b);
    object &operator =(reference ref);

	type::kind get_type() const;
    bool empty() const;
    std::size_t size() const;

    virtual operator bool() const;
    virtual operator std::int64_t() const;
    virtual operator double() const;
    virtual operator std::string() const;
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

    object operator[](std::size_t);
    object operator[](std::size_t) const;

    boost::optional<bool> as_bool() const;
    boost::optional<double> as_number() const;
    boost::optional<std::string> as_string() const;
    boost::optional<reference> as_ref() const;
};

EMEL_EXPORT std::ostream &operator <<(std::ostream &os, const object &arg);

} // namespace runtime

} // namespace emel
