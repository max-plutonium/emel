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
#include "context.h"

#include <boost/algorithm/string/split.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

#include <codecvt>
#include <locale>

namespace emel { inline namespace type_system {

namespace mi = boost::multi_index;

#define MEMBER(Type, MemberName) \
	BOOST_MULTI_INDEX_MEMBER(Type, decltype(std::declval<Type>().MemberName), MemberName)

using context_info_table = mi::multi_index_container
<
	memory_ptr<context_info>,
	mi::indexed_by
	<
//		mi::hashed_non_unique<mi::tag<context_info::by_kind>, MEMBER(context_info, k)>,
//		mi::hashed_non_unique<mi::tag<context_info::by_name>, MEMBER(context_info, name)>,
//		mi::hashed_unique
//		<
//			mi::tag<context_info::by_parent_and_name>,
//			mi::composite_key<context_info, MEMBER(context_info, parent), MEMBER(context_info, name)>
//		>,
//		mi::hashed_non_unique
//		<
//			mi::tag<context_info::by_parent_and_kind>,
//			mi::composite_key<context_info, MEMBER(context_info, parent), MEMBER(context_info, k)>
//		>,
		mi::hashed_unique
		<
			mi::tag<context_info::by_parent_kind_name>,
			mi::composite_key<context_info, MEMBER(context_info, parent),
				MEMBER(context_info, k), MEMBER(context_info, name)>
		>/*,
		mi::hashed_unique
		<
			mi::tag<context_info::by_parent_and_offset>,
			mi::composite_key<context_info, MEMBER(context_info, parent), MEMBER(context_info, offset)>
		>*/
	>
>;

#undef MEMBER

/*static*/
context_info_storage context_info_storage::create()
{
	context_info_storage ret;
	ret.impl.reset(memory::allocate_counted<context_info_table>(
		rt_allocator<char>(memory::get_source(memory::fast_pool))), false);
	return ret;
}

static memory_ptr<context_info> find_context(
	const context_info_table &table, context_kind k,
		const std::vector<boost::flyweight<std::string>> &path)
{
	memory_ptr<context_info> ret;

	const auto &module_key = table.get<context_info::by_parent_kind_name>();

	for(const auto &path_entry : path) {
		auto it = module_key.find(std::make_tuple(ret, k, path_entry));
		if(module_key.end() == it)
			return nullptr;
		ret = *it;
	}

	return ret;
}

static std::vector<boost::flyweight<std::string>>
split_path(const std::string &path_string, char delim = '/')
{
	std::vector<boost::flyweight<std::string>> path;

	boost::algorithm::split(path, path_string,
		[delim](char c) -> bool { return delim == c; });

	return path;
}

std::pair<memory_ptr<module_info>, bool>
context_info_storage::register_module(const std::string &name)
{
	context_info_table &table = *impl->get<context_info_table>();

	auto path = split_path(name);
	auto str = std::move(path.back());
	path.pop_back();

	auto context_ptr = find_context(table, context_kind::module, path);

	if(!context_ptr && !path.empty())
		return std::make_pair(nullptr, false);

	auto pair = table.emplace(memory::allocate_counted<module_info>(
		rt_allocator<char>(memory::get_source(memory::fast_pool)),
			std::move(str), context_ptr), false);

	return std::make_pair(*pair.first, pair.second);
}

std::pair<memory_ptr<module_info>, bool>
context_info_storage::get_module(const std::string &name)
{
	context_info_table &table = *impl->get<context_info_table>();
	auto module_ptr = find_context(table, context_kind::module, split_path(name));
	return std::make_pair(module_ptr, static_cast<bool>(module_ptr));
}

std::pair<memory_ptr<type_info>, bool>
context_info_storage::register_ti(type::kind k, const std::string &name)
{
	context_info_table &table = *impl->get<context_info_table>();

	auto path = split_path(name);
	auto str = std::move(path.back());
	path.pop_back();

	auto context_ptr = find_context(table, context_kind::module, path);

	if(!context_ptr && !path.empty())
		return std::make_pair(nullptr, false);

	auto pair = table.emplace(memory::allocate_counted<type_info>(
		rt_allocator<char>(memory::get_source(memory::fast_pool)),
			k, std::move(str), context_ptr), false);

	return std::make_pair(*pair.first, pair.second);
}

std::pair<memory_ptr<type_info>, bool>
context_info_storage::get_ti(const std::string &name)
{
	context_info_table &table = *impl->get<context_info_table>();

	auto path = split_path(name);
	auto str = std::move(path.back());
	path.pop_back();

	auto module_ptr = find_context(table, context_kind::module, path);
	const auto &key = table.get<context_info::by_parent_kind_name>();
	auto it = key.find(std::make_tuple(module_ptr, context_kind::type, str));

	if(key.end() == it)
		return std::make_pair(nullptr, false);

	return std::make_pair(*it, true);
}

struct string_codec : std::codecvt<char_type, char, std::mbstate_t> { };

using conv_type = std::wstring_convert<string_codec,
	char_type, rt_allocator<char_type>>;

string_data::string_data(const char *str, std::size_t len)
	: u(get_alloc())
{
	set(str, len);
}

std::string string_data::get_str() const
{
	conv_type converter;
	return converter.to_bytes(u);
}

void string_data::set(const char *utf8, std::size_t len)
{
	conv_type converter;
	u.assign(converter.from_bytes(utf8, utf8 + len));
}

bool string_data::empty() const noexcept
{
	return u.empty();
}

std::size_t string_data::size() const noexcept
{
	return u.size();
}

type::rep string_data::at(std::size_t i)
{
	conv_type converter;
	return converter.to_bytes(u.at(i));
}

array_data::array_data(const std::vector<type::rep> &vec)
	: a(get_alloc())
{
	for(auto &v : vec) a.push_back(v);
}

array_data::array_data(std::vector<type::rep> &&vec)
	: a(get_alloc())
{
	for(auto &v : vec) a.push_back(std::move(v));
	vec.clear();
}

std::vector<type::rep> array_data::get_arr() const
{
	return std::vector<type::rep>(a.cbegin(), a.cend());
}

bool array_data::empty() const noexcept
{
	return a.empty();
}

std::size_t array_data::size() const noexcept
{
	return a.size();
}

type::rep array_data::at(std::size_t i)
{
	return a.at(i);
}

} // inline namespace type_system

} // namespace emel
