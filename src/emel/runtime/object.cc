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
#include "object-data.h"

#include <boost/algorithm/string.hpp>
#include <boost/convert.hpp>
#include <boost/convert/spirit.hpp>

struct boost::cnv::by_default : boost::cnv::spirit { };

namespace emel { namespace runtime {

void *object::operator new(std::size_t size)
{
	return memory::alloc<object>(size);
}

void object::operator delete(void *ptr)
{
	memory::gc::deallocate(ptr);
}

object::object(std::shared_ptr<data> d) : d(std::move(d))
{
	assert(this->d);
}

object::object()
	: object(std::allocate_shared<data>(memory::rt_allocator<data>()))
{
}

object::object(empty_value_type) : object()
{
}

object::object(array a) : object()
{
	d->u = std::move(a);
}

object::object(object *ptr, std::size_t len) : object()
{
	d->u = array(ptr, len);
}

object:: object(const std::vector<object> &vec) : object()
{
	d->u = array(vec);
}

object::object(std::vector<object> &&vec) : object()
{
	d->u = array(std::move(vec));
}

object::object(const std::string &s) : object()
{
	d->u = s;
}

object::object(const char *s) : object()
{
	d->u = std::string(s);
}

object::object(double num) : object()
{
	d->u = num;
}

object::object(bool b) : object()
{
	d->u = b;
}

object::object(reference ref) : object()
{
	d->u = ref;
}

object::object(const object &other) : object()
{
	assert(other.d);
	d->u = other.d->u;
}

object &object::operator =(const object &other)
{
	if(this != &other)
		object(other).swap(*this);
    return *this;
}

object::object(object &&other) : object()
{
	assert(other.d);
	std::swap(d, other.d);
}

object &object::operator =(object &&other)
{
	if(this != &other)
		object(std::move(other)).swap(*this);
    return *this;
}

void object::swap(object &other)
{
    std::swap(d, other.d);
}

object &object::operator =(empty_value_type)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(empty_value);
}

object &object::operator =(array a)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(std::move(a));
}

object &object::operator =(const std::string &s)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(s);
}

object &object::operator=(const char *s)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(s);
}

object &object::operator =(double num)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(num);
}

object &object::operator =(bool b)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(b);
}

object &object::operator =(reference ref)
{
	assert(d);
    return ((is_ref == d->u.which())
			? *boost::get<reference>(d->u) : *this) = object(ref);
}

object::type object::get_type() const
{
	assert(d);
	return type(d->u.which());
}

bool object::empty() const
{
	assert(d);
	return type(is_empty == d->u.which());
}

object object::clone()
{
	assert(d);
	return object(d);
}

void object::detach()
{
	assert(d);

	if(!d.unique()) {
		auto old_d = std::move(d);
		d = std::allocate_shared<data>(memory::rt_allocator<data>());
		d->u = old_d->u;
	}
}

object::~object()
{
    d = nullptr;
}

object::operator std::string() const
{
	assert(d);

    switch(d->u.which())
	{
		case is_empty:
			return std::string();

        case is_array: {
			const array &a = boost::get<array>(d->u);
			return std::string("array of ") + std::to_string(a.size())
				   + " elements";
		}

        case is_str:
			return boost::get<std::string>(d->u);

        case is_num: {
			const double &num = boost::get<double>(d->u);
			return boost::convert<std::string>(num).value();
		}

        case is_bool:
			return boost::get<bool>(d->u) ? "true" : "false";

        case is_ref:
			return boost::get<reference>(d->u)->operator std::string();
    }

    return std::string();
}

object::operator double() const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty:
			return 0.0;

        case is_array:
			return boost::get<array>(d->u).size();

        case is_str:
            return boost::convert<double>(boost::get<std::string>(d->u))
                    .value_or(std::numeric_limits<double>::quiet_NaN());

        case is_num:
			return boost::get<double>(d->u);

        case is_bool:
			return boost::get<bool>(d->u) ? 1.0 : 0.0;

        case is_ref:
			return boost::get<reference>(d->u)->operator double();
    }

    return 0.0;
}

object::operator bool() const
{
	assert(d);

