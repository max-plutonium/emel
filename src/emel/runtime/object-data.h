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

#include "../memory/gc.h"
#include "../memory/pool.h"

#include "object.h"
#include "string.h"
#include "array.h"

#include <codecvt>
#include <locale>

#include <boost/algorithm/string.hpp>
#include <boost/convert.hpp>
#include <boost/convert/spirit.hpp>

struct boost::cnv::by_default : boost::cnv::spirit { };

namespace emel { namespace runtime {

template <typename Tp>
inline memory::rt_allocator<Tp> build_rt_alloc_for()
{
	return memory::rt_allocator<Tp>(memory::pool::get_source());
}

struct object::data
{
	virtual ~data() = default;

	enum {
		is_unique = 0,
		is_shared = 1
	};

	enum {
		is_const = 0,
		is_mutable = 1
	};

	enum {
		is_value = 0,
		is_reference = 1
	};

	enum {
		is_primitive = 0,
		is_composite = 1
	};

	enum {
		is_empty = 0,
		is_single = 1,
		is_array = 2
	};

	std::uint32_t us : 1;
	std::uint32_t cm : 1;
	std::uint32_t vr : 1;
	std::uint32_t pc : 1;
	std::uint32_t esa : 2;

	data(std::uint32_t us = is_unique, std::uint32_t cm = is_const,
		 std::uint32_t vr = is_value, std::uint32_t pc = is_primitive,
		 std::uint32_t esa = is_empty)
		: us(us), cm(cm), vr(vr), pc(pc), esa(esa)
	{
	}

	virtual object::type get_type() const
	{
		return object::is_empty;
	}

	virtual std::shared_ptr<data> clone() const
	{
		return std::allocate_shared<data>(build_rt_alloc_for<data>(), *this);
	}

	virtual bool get_bool() const
	{
		return false;
	}

	virtual double get_num() const
	{
		return 0.0;
	}

	virtual std::string get_str() const
	{
		return std::string();
	}

	virtual reference get_ref() const
	{
		return nullptr;
	}

	virtual bool empty() const
	{
		return true;
	}

	virtual std::size_t size() const
	{
		return 0;
	}

	virtual object at(std::size_t)
	{
		return object();
	}
};

struct value_data : object::data
{
	enum {
		is_bool = 0,
		is_num = 1,
		is_ref = 2
	};

	std::uint32_t bn : 2;

	union {
		bool b;
		double n;
		reference o;
	};

	value_data()
		: object::data(is_unique, is_const, is_value,
			is_primitive, is_single)
		, bn(is_num)
		, n(0.0)
	{
		auto sz = sizeof(*this);
		return;
	}

	explicit value_data(bool b)
		: object::data(is_unique, is_const, is_value,
			is_primitive, is_single)
		, bn(is_bool)
		, b(b)
	{
	}

	explicit value_data(double num)
		: object::data(is_unique, is_const, is_value,
			is_primitive, is_single)
		, bn(is_num)
		, n(num)
	{
	}

	explicit value_data(reference o)
		: object::data(is_unique, is_const, is_value,
			is_primitive, is_single)
		, bn(is_ref)
		, o(o)
	{
	}

	virtual object::type get_type() const override
	{
		switch(bn) {
			case is_bool: return object::is_bool;
			case is_num: return object::is_num;
			case is_ref: return object::is_ref;
		}

		assert(false);
		return object::data::get_type();
	}

	virtual std::shared_ptr<object::data> clone() const override
	{
		return std::allocate_shared<value_data>(build_rt_alloc_for<value_data>(), *this);
	}

	virtual bool get_bool() const override
	{
		switch(bn) {
			case is_bool: return b;
			case is_num: return static_cast<bool>(n);
			case is_ref: return o->operator bool();
		}

		assert(false);
		return object::data::get_bool();
	}

	virtual double get_num() const override
	{
		switch(bn) {
			case is_bool: return b ? 1.0 : 0.0;
			case is_num: return n;
			case is_ref: return o->operator double();
		}

		assert(false);
		return object::data::get_num();
	}

