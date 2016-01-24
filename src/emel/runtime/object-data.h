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
#include "../memory/gc.h"
#include "../memory/pool.h"

namespace emel { namespace runtime {

struct object::data
{
	virtual ~data() = default;

	boost::variant<empty_value_type, bool, double, std::string, array, reference> u;

	void *operator new(std::size_t size);
	void operator delete(void *ptr);
};

} // namespace runtime

} // namespace emel