	switch(d->u.which())
	{
        case is_empty:
			return false;

        case is_array: {
			const array &a = boost::get<array>(d->u);
			return !a.empty();
		}

        case is_str: {
            auto str = boost::get<std::string>(d->u);
            boost::algorithm::trim(str);
            //boost::algorithm::erase_all(str, " \n\t\r\0");
            return !str.empty() && !boost::iequals("false", str);
        }

        case is_num:
			return static_cast<bool>(boost::get<double>(d->u));

        case is_bool:
			return boost::get<bool>(d->u);

        case is_ref:
			return boost::get<reference>(d->u)->operator bool();
    }

    return false;
}

object::operator reference() const
{
	assert(d);

	if(is_ref == d->u.which())
		return boost::get<reference>(d->u);

    return nullptr;
}

bool object::operator ==(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty:
			return other.get_type() == is_empty;

        case is_array:
			return false;

        case is_str:
			return boost::get<std::string>(d->u) == static_cast<std::string>(other);

        case is_num:
			return boost::get<double>(d->u) == static_cast<double>(other);

        case is_bool:
			return boost::get<bool>(d->u) == static_cast<bool>(other);

        case is_ref:
			return boost::get<reference>(d->u)->operator ==(other);
    }

    return false;
}

bool object::operator !=(const object &other) const
{
    return !(*this == other);
}

bool object::operator <(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty: return false;
        case is_array: return false;

        case is_str:
			return boost::get<std::string>(d->u).compare(static_cast<std::string>(other)) < 0;

        case is_num:
			return boost::get<double>(d->u) < static_cast<double>(other);

        case is_bool: return false;

        case is_ref:
			return boost::get<reference>(d->u)->operator <(other);
    }

    return false;
}

bool object::operator >(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty: return false;
        case is_array: return false;

        case is_str:
			return boost::get<std::string>(d->u).compare(static_cast<std::string>(other)) > 0;

        case is_num:
			return boost::get<double>(d->u) > static_cast<double>(other);

        case is_bool: return false;

        case is_ref:
			return boost::get<reference>(d->u)->operator >(other);
    }

    return false;
}

bool object::operator <=(const object &other) const
{
	assert(d);

	switch(d->u.which())
	{
		case is_empty: return false;
		case is_array: return false;

		case is_str:
			return boost::get<std::string>(d->u).compare(static_cast<std::string>(other)) <= 0;

		case is_num:
			return boost::get<double>(d->u) <= static_cast<double>(other);

		case is_bool: return false;

		case is_ref:
			return boost::get<reference>(d->u)->operator <=(other);
	}

    return false;
}

bool object::operator >=(const object &other) const
{
	assert(d);

	switch(d->u.which())
	{
		case is_empty: return false;
		case is_array: return false;

		case is_str:
			return boost::get<std::string>(d->u).compare(static_cast<std::string>(other)) >= 0;

		case is_num:
			return boost::get<double>(d->u) >= static_cast<double>(other);

		case is_bool: return false;

		case is_ref:
			return boost::get<reference>(d->u)->operator >=(other);
	}

    return false;
}

object object::operator +(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty:
			return empty_value;

        case is_array: {
			std::vector<object> vec = boost::get<array>(d->u).to_vector();
			vec.push_back(other);
			return object(std::move(vec));
		}

        case is_str:
			return boost::get<std::string>(d->u) + static_cast<std::string>(other);

        case is_num:
            if(other.get_type() == is_str)
                return static_cast<std::string>(*this)
                        + static_cast<std::string>(other);
            return boost::get<double>(d->u) + static_cast<double>(other);

        case is_bool:
			return boost::get<bool>(d->u);

        case is_ref:
			return boost::get<reference>(d->u)->operator +(other);
    }

    return empty_value;
}

object object::operator -(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty:
			return empty_value;

        case is_array: {
			std::vector<object> vec = boost::get<array>(d->u).to_vector();
			for(auto i = vec.begin(); i != vec.end(); ++i)
				if(*i == other) {
					vec.erase(i);
					break;
				}

			return object(std::move(vec));
		}

        case is_str: {
			auto str = boost::get<std::string>(d->u);
			const auto ostr = static_cast<std::string>(other);
			const auto idx = str.find(ostr);
			if(idx != std::string::npos)
				return str.erase(idx, ostr.size());
			return str;
		}

        case is_num: {
			const double lhs = boost::get<double>(d->u);
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs - rhs;
        }

        case is_bool:
			return boost::get<bool>(d->u);

        case is_ref:
			return boost::get<reference>(d->u)->operator -(other);
    }

    return empty_value;
}