	virtual std::string get_str() const override
	{
		switch(bn) {
			case is_bool: return b ? "true" : "false";
			case is_num: return boost::convert<std::string>(n).value();
			case is_ref: return o->operator std::string();
		}

		assert(false);
		return object::data::get_str();
	}

	virtual bool empty() const override
	{
		if(is_ref == bn)
			return o->empty();
		return false;
	}

	virtual std::size_t size() const override
	{
		if(is_ref == bn)
			return o->size();
		return 1;
	}

	virtual object at(std::size_t i) override
	{
		if(is_ref == bn)
			return o->operator [](i);
		return object::data::at(i);
	}
};

struct string_codec : std::codecvt<character::type, char, std::mbstate_t> { };

struct string::data : object::data
{
	using string_type = std::basic_string<character::type,
		std::char_traits<character::type>,
		memory::rt_allocator<character::type>>;

	using conv_type = std::wstring_convert<string_codec,
		character::type, memory::rt_allocator<character::type>>;

	string_type u;

	data()
		: object::data(is_unique, is_const, is_value,
			is_composite, is_single)
	{
	}

	explicit data(character::type c)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_single)
		, u(1, c)
	{
	}

	explicit data(const char *str)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_single)
	{
		set(str);
	}

	explicit data(const std::string &str)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_single)
	{
		set(str);
	}

	explicit data(const string_type &str)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_single)
		, u(str)
	{
	}

	virtual object::type get_type() const override
	{
		return object::is_str;
	}

	virtual std::shared_ptr<object::data> clone() const override
	{
		return std::allocate_shared<data>(build_rt_alloc_for<data>(), *this);
	}

	virtual bool get_bool() const override
	{
		auto str = get_str();
		boost::algorithm::trim(str);
		//boost::algorithm::erase_all(str, " \n\t\r\0");
		return !str.empty() && !boost::iequals("false", str);
	}

	virtual double get_num() const override
	{
		return boost::convert<double>(get_str())
			.value_or(std::numeric_limits<double>::quiet_NaN());
	}

	virtual std::string get_str() const override
	{
		conv_type converter;
		return converter.to_bytes(u.c_str());
	}

	void set(const char *utf8)
	{
		conv_type converter;
		u.assign(converter.from_bytes(utf8));
	}

	void set(const std::string &utf8)
	{
		conv_type converter;
		u.assign(converter.from_bytes(utf8.c_str()));
	}

	virtual bool empty() const override
	{
		return u.empty();
	}

	virtual std::size_t size() const override
	{
		return u.size();
	}

	virtual object at(std::size_t i) override
	{
		conv_type converter;
		return converter.to_bytes(u.at(i));
	}
};

struct array::data : object::data
{
	std::deque<object, memory::rt_allocator<object>> a;

	data()
		: object::data(is_unique, is_const, is_value,
			is_composite, is_array)
	{
		auto sz = sizeof(*this);
		return;
	}

	data(object *ptr, std::size_t len)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_array)
		, a(ptr, ptr + len)
	{
	}

	explicit data(const std::vector<object> &vec)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_array)
		, a(vec.begin(), vec.end())
	{
	}

	explicit data(std::vector<object> &&vec)
		: object::data(is_unique, is_const, is_value,
			is_composite, is_array)
	{
		for(auto &v : vec)
			a.push_back(std::move(v));
	}

	virtual object::type get_type() const override
	{
		return object::is_array;
	}

	virtual std::shared_ptr<object::data> clone() const override
	{
		return std::allocate_shared<data>(build_rt_alloc_for<data>(), *this);
	}

	virtual bool get_bool() const override
	{
		return !a.empty();
	}

	virtual double get_num() const override
	{
		return a.size();
	}

	virtual std::string get_str() const override
	{
		return std::string("array of ") + std::to_string(a.size())
			   + " elements";
	}

	virtual bool empty() const override
	{
		return a.empty();
	}

	virtual std::size_t size() const override
	{
		return a.size();
	}

	virtual object at(std::size_t i) override
	{
		return a.at(i);
	}
};

} // namespace runtime

} // namespace emel
