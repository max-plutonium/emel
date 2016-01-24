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
	: object(std::allocate_shared<data>(build_rt_alloc_for<data>()))
{
}

object::object(empty_value_type) : object()
{
}

object::object(object *ptr, std::size_t len)
	: object(std::allocate_shared<array::data>(
		build_rt_alloc_for<array::data>(), ptr, len))
{
}

object:: object(const std::vector<object> &vec)
	: object(std::allocate_shared<array::data>(
		build_rt_alloc_for<array::data>(), vec))
{
}

object::object(std::vector<object> &&vec)
	: object(std::allocate_shared<array::data>(
		build_rt_alloc_for<array::data>(), std::move(vec)))
{
}

object::object(const std::string &s)
	: object(std::allocate_shared<string::data>(
		build_rt_alloc_for<string::data>(), s))
{
}

object::object(const char *s)
	: object(std::allocate_shared<string::data>(
		build_rt_alloc_for<string::data>(), s))
{
}

object::object(double num)
	: object(std::allocate_shared<value_data>(
		build_rt_alloc_for<value_data>(), num))
{
}

object::object(bool b)
	: object(std::allocate_shared<value_data>(
		build_rt_alloc_for<value_data>(), b))
{
}

object::object(reference ref)
	: object(std::allocate_shared<value_data>(
		build_rt_alloc_for<value_data>(), ref))
{
}

object::~object()
{
    d = nullptr;
}

object::object(const object &other)
	: object(other.d)
{
}

object &object::operator =(const object &other)
{
	if(this != &other)
		object(other).swap(*this);
    return *this;
}

object::object(object &&other)
	: object()
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
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(empty_value);
}

object &object::operator =(const std::string &s)
{
	assert(d);
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(s);
}

object &object::operator=(const char *s)
{
	assert(d);
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(s);
}

object &object::operator =(double num)
{
	assert(d);
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(num);
}

object &object::operator =(bool b)
{
	assert(d);
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(b);
}

object &object::operator =(reference ref)
{
	assert(d);
    return ((is_ref == d->get_type())
			? *d->get_ref() : *this) = object(ref);
}

object::type object::get_type() const
{
	assert(d);
	return d->get_type();
}

bool object::empty() const
{
	assert(d);
	return d->empty();
}

std::size_t object::size() const
{
	assert(d);
	return d->size();
}

object object::clone()
{
	assert(d);
	return object(d);
}

void object::detach()
{
	assert(d);

	if(!d.unique())
		d = d->clone();
}

object::operator bool() const
{
	assert(d);
	return d->get_bool();
}

object::operator double() const
{
	assert(d);
	return d->get_num();
}

object::operator std::string() const
{
	assert(d);
	return d->get_str();
}

object::operator reference() const
{
	assert(d);
    return d->get_ref();
}

