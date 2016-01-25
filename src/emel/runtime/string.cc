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

character::character(char c)
{
	set(c);
}

character::character(const char *new_char)
{
	set(new_char);
}

void character::set(char new_char)
{
	std::wstring_convert<string_codec, character::type> converter;
	c = converter.from_bytes(new_char).front();
}

void character::set(type new_char)
{
	c = new_char;
}

void character::set(const char *new_char)
{
	std::wstring_convert<string_codec, character::type> converter;
	c = converter.from_bytes(new_char).front();
}

std::string character::to_string() const
{
	std::wstring_convert<string_codec, character::type> converter;
	return converter.to_bytes(c);
}

string::string()
	: object(new data())
{
}

string::string(const char *s) : object(s)
{
}

string::string(const std::string &s) : object(s)
{
}

} // namespace runtime

} // namespace emel
