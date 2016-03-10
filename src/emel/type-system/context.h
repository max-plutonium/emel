/*
 * Copyright (C) 2016 Max Plutonium <plutonium.max@gmail.com>
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

#include "../opcodes.h"
#include "type.h"

#include <boost/container/small_vector.hpp>
#include <boost/flyweight.hpp>
//#include <sparsehash/dense_hash_map>
#include <unordered_map>

namespace emel { inline namespace type_system {

using offset_t = std::ptrdiff_t;
using offsets_vector = std::vector<offset_t, rt_allocator<offset_t>>;
using offsets_map = std::unordered_map<std::string, offset_t>;

using boost::container::small_vector;
using boost::container::small_vector_allocator;

struct context_info;
struct module_info;
struct type_info;
struct func_info;

enum class context_kind {
	module, type, func
};

class context_info_storage
{
	memory::counted_ptr impl;

	context_info_storage() = default;

public:
	static context_info_storage create();

	std::pair<memory_ptr<module_info>, bool>
		register_module(const std::string &name);
	std::pair<memory_ptr<module_info>, bool>
		get_module(const std::string &name);

	std::pair<memory_ptr<type_info>, bool>
		register_ti(type::kind k, const std::string &name);
	std::pair<memory_ptr<type_info>, bool>
		get_ti(const std::string &name);
};

struct object_data
{
	memory_ptr<context_info> whois;
	std::uint32_t length;
	std::uint8_t bytes[];
	insn_type code[];
};

struct context_info
{
	const context_kind k;
	std::bitset<32> bits;
	const boost::flyweight<std::string> name;
	std::ptrdiff_t offset;

	const memory_ptr<context_info> parent;
	offsets_vector types_offsets, fields_offsets, vars_offsets, funcs_offsets;
	offsets_map types_map, fields_map, vars_map, funcs_map;

	struct by_kind { };
	struct by_name { };
	struct by_parent_and_name { }; // parent + name
	struct by_parent_and_kind { }; // parent + kind
	struct by_parent_kind_name { }; // parent + kind + name
	struct by_parent_and_offset { }; // parent + offset

	context_info(context_kind k, boost::flyweight<std::string> name,
			const memory_ptr<context_info> &parent = { })
		: k(k), name(name), parent(parent)
	{ }
};

struct module_info : context_info
{
	offsets_vector mod_offsets;
	offsets_map mod_map;

	module_info(boost::flyweight<std::string> name,
			const memory_ptr<context_info> &parent = { })
		: context_info(context_kind::module, name, parent)
	{ }
};

struct type_info : context_info
{
	const type::kind k;

	type_info(type::kind k, boost::flyweight<std::string> name,
			  const memory_ptr<context_info> &parent = { })
		: context_info(context_kind::type, name, parent)
		, k(k)
	{ }
};

struct func_info : context_info
{
	const std::uint8_t nr_params;
	const std::uint32_t code_size, stack_size;

	func_info(boost::flyweight<std::string> name, std::uint8_t nr_params,
			  std::uint32_t code_size, std::uint32_t stack_size,
			  const memory_ptr<context_info> &parent = { })
		: context_info(context_kind::func, name, parent)
		, nr_params(nr_params), code_size(code_size), stack_size(stack_size)
	{ }
};

struct object_info
{
	const type *t;
	bool is_ref = false;
	bool is_const = false;
};

using char_type = char16_t;

struct string_data
{
	using string_type = std::basic_string<char_type,
		std::char_traits<char_type>, rt_allocator<char_type>>;

	string_type u;

	static inline auto get_alloc() {
		return rt_allocator<char_type>(memory::get_source());
	}

	explicit string_data(const char *str, std::size_t len);

	std::string get_str() const;
	void set(const char *utf8, std::size_t len);
	bool empty() const noexcept;
	std::size_t size() const noexcept;
	type::rep at(std::size_t i);
};

struct array_data
{
	small_vector<type::rep, 16, rt_allocator<type::rep>> a;

	static inline auto get_alloc() {
		return small_vector_allocator<rt_allocator<type::rep>>(
				memory::get_source(memory::collectable_pool));
	}

	explicit array_data(const std::vector<type::rep> &vec);
	explicit array_data(std::vector<type::rep> &&vec);

	std::vector<type::rep> get_arr() const;
	bool empty() const noexcept;
	std::size_t size() const noexcept;
	type::rep at(std::size_t i);
};

} // inline namespace type_system

} // namespace emel
