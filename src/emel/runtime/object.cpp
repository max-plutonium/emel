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
#include <boost/algorithm/string.hpp>

namespace emel { namespace runtime {

array::array(object *ptr, std::size_t len)
{
    ptrs.first = new object[len];
    ptrs.second = ptrs.first + len;
    std::copy(ptr, ptr + len, ptrs.first);
}

array::array(const std::vector<object> &vec)
{
    ptrs.first = new object[vec.size()];
    ptrs.second = ptrs.first + vec.size();
    std::copy(vec.begin(), vec.end(), ptrs.first);
}

array::array(std::vector<object> &&vec)
{
    ptrs.first = new object[vec.size()];
    ptrs.second = ptrs.first + vec.size();
    std::move(vec.begin(), vec.end(), ptrs.first);
}

array::array(const array &other)
{
    const auto len = other.ptrs.second - other.ptrs.first;
    ptrs.first = new object[len];
    ptrs.second = ptrs.first + len;
    std::copy(other.ptrs.first, other.ptrs.second, ptrs.first);
}

array &array::operator =(const array &other)
{
    if(this != &other)
        array(other).swap(*this);
    return *this;
}

array::array(array &&other) : ptrs(nullptr, nullptr)
{
    std::swap(ptrs, other.ptrs);
}

array &array::operator =(array &&other)
{
    if(this != &other)
        array(std::move(other)).swap(*this);
    return *this;
}

array::~array()
{
    delete [] ptrs.first; ptrs.first = ptrs.second = nullptr;
}

void array::swap(array &other)
{
    std::swap(ptrs, other.ptrs);
}

std::size_t array::size() const
{
    return ptrs.second - ptrs.first;
}

bool array::empty() const
{
    return ptrs.second == ptrs.first;
}

std::vector<object> array::to_vector() const &
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = ptrs.first[i];
    return ret;
}

std::vector<object> array::to_vector() &&
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = std::move(ptrs.first[i]);
    return ret;
}

object &array::operator[](std::size_t i)
{
    return ptrs.first[i];
}

const object &array::operator[](std::size_t i) const
{
    return ptrs.first[i];
}

object::object() : t(is_empty), d(0)
{
}

object::object(empty_value_type) : t(is_empty), d(0)
{
}

object::object(array a) : t(is_array), a(std::move(a))
{
}

object::object(object *ptr, std::size_t len) : t(is_array), a(ptr, len)
{
}

object:: object(const std::vector<object> &vec) : t(is_array), a(vec)
{
}

object::object(std::vector<object> &&vec) : t(is_array), a(std::move(vec))
{

}

object::object(const std::string &s) : t(is_string), s(s)
{
}

object::object(const char *s) : t(is_string), s(s)
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

object::object(const object &other)
    : base(other), t(other.t), d(0)
{
    switch(t) {
        case is_empty: break;
        case is_array: new (&a) array(other.a); break;
        case is_string: new (&s) std::string(other.s); break;
        case is_number: new (&d) double(other.d); break;
        case is_boolean: new (&b) bool(other.b); break;
        case is_ref: new (&ref) reference(other.ref); break;
    }
}

object &object::operator =(const object &other)
{
    if(this != &other)
        object(other).swap(*this);
    return *this;
}

object::object(object &&other) : t(other.t), d(0)
{
    switch(t) {
        case is_empty: break;
        case is_array: new (&a) array(std::move(other.a)); break;
        case is_string: new (&s) std::string(std::move(other.s)); break;
        case is_number: new (&d) double(other.d); break;
        case is_boolean: new (&b) bool(other.b); break;
        case is_ref: new (&ref) reference(std::move(other.ref)); break;
    }

    other.d = 0;
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
    if(t == other.t) {
        switch(t) {
            case is_empty: break;
            case is_array: std::swap(a, other.a); break;
            case is_string: std::swap(s, other.s); break;
            case is_number: std::swap(d, other.d); break;
            case is_boolean: std::swap(b, other.b); break;
            case is_ref: std::swap(ref, other.ref); break;
        }

        return;
    }

    union U {
        array a;
        std::string s;
        double d;
        bool b;
        reference ref;
        U(const std::string &str) : s(str) { }
        ~U() { }
    } tmp("");

