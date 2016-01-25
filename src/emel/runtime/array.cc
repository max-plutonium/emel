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

array::array()
	: object(new data())
{
}

array::array(object *ptr, std::size_t len)
	: object(new data(ptr, len))
{
}

array::array(const std::vector<object> &vec)
	: object(new data(vec))
{
}

array::array(std::vector<object> &&vec)
	: object(new data(std::move(vec)))
{
}

std::vector<object> array::to_vector() const &
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = this->operator [](i);
    return ret;
}

std::vector<object> array::to_vector() &&
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = std::move(this->operator [](i));
    return ret;
}

} // namespace runtime

} // namespace emel
