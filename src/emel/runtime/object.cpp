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
#include "object.h"

#include <boost/lexical_cast.hpp>

namespace emel { namespace runtime {

object::object() : t(is_empty)
{
}

object::object(empty_value_type) : t(is_empty)
{
}

object::object(const std::string &s) : t(is_string), s(s)
{
}

object::object(double d) : t(is_number), d(d)
{
}

object::object(bool b) : t(is_boolean), b(b)
{
}

object::object(reference ref) : t(is_ref), ref(ref)
{
}

object::object(const object &other) : base(other), t(other.t)
{
    switch(t) {
        case is_empty: break;
        case is_string: s = std::string(); s = other.s; break;
        case is_number: d = other.d; break;
        case is_boolean: b = other.b; break;
        case is_ref: ref = other.ref; break; // BUG падает
    }
}

object &object::operator =(const object &other)
{
    if(this != &other)
        object(other).swap(*this);
    return *this;
}

object &object::operator =(const std::string &s)
{
    return *this = object(s);
}

object &object::operator =(double d)
{
    return *this = object(d);
}

object &object::operator =(bool b)
{
    return *this = object(b);
}

object &object::operator =(reference ref)
{
    return *this = object(ref);
}

object::object(object &&other) : t(other.t)
{
    switch(t) {
        case is_empty: break;
        case is_string: s = std::move(other.s); break;
        case is_number: d = other.d; break;
        case is_boolean: b = other.b; break;
        case is_ref: ref = std::move(other.ref); break;
    }

    other.t = is_empty;
}

object &object::operator =(object &&other)
{
    if(this != &other)
        object(std::move(other)).swap(*this);
    return *this;
}

void object::swap(object &other)
{
    if(t == other.t)
        return;

    switch(other.t) {
        case is_empty: break;
        case is_string: s = std::move(other.s); break;
        case is_number: d = other.d; break;
        case is_boolean: b = other.b; break;
        case is_ref: ref = std::move(other.ref); break;
    }

    switch(t) {
        case is_empty: break;
        case is_string: other.s = std::move(s); break;
        case is_number: other.d = d; break;
        case is_boolean: other.b = b; break;
        case is_ref: other.ref = std::move(ref); break;
    }

    std::swap(t, other.t);
}

object::operator std::string() const {
    switch(t) {
        case is_empty: return std::string();
        case is_string: return s;
        case is_number: return boost::lexical_cast<std::string>(d);
        case is_boolean: return b ? "true" : "false";
        case is_ref: return std::string("object ")
                    + boost::lexical_cast<std::string>(ref.get());
    }

    return std::string();
}

object::operator double() const {
    switch(t) {
        case is_empty: return 0.0;
        case is_string: return boost::lexical_cast<double>(s);
        case is_number: return d;
        case is_boolean: return b ? 1 : 0;
        case is_ref: return ref->operator double();
    }

    return 0;
}

object::operator bool() const {
    switch(t) {
        case is_empty: return false;
        case is_string: return !s.empty();
        case is_number: return static_cast<bool>(d);
        case is_boolean: return b;
        case is_ref: return ref->operator bool();
    }

    return false;
}

object::operator reference() {
    switch(t) {
        case is_empty:
        case is_string:
        case is_number:
        case is_boolean:
            return this->shared_from_this();

        case is_ref: return ref;
    }

    return nullptr;
}

object::operator const_reference() const
{
    switch(t) {
        case is_empty:
        case is_string:
        case is_number:
        case is_boolean:
            return this->shared_from_this();

        case is_ref: return ref;
    }

    return nullptr;
}

bool object::operator ==(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_string: return s == static_cast<std::string>(other);
        case is_number: return d == static_cast<double>(other);
        case is_boolean: return b == static_cast<bool>(other);
        case is_ref: return ref == static_cast<const_reference>(other);
    }

    return false;
}

bool object::operator !=(const object &other) const
{
    return !(*this == other);
}

bool object::operator <(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) < 0;
        case is_number: return d < static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref.get() < static_cast<const_reference>(other).get();
    }

    return false;
}

bool object::operator >(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) > 0;
        case is_number: return d > static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref.get() > static_cast<const_reference>(other).get();
    }

    return false;
}

bool object::operator <=(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) <= 0;
        case is_number: return d <= static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref.get() <= static_cast<const_reference>(other).get();
    }

    return false;
}

bool object::operator >=(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) >= 0;
        case is_number: return d >= static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref.get() >= static_cast<const_reference>(other).get();
    }

    return false;
}

object object::operator +(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_string: return s + static_cast<std::string>(other);
        case is_number:
            if(other.get_type() == is_string)
                return static_cast<std::string>(*this)
                        + static_cast<std::string>(other);
            return d + static_cast<double>(other);
        case is_boolean: return b;
        case is_ref: return ref->operator +(other);
    }

    return empty_value;
}

object object::operator -(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_string: {
                const auto str = static_cast<std::string>(other);
                const auto idx = s.find_first_of(str);
                if(idx != std::string::npos)
                    return std::string(s).erase(idx, str.size());
                return s;
            }
        case is_number: return d - static_cast<double>(other);
        case is_boolean: return b;
        case is_ref: return ref->operator -(other);
    }

    return empty_value;
}

object object::operator *(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_string: {
                if(other.get_type() != is_number)
                    return s;
                auto i = static_cast<double>(other);
                if(i < 1000) {
                    auto str = s;
                    str.reserve(str.size() * i);
                    while(i-- > 0)
                        str = str.append(s);
                }
                return s;
            }
        case is_number: return d * static_cast<double>(other);
        case is_boolean: return b;
        case is_ref: return ref->operator *(other);
    }

    return empty_value;
}

object object::operator /(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_string: return s;
        case is_number: return d / static_cast<double>(other); // TODO div by zero
        case is_boolean: return b;
        case is_ref: return ref->operator /(other);
    }

    return empty_value;
}

} // namespace runtime

} // namespace emel
