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

#include "type.h"

namespace emel { inline namespace type_system {

class none final : public type
{
	none();
	virtual kind get_kind() const override;
	virtual std::string get_name() const override;
	virtual bool get_bool(const rep &r) const override;
	virtual std::int64_t get_int(const rep &r) const override;
	virtual double get_num(const rep &r) const override;
	virtual std::string get_str(const rep &r) const override;
	virtual bool empty(const rep &r) const override;
	virtual std::size_t size(const rep &r) const override;
	virtual std::size_t raw_size(const rep &r) const override;

public:
	static const none *get() noexcept;
};

class bool_ : public type
{
	bool_();

protected:
	bool_(unsigned long long);
	virtual kind get_kind() const final override;
	virtual std::string get_name() const final override;
	virtual bool get_bool(const rep &r) const override;
	virtual std::int64_t get_int(const rep &r) const final override;
	virtual double get_num(const rep &r) const final override;
	virtual std::string get_str(const rep &r) const final override;
	virtual bool empty(const rep &r) const final override;
	virtual std::size_t size(const rep &r) const final override;
	virtual std::size_t raw_size(const rep &r) const final override;

public:
	static const bool_ *get() noexcept;
};

class bool_ptr final : public bool_
{
	bool_ptr();
	virtual bool get_bool(const rep &r) const override;

public:
	static const bool_ptr *get() noexcept;
};

class int_ : public type
{
	int_();

protected:
	int_(unsigned long long);
	virtual kind get_kind() const final override;
	virtual std::string get_name() const final override;
	virtual bool get_bool(const rep &r) const final override;
	virtual std::int64_t get_int(const rep &r) const override;
	virtual double get_num(const rep &r) const final override;
	virtual std::string get_str(const rep &r) const final override;
	virtual bool empty(const rep &r) const final override;
	virtual std::size_t size(const rep &r) const final override;
	virtual std::size_t raw_size(const rep &r) const final override;

public:
	static const int_ *get() noexcept;
};

class int_ptr final : public int_
{
	int_ptr();
	virtual std::int64_t get_int(const rep &r) const override;

public:
	static const int_ptr *get() noexcept;
};

class num : public type
{
	num();

protected:
	num(unsigned long long);
	virtual kind get_kind() const final override;
	virtual std::string get_name() const final override;
	virtual bool get_bool(const rep &r) const final override;
	virtual std::int64_t get_int(const rep &r) const final override;
	virtual double get_num(const rep &r) const override;
	virtual std::string get_str(const rep &r) const final override;
	virtual bool empty(const rep &r) const final override;
	virtual std::size_t size(const rep &r) const final override;
	virtual std::size_t raw_size(const rep &r) const final override;

public:
	static const num *get() noexcept;
};

class num_ptr final : public num
{
	num_ptr();
	virtual double get_num(const rep &r) const override;

public:
	static const num_ptr *get() noexcept;
};

class str : public type
{
	str();

protected:
	str(unsigned long long);
	virtual kind get_kind() const final override;
	virtual std::string get_name() const final override;
	virtual bool get_bool(const rep &r) const final override;
	virtual std::int64_t get_int(const rep &r) const final override;
	virtual double get_num(const rep &r) const final override;
	virtual std::string get_str(const rep &r) const override;
	virtual bool empty(const rep &r) const override;
	virtual std::size_t size(const rep &r) const override;
	virtual std::size_t raw_size(const rep &r) const override;
	virtual rep at(const rep &r, std::size_t) const override;

public:
	static const str *get() noexcept;
};

class loc_str final : public str
{
	loc_str();
	virtual std::string get_str(const rep &r) const override;
	virtual bool empty(const rep &r) const override;
	virtual std::size_t size(const rep &r) const override;
	virtual std::size_t raw_size(const rep &r) const override;
	virtual rep at(const rep &r, std::size_t) const override;

public:
	static const loc_str *get() noexcept;
};

class arr final : public type
{
	arr();
	virtual kind get_kind() const override;
	virtual std::string get_name() const override;
	virtual bool get_bool(const rep &r) const override;
	virtual std::int64_t get_int(const rep &r) const override;
	virtual double get_num(const rep &r) const override;
	virtual std::string get_str(const rep &r) const override;
	virtual std::vector<rep> get_arr(const rep &r) const override;
	virtual bool empty(const rep &r) const override;
	virtual std::size_t size(const rep &r) const override;
	virtual std::size_t raw_size(const rep &r) const override;
	virtual rep at(const rep &r, std::size_t) const override;

public:
	static const arr *get() noexcept;
};

/*class ptr : public type
{
public:
	ptr();
	virtual kind get_kind() const override;
	virtual std::string get_name() const override;
	virtual bool get_bool(const rep &r) const override;
	virtual std::int64_t get_int(const rep &r) const override;
	virtual double get_num(const rep &r) const override;
	virtual std::string get_str(const rep &r) const override;
	virtual std::vector<rep> get_arr(const rep &r) const override;
	virtual void *get_ptr(const rep &r) const override;
};*/

} // inline namespace type_system

} // namespace emel
