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
#include "type.h"
#include "type-builtins.h"
#include "context.h"

#include <limits>

namespace emel { inline namespace type_system {

type::rep::rep(const rep &other) noexcept
{
	if(get_type()->is_counted()) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(other.i & ~0b1111L);
		const auto alive = ac->acquire();
		assert(alive);
	}

	i = other.i;
}

type::rep &type::rep::operator =(const rep &other) noexcept
{
	if(get_type()->is_counted()) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(other.i & ~0b1111L);
		const auto alive = ac->acquire();
		assert(alive);
	}

	clear();
	i = other.i;
	return *this;
}

type::rep::rep(rep &&other) noexcept : rep()
{
	std::swap(i, other.i);
}

type::rep &type::rep::operator =(rep &&other) noexcept
{
	clear();
	std::swap(i, other.i);
	return *this;
}

type::rep::~rep() noexcept
{
	clear();
}

// x...xxxx xxx0 - num
// x...xxxx xx01 - int
// x...xxxx 0011 - str non-local
// x...xxxx 0111 - arr
// x...xxxx 1011 - ptr
// 0...1000 1111 - bool false
// 0...1111 1111 - bool true
// x...0xxx 1111 - str local
// 0...0000 1011 - none (0x0 ptr)
// FIXME 8-byte ptr alignment!

const type *type::rep::get_type() const noexcept
{
	if(0L == (i & 1L))
		return num::get();

	else if(0L == (i & 2L))
		return int_::get();

	else if(0b1011L == i)
		return none::get();

	switch(i & 0b10001111L) {
		case 0b00001111L:
			return loc_str::get(); // local str
		case 0b10001111L:
			return bool_::get();
		default:
			switch(i & 0b1111L) {
				case 0b0011L:
					return str::get(); // non-local str
				case 0b0111L:
					return arr::get();
				case 0b1011L: {
					assert(0L != (i & ~0b1111L)); // this is none
					auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
					return ac->get<object_info>()->t;
				}
				case 0b1111L:
					assert(false); // bool or local str
				default: break;
			}
	}

	assert(false);
	return nullptr;
}

void type::rep::clear()
{
	if(get_type()->is_counted()) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		ac->release();
	}

	i = 0b1011L;
}

bool type::rep::get(bool &value) const noexcept
{
	if(0b10001111L == (i & 0b10001111L)) {
		value = 0b01110000L == (i & 0b01110000L);
		return true;
	}

	return false;
}

bool type::rep::get(std::int64_t &value) const noexcept
{
	if(1L == (i & 0b11L)) {
		const bool negative = 0L != (i & 0b100L);
		value = (static_cast<std::uint64_t>(i) >> 3L) | ((negative ? 0b111L : 0L) << 61L);
		return true;
	}

	if(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L)) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		if(type::int_ == ac->get<object_info>()->t->get_kind()) {
			value = *ac->get<std::int64_t>();
			return true;
		}
	}

	return false;
}

static constexpr auto exp_mask = 0b011111111111UL << 52;
static constexpr auto beg_mask = 0b001000000000UL << 52;

bool type::rep::get(double &value) const noexcept
{
	if(0L == (i & 1L)) {
		auto orig_i = (static_cast<std::uint64_t>(i) >> 3L)
			| ((i | (0L == (i & 0b10L))) << 61L);

		if (__builtin_expect((beg_mask | 0xffffffffffffL) == (orig_i & ~(1L << 63L)), false))
			orig_i &= 1L << 63L;
		else if(__builtin_expect(beg_mask == (orig_i & exp_mask), false))
			orig_i = (orig_i & ~beg_mask) | exp_mask;

		union { std::uint64_t i; double d; } u;
		u.i = orig_i; value = u.d;
		return true;
	}

	if(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L)) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		if(type::num == ac->get<object_info>()->t->get_kind()) {
			value = *ac->get<double>();
			return true;
		}
	}

	return false;
}

bool type::rep::get(std::string &value) const noexcept
{
	// get from local storage
	if(0b00001111L == (i & 0b10001111L)) {
		const auto len = std::size_t(i & 0b01110000L) >> 4L;
		value.assign(s + 1L, len);
		return true;
	}

	// get from non-local storage
	if(0b0011L == (i & 0b1111L)) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		value = ac->get<string_data>()->get_str();
		return true;
	}

	return false;
}

bool type::rep::get(std::vector<rep> &values) const noexcept
{
	if(0b0111L == (i & 0b1111L)) {
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		values = ac->get<array_data>()->get_arr();
		return true;
	}

	return false;
}

bool type::rep::get(memory::counted_ptr &value) const noexcept
{
	if(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L)) {
		value = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		return true;
	}

	return false;
}

void type::rep::set(bool value) noexcept
{
	clear();
	i = value ? 0b11111111L : 0b10001111L;
}

void type::rep::set(std::int64_t value) noexcept
{
	clear();

	static constexpr auto max_local_value = std::numeric_limits<decltype(value)>::max() / 4;
	static constexpr auto min_local_value = std::numeric_limits<decltype(value)>::min() / 4;

	if(__builtin_expect((value < max_local_value) && (value > min_local_value), true))
		i = (value << 3L) | (value < 0L ? 0b101L : 0b1L);

	else {
		auto *const ac = memory::make_counted<decltype(value)>(value);
		i = reinterpret_cast<std::int64_t>(ac);
		assert(0L == (i & 0b1111L)); // alignment
		i |= 0b1011L;
	}
}

