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
#include "array.h"
#include "object.h"

namespace emel { namespace runtime {

array::array(object *ptr, std::size_t len)
{
    ptrs.first = new object[len];
    ptrs.second = ptrs.first + len;
    std::copy(ptr, ptr + len, ptrs.first);
}

array::array(const std::vector<object> &vec)
{
    ptrs.first = new object[vec.size()];
    ptrs.second = ptrs.first + vec.size();
    std::copy(vec.begin(), vec.end(), ptrs.first);
}

array::array(std::vector<object> &&vec)
{
    ptrs.first = new object[vec.size()];
    ptrs.second = ptrs.first + vec.size();
    std::move(vec.begin(), vec.end(), ptrs.first);
}

array::array(const array &other)
{
    const auto len = other.ptrs.second - other.ptrs.first;
    ptrs.first = new object[len];
    ptrs.second = ptrs.first + len;
    std::copy(other.ptrs.first, other.ptrs.second, ptrs.first);
}

array &array::operator =(const array &other)
{
    if(this != &other)
        array(other).swap(*this);
    return *this;
}

array::array(array &&other) : ptrs(nullptr, nullptr)
{
    std::swap(ptrs, other.ptrs);
}

array &array::operator =(array &&other)
{
    if(this != &other)
        array(std::move(other)).swap(*this);
    return *this;
}

array::~array()
{
    delete [] ptrs.first; ptrs.first = ptrs.second = nullptr;
}

void array::swap(array &other)
{
    std::swap(ptrs, other.ptrs);
}

std::size_t array::size() const
{
    return ptrs.second - ptrs.first;
}

bool array::empty() const
{
    return ptrs.second == ptrs.first;
}

std::vector<object> array::to_vector() const &
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = ptrs.first[i];
    return ret;
}

std::vector<object> array::to_vector() &&
{
    std::vector<object> ret(size());
    for(std::size_t i = 0; i < size(); ++i)
        ret[i] = std::move(ptrs.first[i]);
    return ret;
}

object &array::operator[](std::size_t i)
{
    return ptrs.first[i];
}

const object &array::operator[](std::size_t i) const
{
    return ptrs.first[i];
}

} // namespace runtime

} // namespace emel
