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
#ifndef ARRAY_H
#define ARRAY_H

#include <utility>
#include <vector>

namespace emel EMEL_EXPORT { namespace runtime EMEL_EXPORT {

class object;

class array
{
    std::pair<object *, object *> ptrs;

public:
    array(object *ptr, std::size_t len);
    array(const std::vector<object> &vec);
    array(std::vector<object> &&vec);
    array(const array &other);
    array &operator =(const array &other);
    array(array &&other);
    array &operator =(array &&other);
    ~array();

    void swap(array &other);

    std::size_t size() const;
    bool empty() const;

    std::vector<object> to_vector() const &;
    std::vector<object> to_vector() &&;

    object &operator[](std::size_t);
    const object &operator[](std::size_t) const;
};

} // namespace runtime

} // namespace emel

#endif // ARRAY_H
