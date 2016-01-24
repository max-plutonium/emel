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

#include "object.h"

namespace emel EMEL_EXPORT { namespace runtime EMEL_EXPORT {

struct character
{
	using type = char16_t;

	type c = 0;

	constexpr character() = default;
	character(char c);
	constexpr character(type c) : c(c) { }
	character(const char *new_char);

	void set(char new_char);
	void set(type new_char);
	void set(const char *new_char);
	std::string to_string() const;

	constexpr type get() const { return c; }
};

class string : public object
{
protected:
	class data;
	friend class object;

public:
	string();
	string(const char *s);
	string(const std::string &s);
};

} // namespace runtime

} // namespace emel