object object::operator *(const object &other) const
{
	assert(d);

	switch(d->u.which())
	{
        case is_empty:
			return empty_value;

        case is_array:
			return *this;

        case is_str: {
			const auto str = boost::get<std::string>(d->u);
			if(other.get_type() != is_num)
				return str;
			int i = static_cast<int>(static_cast<double>(other));
			if(i < 1000) {
				std::string new_str;
				new_str.reserve(str.size() * i);
				while(i-- > 0) // TODO test for i == 1
					new_str.append(str);
				return new_str;
			}
			return str;
		}

        case is_num: {
			const double lhs = boost::get<double>(d->u);
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs * rhs;
        }

        case is_bool:
			return boost::get<bool>(d->u);

        case is_ref:
			return boost::get<reference>(d->u)->operator *(other);
    }

    return empty_value;
}

object object::operator /(const object &other) const
{
	assert(d);

    switch(d->u.which())
	{
        case is_empty:
			return empty_value;

        case is_array:
			return *this;

        case is_str:
			return boost::get<std::string>(d->u);

        case is_num: {
			const double lhs = boost::get<double>(d->u);
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs / rhs;
        }

        case is_bool:
			return boost::get<bool>(d->u);

        case is_ref:
			return boost::get<reference>(d->u)->operator /(other);
    }

    return empty_value;
}

std::size_t object::size() const
{
	assert(d);

    switch(d->u.which()) {
        case is_empty: return 0;
        case is_array: return boost::get<array>(d->u).size();
        case is_str: return boost::get<std::string>(d->u).size();
        case is_num:
        case is_bool:
            return 1;

        case is_ref: return boost::get<reference>(d->u)->size();
    }

    return 0;
}

object object::operator[](std::size_t i)
{
	assert(d);

	switch(d->u.which()) {
        case is_empty: return empty_value;
        case is_array: return boost::get<array>(d->u)[i];
        case is_str: return std::string(1, boost::get<std::string>(d->u)[i]);
        case is_num: return boost::get<double>(d->u);
        case is_bool: return boost::get<bool>(d->u);
        case is_ref: return boost::get<reference>(d->u)->operator[](i);
    }

	return empty_value;
}

object object::operator[](std::size_t i) const
{
	assert(d);

	switch(d->u.which()) {
		case is_empty: return empty_value;
		case is_array: return boost::get<array>(d->u)[i];
		case is_str: return std::string(1, boost::get<std::string>(d->u)[i]);
		case is_num: return boost::get<double>(d->u);
		case is_bool: return boost::get<bool>(d->u);
		case is_ref: return boost::get<reference>(d->u)->operator[](i);
	}

	return empty_value;
}

boost::optional<array> object::as_array() const
{
	assert(d);
    boost::optional<array> ret;
    if(is_array == d->u.which())
        ret = boost::get<array>(d->u);
    return ret;
}

boost::optional<std::string> object::as_string() const
{
	assert(d);
    boost::optional<std::string> ret;
    if(is_str == d->u.which())
        ret = boost::get<std::string>(d->u);
    return ret;
}

boost::optional<double> object::as_number() const
{
	assert(d);
    boost::optional<double> ret;
    if(is_num == d->u.which())
        ret = boost::get<double>(d->u);
    return ret;
}

boost::optional<bool> object::as_bool() const
{
	assert(d);
    boost::optional<bool> ret;
    if(is_bool == d->u.which())
        ret = boost::get<bool>(d->u);
    return ret;
}

boost::optional<reference> object::as_ref() const
{
	assert(d);
    boost::optional<reference> ret;
    if(is_ref == d->u.which())
        ret = boost::get<reference>(d->u);
    return ret;
}

std::ostream &operator <<(std::ostream &os, const object &arg)
{
    return os << arg.operator std::string();
}

} // namespace runtime

} // namespace emel