bool object::operator ==(const object &other) const
{
	assert(d);

    switch(d->get_type())
	{
        case is_empty:
			return other.get_type() == is_empty;

        case is_array:
			return false;

        case is_str:
			return d->get_str() == static_cast<std::string>(other);

        case is_num:
			return d->get_num() == static_cast<double>(other);

        case is_bool:
			return d->get_bool() == static_cast<bool>(other);

        case is_ref:
			return d->get_ref()->operator ==(other);
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

    switch(d->get_type())
	{
        case is_empty: return false;
        case is_array: return false;

        case is_str:
			return d->get_str().compare(static_cast<std::string>(other)) < 0;

        case is_num:
			return d->get_num() < static_cast<double>(other);

        case is_bool: return false;

        case is_ref:
			return d->get_ref()->operator <(other);
    }

    return false;
}

bool object::operator >(const object &other) const
{
	assert(d);

    switch(d->get_type())
	{
        case is_empty: return false;
        case is_array: return false;

        case is_str:
			return d->get_str().compare(static_cast<std::string>(other)) > 0;

        case is_num:
			return d->get_num() > static_cast<double>(other);

        case is_bool: return false;

        case is_ref:
			return d->get_ref()->operator >(other);
    }

    return false;
}

bool object::operator <=(const object &other) const
{
	assert(d);

	switch(d->get_type())
	{
		case is_empty: return false;
		case is_array: return false;

		case is_str:
			return d->get_str().compare(static_cast<std::string>(other)) <= 0;

		case is_num:
			return d->get_num() <= static_cast<double>(other);

		case is_bool: return false;

		case is_ref:
			return d->get_ref()->operator <=(other);
	}

    return false;
}

bool object::operator >=(const object &other) const
{
	assert(d);

	switch(d->get_type())
	{
		case is_empty: return false;
		case is_array: return false;

		case is_str:
			return d->get_str().compare(static_cast<std::string>(other)) >= 0;

		case is_num:
			return d->get_num() >= static_cast<double>(other);

		case is_bool: return false;

		case is_ref:
			return d->get_ref()->operator >=(other);
	}

    return false;
}

object object::operator +(const object &other) const
{
	assert(d);

    switch(d->get_type())
	{
        case is_empty:
			return empty_value;

        case is_array: {
			std::vector<object> vec; // TODO = boost::get<array>(d->u).to_vector();
			vec.push_back(other);
			return object(std::move(vec));
		}

        case is_str:
			return d->get_str() + static_cast<std::string>(other);

        case is_num:
            if(other.get_type() == is_str)
                return static_cast<std::string>(*this)
                        + static_cast<std::string>(other);
            return d->get_num() + static_cast<double>(other);

        case is_bool:
			return d->get_bool();

        case is_ref:
			return d->get_ref()->operator +(other);
    }

    return empty_value;
}

object object::operator -(const object &other) const
{
	assert(d);

    switch(d->get_type())
	{
        case is_empty:
			return empty_value;

        case is_array: {
			std::vector<object> vec; // TODO = boost::get<array>(d->u).to_vector();
			for(auto i = vec.begin(); i != vec.end(); ++i)
				if(*i == other) {
					vec.erase(i);
					break;
				}

			return object(std::move(vec));
		}

        case is_str: {
			auto str = d->get_str();
			const auto ostr = static_cast<std::string>(other);
			const auto idx = str.find(ostr);
			if(idx != std::string::npos)
				return str.erase(idx, ostr.size());
			return str;
		}

        case is_num: {
			const double lhs = d->get_num();
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs - rhs;
        }

        case is_bool:
			return d->get_bool();

        case is_ref:
			return d->get_ref()->operator -(other);
    }

    return empty_value;
}

object object::operator *(const object &other) const
{
	assert(d);

	switch(d->get_type())
	{
        case is_empty:
			return empty_value;

        case is_array:
			return *this;

        case is_str: {
			const auto str = d->get_str();
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
			const double lhs = d->get_num();
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs * rhs;
        }

        case is_bool:
			return d->get_bool();

        case is_ref:
			return d->get_ref()->operator *(other);
    }

    return empty_value;
}

object object::operator /(const object &other) const
{
	assert(d);

    switch(d->get_type())
	{
        case is_empty:
			return empty_value;

        case is_array:
			return *this;

        case is_str:
			return d->get_str();

        case is_num: {
			const double lhs = d->get_num();
            const double rhs = static_cast<double>(other);
            return std::isnan(rhs) ? lhs : lhs / rhs;
        }

        case is_bool:
			return d->get_bool();

        case is_ref:
			return d->get_ref()->operator /(other);
    }

    return empty_value;
}

object object::operator[](std::size_t i)
{
	assert(d);
	return object(d->at(i));
}

object object::operator[](std::size_t i) const
{
	assert(d);
	return object(d->at(i));
}

boost::optional<bool> object::as_bool() const
{
	assert(d);
    boost::optional<bool> ret;
    if(is_bool == d->get_type())
		ret = d->get_bool();
    return ret;
}

boost::optional<double> object::as_number() const
{
	assert(d);
    boost::optional<double> ret;
    if(is_num == d->get_type())
		ret = d->get_num();
    return ret;
}

boost::optional<std::string> object::as_string() const
{
	assert(d);
    boost::optional<std::string> ret;
    if(is_str == d->get_type())
		ret = d->get_str();
    return ret;
}

boost::optional<reference> object::as_ref() const
{
	assert(d);
    boost::optional<reference> ret;
    if(is_ref == d->get_type())
        ret = d->get_ref();
    return ret;
}

std::ostream &operator <<(std::ostream &os, const object &arg)
{
    return os << arg.operator std::string();
}

} // namespace runtime

} // namespace emel
