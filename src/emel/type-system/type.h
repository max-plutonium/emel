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

#include <bitset>
#include <boost/type_index.hpp>
#include <cstdint>
#include <string>
#include <vector>

#include "../memory/memory.h"

namespace emel EMEL_EXPORT { inline namespace type_system {

class type
{
	std::bitset<32> bits;

	enum {
		pos_sys_user = 0,
		pos_prim_comp = 1,
		pos_single_array = 2,
		pos_counted = 3
	};

	friend class none;
	friend class bool_;
	friend class bool_ptr;
	friend class int_;
	friend class int_ptr;
	friend class num;
	friend class num_ptr;
	friend class str;
	friend class loc_str;
	friend class arr;

	type() = default;
	type(std::bitset<32> &&bs) : bits(std::move(bs)) { }

protected:
	virtual ~type() = default;

public:
	enum kind {
		none,
		bool_,
		int_,
		num,
		str,
		arr,
		ptr
	};

	bool is_system() const { return !bits.test(pos_sys_user); }
	bool is_user() const { return bits.test(pos_sys_user); }

	bool is_primitive() const { return !bits.test(pos_prim_comp); }
	bool is_composite() const { return bits.test(pos_prim_comp); }

	bool is_single() const { return !bits.test(pos_single_array); }
	bool is_array() const { return bits.test(pos_single_array); }

	bool is_local() const { return !bits.test(pos_counted); }
	bool is_counted() const { return bits.test(pos_counted); }

	bool is_managed() const { return is_composite(); }

	union rep
	{
	private:
		double n;
		std::int64_t i;
		char s[sizeof(double)];

		static_assert(sizeof(double) == sizeof(std::int64_t), "doubles must be 64 bit");
		static_assert(BYTE_ORDER == LITTLE_ENDIAN, "little endian only");

		static constexpr std::size_t local_bytes_count = sizeof(double) - sizeof(char);

	public:
		inline rep() noexcept : i(0) { set(nullptr); }
		inline rep(bool value) noexcept : i(0) { set(value); }
		inline rep(std::int64_t value) noexcept : i(0) { set(value); }
		inline rep(double value) noexcept : i(0) { set(value); }
		inline rep(const std::string &value) : i(0) { set(value); }
		inline rep(const char *value, std::size_t len) : i(0) { set(value, len); }
		inline rep(const std::vector<rep> &values) : i(0) { set(values); }
		inline rep(std::vector<rep> &&values) : i(0) { set(std::move(values)); }
		inline rep(void *value) noexcept : i(0) { set(value); }

	  template <std::size_t Nm>
		inline rep(const  char (&value) [Nm]) : i(0) { set(value); }

		rep(const rep &other) noexcept;
		rep &operator =(const rep &other) noexcept;
		rep(rep &&other) noexcept;
		rep &operator =(rep &&other) noexcept;

		~rep() noexcept;
		const type *get_type() const noexcept;
		void clear();

		bool get(bool &value) const noexcept;
		bool get(std::int64_t &value) const noexcept;
		bool get(double &value) const noexcept;
		bool get(std::string &value) const noexcept;
		bool get(std::vector<rep> &values) const noexcept;
		bool get(memory::counted_ptr &value) const noexcept;

		void set(bool value) noexcept;
		void set(std::int64_t value) noexcept;
		void set(double value) noexcept;
		void set(const char *value, std::size_t len);
		void set(const std::vector<rep> &values);
		void set(std::vector<rep> &&values);
		void set(memory::counted_ptr value) noexcept;
		void set(std::nullptr_t) noexcept;

	  template <std::size_t Nm>
		inline void set(const  char (&value) [Nm]) { set(value, Nm - 1); }

		inline void set(const std::string &value) { set(value.data(), value.length()); }

		bool get_bool_unchecked(bool from_ptr = false) const noexcept;
		std::int64_t get_int_unchecked(bool from_ptr = false) const noexcept;
		double get_num_unchecked(bool from_ptr = false) const noexcept;
		std::string get_str_unchecked(bool from_ptr = false) const noexcept;
		std::vector<rep> get_arr_unchecked() const noexcept;
		memory::counted_ptr get_ptr_unchecked() const noexcept;
	};

	virtual kind get_kind() const = 0;
	virtual std::string get_name() const = 0;

	virtual bool get_bool(const rep &r) const = 0;
	virtual std::int64_t get_int(const rep &r) const = 0;
	virtual double get_num(const rep &r) const = 0;
	virtual std::string get_str(const rep &r) const = 0;
	virtual std::vector<rep> get_arr(const rep &) const { return std::vector<rep>(); }
	virtual void *get_ptr(const rep &) const { return nullptr; }
	virtual bool empty(const rep &r) const = 0;
	virtual std::size_t size(const rep &r) const = 0;
	virtual std::size_t raw_size(const rep &r) const = 0;
	virtual rep at(const rep &, std::size_t) const { return rep(); }
};

EMEL_EXPORT const char *type_name(type::kind t);

} // inline namespace type_system

} // namespace emel
