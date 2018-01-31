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
#include "type-builtins.h"
#include "context.h"

#include <boost/algorithm/string.hpp>
#include <boost/convert.hpp>
#include <boost/convert/spirit.hpp>

struct boost::cnv::by_default : boost::cnv::spirit { };

namespace emel { inline namespace type_system {

none::none() : type() { }
type::kind none::get_kind() const { return type::none; }
std::string none::get_name() const { return "none"; }

bool none::get_bool(const type::rep &) const { return false; }
std::int64_t none::get_int(const type::rep &) const { return 0L; }
double none::get_num(const type::rep &) const { return 0.0; }
std::string none::get_str(const type::rep &) const { return std::string(); }
bool none::empty(const type::rep &) const { return true; }
std::size_t none::size(const type::rep &) const { return 0; }
std::size_t none::raw_size(const type::rep &) const { return 0; }

/*static*/
const none *none::get() noexcept { static none instance; return &instance; }


bool_::bool_() : type() { }
bool_::bool_(unsigned long long l) : type(l) { }
type::kind bool_::get_kind() const { return type::bool_; }
std::string bool_::get_name() const { return "bool"; }

bool bool_::get_bool(const type::rep &r) const {
	return r.get_bool_unchecked();
}

std::int64_t bool_::get_int(const type::rep &r) const {
	return get_bool(r) ? 1L : 0L;
}

double bool_::get_num(const type::rep &r) const {
	return get_bool(r) ? 1.0 : 0.0;
}

std::string bool_::get_str(const type::rep &r) const {
	return get_bool(r) ? "true" : "false";
}

bool bool_::empty(const type::rep &) const { return false; }
std::size_t bool_::size(const type::rep &) const { return 1; }
std::size_t bool_::raw_size(const type::rep &) const { return 1; }

/*static*/
const bool_ *bool_::get() noexcept { static bool_ instance; return &instance; }


bool_ptr::bool_ptr() : bool_(1 << pos_counted) { }

bool bool_ptr::get_bool(const type::rep &r) const {
	return r.get_bool_unchecked(true);
}

/*static*/
const bool_ptr *bool_ptr::get() noexcept { static bool_ptr instance; return &instance; }


int_::int_() : type() { }
int_::int_(unsigned long long l) : type(l) { }
type::kind int_::get_kind() const { return type::int_; }
std::string int_::get_name() const { return "int"; }

bool int_::get_bool(const type::rep &r) const {
	return static_cast<bool>(get_int(r));
}

std::int64_t int_::get_int(const type::rep &r) const {
	return r.get_int_unchecked();
}

double int_::get_num(const type::rep &r) const {
	return static_cast<double>(get_int(r));
}

std::string int_::get_str(const type::rep &r) const {
	return boost::convert<std::string>(get_int(r)).value();
}

bool int_::empty(const type::rep &) const { return false; }
std::size_t int_::size(const type::rep &) const { return 1; }
std::size_t int_::raw_size(const type::rep &) const { return sizeof(std::int64_t); }

/*static*/
const int_ *int_::get() noexcept { static int_ instance; return &instance; }


int_ptr::int_ptr() : int_(1 << pos_counted) { }

std::int64_t int_ptr::get_int(const type::rep &r) const {
	return r.get_int_unchecked(true);
}

/*static*/
const int_ptr *int_ptr::get() noexcept { static int_ptr instance; return &instance; }


num::num() : type() { }
num::num(unsigned long long l) : type(l) { }
type::kind num::get_kind() const { return type::num; }
std::string num::get_name() const { return "num"; }

bool num::get_bool(const type::rep &r) const {
	return static_cast<bool>(get_num(r));
}

std::int64_t num::get_int(const type::rep &r) const {
	return static_cast<std::int64_t>(get_num(r));
}

double num::get_num(const type::rep &r) const {
	return r.get_num_unchecked();
}

std::string num::get_str(const type::rep &r) const {
	return boost::convert<std::string>(get_num(r)).value();
}

bool num::empty(const type::rep &) const { return false; }
std::size_t num::size(const type::rep &) const { return 1; }
std::size_t num::raw_size(const type::rep &) const { return sizeof(double); }

/*static*/
const num *num::get() noexcept { static num instance; return &instance; }


num_ptr::num_ptr() : num(1 << pos_counted) { }

double num_ptr::get_num(const type::rep &r) const {
	return r.get_num_unchecked(true);
}

/*static*/
const num_ptr *num_ptr::get() noexcept { static num_ptr instance; return &instance; }


str::str() : type((1 << pos_counted) | (1 << pos_single_array)) { }
str::str(unsigned long long l) : type(l) { }
type::kind str::get_kind() const { return type::str; }
std::string str::get_name() const { return "str"; }

bool str::get_bool(const type::rep &r) const {
	auto str = get_str(r);
	boost::algorithm::trim(str);
	//boost::algorithm::erase_all(str, " \n\t\r\0");
	return !str.empty() && !boost::iequals("false", str);
}

std::int64_t str::get_int(const type::rep &r) const {
	const auto str = get_str(r);
	return boost::convert<std::int64_t>(str).value_or_eval([&str]() -> std::int64_t {
		throw std::invalid_argument("can't convert \"" + str.substr(0, 10) + "\" to int");
	});
}

double str::get_num(const type::rep &r) const {
	return boost::convert<double>(get_str(r))
		.value_or(std::numeric_limits<double>::quiet_NaN());
}

std::string str::get_str(const type::rep &r) const {
	return r.get_str_unchecked(true);
}

bool str::empty(const type::rep &r) const {
	auto ptr = r.get_ptr_unchecked();
	return ptr->get<string_data>()->empty();
}

std::size_t str::size(const type::rep &r) const {
	auto ptr = r.get_ptr_unchecked();
	return ptr->get<string_data>()->size();
}

std::size_t str::raw_size(const type::rep &r) const {
	return size(r) * sizeof(char_type) + sizeof(string_data);
}

type::rep str::at(const type::rep &r, std::size_t idx) const {
	auto ptr = r.get_ptr_unchecked();
	auto *const sd = ptr->get<string_data>();
	if(idx >= sd->size())
		throw std::out_of_range("str: index " + std::to_string(idx)
			+ " >= size of str (which is " + std::to_string(sd->size()) + ").");
	return sd->at(idx);
}

/*static*/
const str *str::get() noexcept { static str instance; return &instance; }


loc_str::loc_str() : str(1 << pos_single_array) { }

std::string loc_str::get_str(const type::rep &r) const {
	return r.get_str_unchecked();
}

bool loc_str::empty(const type::rep &r) const {
	return get_str(r).empty();
}

std::size_t loc_str::size(const type::rep &r) const {
	return get_str(r).size(); // TODO multibyte!
}

std::size_t loc_str::raw_size(const type::rep &r) const {
	return size(r);
}

type::rep loc_str::at(const type::rep &r, std::size_t idx) const {
	auto s = get_str(r);
	const auto len = s.size();
	if(idx >= len)
		throw std::out_of_range("str: index " + std::to_string(idx)
			+ " >= size of str (which is " + std::to_string(len) + ").");
	return s.substr(idx, 1); // TODO multibyte!
}

/*static*/
const loc_str *loc_str::get() noexcept { static loc_str instance; return &instance; }


arr::arr() : type((1 << pos_counted) | (1 << pos_single_array)) { }
type::kind arr::get_kind() const { return type::arr; }
std::string arr::get_name() const { return "arr"; }

bool arr::get_bool(const type::rep &r) const {
	return !empty(r);
}

std::int64_t arr::get_int(const type::rep &r) const {
	return size(r);
}

double arr::get_num(const type::rep &r) const {
	return size(r);
}

std::string arr::get_str(const type::rep &r) const {
	return "array of " + std::to_string(size(r))+ " elements";
}

std::vector<type::rep> arr::get_arr(const type::rep &r) const {
	auto ptr = r.get_ptr_unchecked();
	return ptr->get<array_data>()->get_arr();
}

bool arr::empty(const type::rep &r) const {
	auto ptr = r.get_ptr_unchecked();
	return ptr->get<array_data>()->empty();
}

std::size_t arr::size(const type::rep &r) const {
	auto ptr = r.get_ptr_unchecked();
	return ptr->get<array_data>()->size();
}

std::size_t arr::raw_size(const type::rep &r) const {
	return size(r) * sizeof(rep) + sizeof(array_data);
}

type::rep arr::at(const type::rep &r, std::size_t idx) const {
	auto ptr = r.get_ptr_unchecked();
	auto *const ad = ptr->get<array_data>();
	if(idx >= ad->size())
		throw std::out_of_range("arr: index " + std::to_string(idx)
			+ " >= size of arr (which is " + std::to_string(ad->size()) + ").");
	return ad->at(idx);
}

/*static*/
const arr *arr::get() noexcept { static arr instance; return &instance; }

} // inline namespace type_system

} // namespace emel