void type::rep::set(double value) noexcept
{
	clear();

	n = value;
	const auto is_zero = 0L == (i & ~(1L << 63L));
	const bool fits_in_local_range = (0L != (i & (1L << 61L))) ^ (0L != (i & (1L << 62L)));
	const bool greater_than_range_begin = beg_mask != (i & exp_mask);
	const bool is_spec_number = exp_mask == (i & exp_mask);

	if(__builtin_expect(is_zero, false))
		i |= beg_mask | 0xffffffffffffL;
	else if(__builtin_expect(is_spec_number, false))
		i = (i & ~exp_mask) | beg_mask;

	if(__builtin_expect(is_zero
		|| (fits_in_local_range && greater_than_range_begin)
		|| is_spec_number, true))

		i = (i << 3L) | ((static_cast<std::uint64_t>(i) >> 61L) & ~1L);

	else {
		auto *const ac = memory::make_counted<decltype(value)>(value);
		i = reinterpret_cast<std::int64_t>(ac);
		assert(0L == (i & 0b1111L)); // alignment
		i |= 0b1011L;
	}
}

void type::rep::set(const char *value, std::size_t len)
{
	clear();

	// put into local storage
	if(len <= local_bytes_count) {
		assert(0L == (len & ~0b0111L));
		std::memcpy(s + 1L, value, len);
		i |= (len << 4L) | 0b1111L;

	} else {
		auto *const ac = memory::make_counted<string_data>(value, len);
		i = reinterpret_cast<std::int64_t>(ac);
		assert(0L == (i & 0b1111L)); // alignment
		i |= 0b0011L;
	}
}

void type::rep::set(const std::vector<rep> &values)
{
	clear();
	auto *const ac = memory::make_counted<array_data>(values);
	i = reinterpret_cast<std::int64_t>(ac);
	assert(0L == (i & 0b1111L)); // alignment
	i |= 0b0111L;
}

void type::rep::set(std::vector<rep> &&values)
{
	clear();
	auto *const ac = memory::make_counted<array_data>(std::move(values));
	i = reinterpret_cast<std::int64_t>(ac);
	assert(0L == (i & 0b1111L)); // alignment
	i |= 0b0111L;
}

void type::rep::set(memory::counted_ptr value) noexcept
{
	clear();
	assert(0L == (std::int64_t(value.get()) & 0b1111L)); // alignment
	assert(value->use_count() > 0);
	i = std::int64_t(value.detach()) | 0b1011L;
}

void type::rep::set(std::nullptr_t) noexcept
{
	clear();
}

bool type::rep::get_bool_unchecked(bool /*from_ptr*/) const noexcept
{
	// TODO from_ptr
	assert(0b10001111L == (i & 0b10001111L));
	return 0b01110000L == (i & 0b01110000L);
}

std::int64_t type::rep::get_int_unchecked(bool from_ptr) const noexcept
{
	if(from_ptr) {
		assert(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L));
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		assert(type::int_ == ac->get<object_info>()->t->get_kind());
		return *ac->get<std::int64_t>();

	} else {
		assert(1L == (i & 0b11L));
		const bool negative = 0L != (i & 0b100L);
		return (static_cast<std::uint64_t>(i) >> 3L) | ((negative ? 0b111L : 0L) << 61L);
	}
}

double type::rep::get_num_unchecked(bool from_ptr) const noexcept
{
	if(from_ptr) {
		assert(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L));
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		assert(type::num == ac->get<object_info>()->t->get_kind());
		return *ac->get<double>();

	} else {
		assert(0L == (i & 1L));
		auto orig_i = (static_cast<std::uint64_t>(i) >> 3L)
			| ((i | (0L == (i & 0b10L))) << 61L);

		if (__builtin_expect((beg_mask | 0xffffffffffffL) == (orig_i & ~(1L << 63L)), false))
			orig_i &= 1L << 63L;
		else if(__builtin_expect(beg_mask == (orig_i & exp_mask), false))
			orig_i = (orig_i & ~beg_mask) | exp_mask;

		union { std::uint64_t i; double d; } u;
		u.i = orig_i;
		return u.d;
	}
}

std::string type::rep::get_str_unchecked(bool from_ptr) const noexcept
{
	if(from_ptr) {
		assert(0b0011L == (i & 0b1111L));
		auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
		assert(type::str == ac->get<object_info>()->t->get_kind());
		return ac->get<string_data>()->get_str();

	} else {
		assert(0b00001111L == (i & 0b10001111L));
		const auto len = std::size_t(i & 0b01110000L) >> 4L;
		return std::string(s + 1L, len);
	}
}

std::vector<type::rep> type::rep::get_arr_unchecked() const noexcept
{
	assert(0b0111L == (i & 0b1111L));
	auto *const ac = reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
	return ac->get<array_data>()->get_arr();
}

memory::counted_ptr type::rep::get_ptr_unchecked() const noexcept
{
	assert(0b1011L == (i & 0b1111L) && 0L != (i & ~0b1111L));
	return reinterpret_cast<memory::atomic_counted *>(i & ~0b1111L);
}

const char *type_name(type::kind t)
{
	const char *result = "{ptr}";

# define TYPE_NAME(T, STR) \
	case T: result = STR; break;

	switch (t) {
		TYPE_NAME(type::none,  "{none}")
		TYPE_NAME(type::bool_, "{bool}")
		TYPE_NAME(type::int_,  "{int}")
		TYPE_NAME(type::num,   "{num}")
		TYPE_NAME(type::str,   "{str}")
		TYPE_NAME(type::arr,   "{arr}")

		default:
			break;
	}

# undef TYPE_NAME

	return result;
}

} // inline namespace type_system

} // namespace emel