    switch(other.t) {
        case is_empty: break;
        case is_array: new (&tmp.a) array(std::move(other.a)); break;
        case is_string: new (&tmp.s) std::string(std::move(other.s)); break;
        case is_number: new (&tmp.d) double(other.d); break;
        case is_boolean: new (&tmp.b) bool(other.b); break;
        case is_ref: new (&tmp.ref) reference(std::move(other.ref)); break;
    }

    switch(t) {
        case is_empty: break;
        case is_array: new (&other.a) array(std::move(a)); break;
        case is_string: new (&other.s) std::string(std::move(s)); break;
        case is_number: new (&other.d) double(d); break;
        case is_boolean: new (&other.b) bool(b); break;
        case is_ref: new (&other.ref) reference(std::move(ref)); break;
    }

    switch(other.t) {
        case is_empty: break;
        case is_array: new (&a) array(std::move(tmp.a)); break;
        case is_string: new (&s) std::string(std::move(tmp.s)); break;
        case is_number: new (&d) double(tmp.d); break;
        case is_boolean: new (&b) bool(tmp.b); break;
        case is_ref: new (&ref) reference(std::move(tmp.ref)); break;
    }

    std::swap(t, other.t);
}

object &object::operator =(empty_value_type)
{
    return ((is_ref == t) ? *ref : *this) = object(empty_value);
}

object &object::operator =(array a)
{
    return ((is_ref == t) ? *ref : *this) = object(std::move(a));
}

object &object::operator =(const std::string &s)
{
    return ((is_ref == t) ? *ref : *this) = object(s);
}

object &object::operator=(const char *s)
{
    return ((is_ref == t) ? *ref : *this) = object(s);
}

object &object::operator =(double d)
{
    return ((is_ref == t) ? *ref : *this) = object(d);
}

object &object::operator =(bool b)
{
    return ((is_ref == t) ? *ref : *this) = object(b);
}

object &object::operator =(reference ref)
{
    return ((is_ref == t) ? *ref : *this) = object(ref);
}

object object::share()
{
    reference ret;

    switch(t) {
        case is_empty: return empty_value;
        case is_array: ret = make_object(std::move(a)); break;
        case is_string: ret = make_object(std::move(s)); break;
        case is_number: ret = make_object(std::move(d)); break;
        case is_boolean: ret = make_object(std::move(b)); break;
        case is_ref: return ref;
    }

    return *this = object(ret);
}

object object::clone()
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: return a; break;
        case is_string: return s; break;
        case is_number: return d; break;
        case is_boolean: return b; break;
        case is_ref: return ref->clone();
    }
}

object::~object()
{
    switch(t) {
        case is_empty: break;
        case is_array: a.~array(); break;
        case is_string: s.~basic_string(); break;
        case is_number: break;
        case is_boolean: break;
        case is_ref: ref.~reference(); break;
    }

    d = 0;
    t = is_empty;
}

object::operator std::string() const
{
    switch(t) {
        case is_empty: return std::string();
        case is_array:
            return std::string("array of ") + std::to_string(a.size())
            + " elements";

        case is_string: return s;
        case is_number: return boost::lexical_cast<std::string>(d);
        case is_boolean: return b ? "true" : "false";
        case is_ref: return ref->operator std::string();
    }

    return std::string();
}

object::operator double() const
{
    switch(t) {
        case is_empty: return 0.0;
        case is_array: return a.size();
        case is_string: {
            double res = 0.0;
            if(boost::conversion::try_lexical_convert<double>(s, res))
                return res;
            return std::numeric_limits<double>::quiet_NaN();
        }

        case is_number: return d;
        case is_boolean: return b ? 1 : 0;
        case is_ref: return ref->operator double();
    }

    return 0.0;
}

object::operator bool() const
{
    switch(t) {
        case is_empty: return false;
        case is_array: return !a.empty();
        case is_string: {
            auto str = s;
            boost::algorithm::trim(str);
            boost::algorithm::erase_all(str, " \n\t\r\0");
            return !str.empty() && !boost::iequals("false", str);
        }

        case is_number: return static_cast<bool>(d);
        case is_boolean: return b;
        case is_ref: return ref->operator bool();
    }

    return false;
}

object::operator reference() const
{
    switch(t) {
        case is_empty:
        case is_array:
        case is_string:
        case is_number:
        case is_boolean:
            return nullptr;

        case is_ref: return ref;
    }

