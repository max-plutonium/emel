/*
 * Copyright (C) 2015, 2016 Max Plutonium <plutonium.max@gmail.com>
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
#include "../type-system/context.h"

#include <cstring>

namespace emel { namespace runtime {

object::object(type::rep d) : d(d)
{
}

object::object()
{
}

object::object(const std::vector<object> &vec) : d()
{
	std::vector<type::rep> reps;
	reps.reserve(vec.size());
	std::transform(vec.cbegin(), vec.cend(), std::back_inserter(reps),
		[](const object &o) { return o.d; });
	d.set(std::move(reps));
}

object::object(std::vector<object> &&vec) : d()
{
	std::vector<type::rep> reps;
	reps.reserve(vec.size());
	std::transform(vec.begin(), vec.end(), std::back_inserter(reps),
		[](object &o) { return std::move(o.d); });
	d.set(std::move(reps));
}

object::object(const std::string &s) : d(s)
{
}

object::object(const char *s) : d(s, std::strlen(s))
{
}

object::object(double num) : d(num)
{
}

object::object(std::int64_t i) : d(i)
{
}

object::object(bool b) : d(b)
{
}

object::object(reference ref) : d(ref)
{
}

object::~object()
{
}

object::object(const object &other) : object(other.d)
{
}

object &object::operator =(const object &other)
{
	if(this != &other)
		object(other).swap(*this);
    return *this;
}

object::object(object &&other) : object()
{
	std::swap(d, other.d);
}

object &object::operator =(object &&other)
{
	if(this != &other)
		object(std::move(other)).swap(*this);
    return *this;
}

void object::swap(object &other) noexcept
{
    std::swap(d, other.d);
}

void object::detach()
{
//	if(type::local != d.get_state())
//	{
//		void *ptr = nullptr;
//		const auto res = d.get(ptr);
//		assert(res);
//		auto *const ac = static_cast<memory::atomic_counted *>(ptr);
//		auto *const od = static_cast<object_data *>(ac->get());
//
//		bool maybe_local = true;
//		switch(d.get_kind())
//		{
//			case type::none: d.set(nullptr); break;
//			case type::bool_: d.set(od->get_bool()); break;
//			case type::int_: d.set(od->get_int()); break;
//			case type::num: d.set(od->get_num()); break;
//
//			case type::str: {
//				auto str = od->get_str();
//				if (str.size() <= type::rep::local_bytes_count) {
//					d.set(str);
//					break;
//				}
//
//				// fall through
//			}
//
//			default: maybe_local = false; break;
//		}
//
//		if(!maybe_local && !ac->unique()) {
//			d.set(ac->clone());
//			ac->release();
//		}
//	}
}

object &object::operator =(const std::string &s)
{
	d.set(s);
}

object &object::operator=(const char *s)
{
	d.set(s, std::strlen(s));
}

object &object::operator =(double num)
{
	d.set(num);
}

object &object::operator =(bool b)
{
	d.set(b);
}

object &object::operator =(reference ref)
{
	d.set(ref);
}

type::kind object::get_type() const
{
	return d.get_type()->get_kind();
}

bool object::empty() const
{
//	return d->empty();
}

std::size_t object::size() const
{
//	return d->size();
}

object::operator bool() const
{
//	return d->get_bool();
}

object::operator std::int64_t () const
{
//	return d->get_int();
}

object::operator double() const
{
//	return d->get_num();
}

object::operator std::string() const
{
//	return d->get_str();
}

object::operator reference() const
{
//    return d->get_ref();
}

bool object::operator ==(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none:
//			return other.get_type() == type::none;
//
//        case type::arr:
//			return false;
//
//        case type::str:
//			return d->get_str() == static_cast<std::string>(other);
//
//        case type::num:
//			return d->get_num() == static_cast<double>(other);
//
//        case type::bool_:
//			return d->get_bool() == static_cast<bool>(other);
//
//        case type::ptr:
//			return d->get_ref()->operator ==(other);
//    }

    return false;
}

bool object::operator !=(const object &other) const
{
    return !(*this == other);
}

bool object::operator <(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none: return false;
//        case type::arr: return false;
//
//        case type::str:
//			return d->get_str().compare(static_cast<std::string>(other)) < 0;
//
//        case type::num:
//			return d->get_num() < static_cast<double>(other);
//
//        case type::bool_: return false;
//
//        case type::ptr:
//			return d->get_ref()->operator <(other);
//    }

    return false;
}

bool object::operator >(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none: return false;
//        case type::arr: return false;
//
//        case type::str:
//			return d->get_str().compare(static_cast<std::string>(other)) > 0;
//
//        case type::num:
//			return d->get_num() > static_cast<double>(other);
//
//        case type::bool_: return false;
//
//        case type::ptr:
//			return d->get_ref()->operator >(other);
//    }

    return false;
}

bool object::operator <=(const object &other) const
{
//	switch(d.get_kind())
//	{
//		case type::none: return false;
//		case type::arr: return false;
//
//		case type::str:
//			return d->get_str().compare(static_cast<std::string>(other)) <= 0;
//
//		case type::num:
//			return d->get_num() <= static_cast<double>(other);
//
//		case type::bool_: return false;
//
//		case type::ptr:
//			return d->get_ref()->operator <=(other);
//	}

    return false;
}

bool object::operator >=(const object &other) const
{
//	switch(d.get_kind())
//	{
//		case type::none: return false;
//		case type::arr: return false;
//
//		case type::str:
//			return d->get_str().compare(static_cast<std::string>(other)) >= 0;
//
//		case type::num:
//			return d->get_num() >= static_cast<double>(other);
//
//		case type::bool_: return false;
//
//		case type::ptr:
//			return d->get_ref()->operator >=(other);
//	}

    return false;
}

object object::operator +(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none:
//			return object();
//
//        case type::arr: {
//			std::vector<object> vec; // TODO = boost::get<array>(d->u).to_vector();
//			vec.push_back(other);
//			return object(std::move(vec));
//		}
//
//        case type::str:
//			return d->get_str() + static_cast<std::string>(other);
//
//        case type::num:
//            if(other.get_type() == type::str)
//                return static_cast<std::string>(*this)
//                        + static_cast<std::string>(other);
//            return d->get_num() + static_cast<double>(other);
//
//        case type::bool_:
//			return d->get_bool();
//
//        case type::ptr:
//			return d->get_ref()->operator +(other);
//    }

    return object();
}

object object::operator -(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none:
//			return object();
//
//        case type::arr: {
//			std::vector<object> vec; // TODO = boost::get<array>(d->u).to_vector();
//			for(auto i = vec.begin(); i != vec.end(); ++i)
//				if(*i == other) {
//					vec.erase(i);
//					break;
//				}
//
//			return object(std::move(vec));
//		}
//
//        case type::str: {
//			auto str = d->get_str();
//			const auto ostr = static_cast<std::string>(other);
//			const auto idx = str.find(ostr);
//			if(idx != std::string::npos)
//				return str.erase(idx, ostr.size());
//			return str;
//		}
//
//        case type::num: {
//			const double lhs = d->get_num();
//            const double rhs = static_cast<double>(other);
//            return std::isnan(rhs) ? lhs : lhs - rhs;
//        }
//
//        case type::bool_:
//			return d->get_bool();
//
//        case type::ptr:
//			return d->get_ref()->operator -(other);
//    }

    return object();
}

object object::operator *(const object &other) const
{
//	switch(d.get_kind())
//	{
//        case type::none:
//			return object();
//
//        case type::arr:
//			return *this;
//
//        case type::str: {
//			const auto str = d->get_str();
//			if(other.get_type() != type::num)
//				return str;
//			int i = static_cast<int>(static_cast<double>(other));
//			if(i >= 0 && i < 1000) {
//				std::string new_str;
//				new_str.reserve(str.size() * i);
//				while(i-- > 0)
//					new_str.append(str);
//				return new_str;
//			}
//			return str;
//		}
//
//        case type::num: {
//			const double lhs = d->get_num();
//            const double rhs = static_cast<double>(other);
//            return std::isnan(rhs) ? lhs : lhs * rhs;
//        }
//
//        case type::bool_:
//			return d->get_bool();
//
//        case type::ptr:
//			return d->get_ref()->operator *(other);
//    }

    return object();
}

object object::operator /(const object &other) const
{
//    switch(d.get_kind())
//	{
//        case type::none:
//			return object();
//
//        case type::arr:
//			return *this;
//
//        case type::str:
//			return d->get_str();
//
//        case type::num: {
//			const double lhs = d->get_num();
//            const double rhs = static_cast<double>(other);
//            return std::isnan(rhs) ? lhs : lhs / rhs;
//        }
//
//        case type::bool_:
//			return d->get_bool();
//
//        case type::ptr:
//			return d->get_ref()->operator /(other);
//    }

    return object();
}

object object::operator[](std::size_t i)
{
//	return object(d->at(i));
}

object object::operator[](std::size_t i) const
{
//	return object(d->at(i));
}

boost::optional<bool> object::as_bool() const
{
    boost::optional<bool> ret;
//    if(type::bool_ == d.get_kind())
//		ret = d->get_bool();
    return ret;
}

boost::optional<double> object::as_number() const
{
    boost::optional<double> ret;
//    if(type::num == d.get_kind())
//		ret = d->get_num();
    return ret;
}

boost::optional<std::string> object::as_string() const
{
    boost::optional<std::string> ret;
//    if(type::str == d.get_kind())
//		ret = d->get_str();
    return ret;
}

boost::optional<reference> object::as_ref() const
{
    boost::optional<reference> ret;
//    if(type::ptr == d.get_kind())
//        ret = d->get_ref();
    return ret;
}

std::ostream &operator <<(std::ostream &os, const object &arg)
{
    return os << arg.operator std::string();
}

} // namespace runtime

} // namespace emel