    return nullptr;
}

bool object::operator ==(const object &other) const
{
    switch(t) {
        case is_empty: return other.get_type() == is_empty;
        case is_array: return false;
        case is_string: return s == static_cast<std::string>(other);
        case is_number: return d == static_cast<double>(other);
        case is_boolean: return b == static_cast<bool>(other);
        case is_ref: return ref->operator ==(other);
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
        case is_array: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) < 0;
        case is_number: return d < static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref->operator <(other);
    }

    return false;
}

bool object::operator >(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_array: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) > 0;
        case is_number: return d > static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref->operator >(other);
    }

    return false;
}

bool object::operator <=(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_array: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) <= 0;
        case is_number: return d <= static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref->operator <=(other);
    }

    return false;
}

bool object::operator >=(const object &other) const
{
    switch(t) {
        case is_empty: return false;
        case is_array: return false;
        case is_string: return s.compare(static_cast<std::string>(other)) >= 0;
        case is_number: return d >= static_cast<double>(other);
        case is_boolean: return false;
        case is_ref: return ref->operator >=(other);
    }

    return false;
}

object object::operator +(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: {
                std::vector<object> vec = a.to_vector();
                vec.push_back(other);
                return object(std::move(vec));
            }

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
        case is_array: {
                std::vector<object> vec = a.to_vector();
                for(auto i = vec.begin(); i != vec.end(); ++i)
                    if(*i == other) {
                        vec.erase(i);
                        break;
                    }

                return object(std::move(vec));
            }

        case is_string: {
                const auto str = static_cast<std::string>(other);
                const auto idx = s.find(str);
                if(idx != std::string::npos)
                    return std::string(s).erase(idx, str.size());
                return s;
            }

        case is_number: {
            double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? d : d - rhs;
        }

        case is_boolean: return b;
        case is_ref: return ref->operator -(other);
    }

    return empty_value;
}

object object::operator *(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: return *this;

        case is_string: {
                if(other.get_type() != is_number)
                    return s;
                int i = static_cast<double>(other);
                if(i < 1000) {
                    decltype(s) str;
                    str.reserve(s.size() * i);
                    while(i-- > 0)
                        str.append(s);
                    return str;
                }
                return s;
            }

        case is_number: {
            double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? d : d * rhs;
        }

        case is_boolean: return b;
        case is_ref: return ref->operator *(other);
    }

    return empty_value;
}

object object::operator /(const object &other) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: return *this;
        case is_string: return s;
        case is_number: {
            double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? d : d / rhs;
        }

        case is_boolean: return b;
        case is_ref: return ref->operator /(other);
    }

    return empty_value;
}

std::size_t object::size() const
{
    switch(t) {
        case is_empty: return 0;
        case is_array: return a.size();
        case is_string: return s.size();
        case is_number:
        case is_boolean:
            return 1;

        case is_ref: return ref->size();
    }

    return 0;
}

object object::operator[](std::size_t i)
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: return a[i].share();
        case is_string: return std::string(1, s[i]);
        case is_number: return d;
        case is_boolean: return b;
        case is_ref: return ref->operator[](i);
    }
}

object object::operator[](std::size_t i) const
{
    switch(t) {
        case is_empty: return empty_value;
        case is_array: return a[i];
        case is_string: return std::string(1, s[i]);
        case is_number: return d;
        case is_boolean: return b;
        case is_ref: return ref->operator[](i);
    }
}

optional<array> object::as_array() const
{
    optional<array> ret;
    if(is_array == t)
        ret = a;
    return ret;
}

optional<std::string> object::as_string() const
{
    optional<std::string> ret;
    if(is_string == t)
        ret = s;
    return ret;
}

optional<double> object::as_number() const
{
    optional<double> ret;
    if(is_number == t)
        ret = d;
    return ret;
}

optional<bool> object::as_bool() const
{
    optional<bool> ret;
    if(is_boolean == t)
        ret = b;
    return ret;
}

optional<reference> object::as_ref() const
{
    optional<reference> ret;
    if(is_ref == t)
        ret = ref;
    return ret;
}

std::ostream &operator <<(std::ostream &os, const object &arg)
{
    return os << arg.operator std::string();
}

} // namespace runtime

} // namespace emel
