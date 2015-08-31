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
#ifndef CONST_POOL_MANAGER_H
#define CONST_POOL_MANAGER_H

#include "bytecode.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace emel { namespace bytecode {

class const_pool_manager
{
    std::unordered_map<std::string, std::size_t> string_pool;
    std::unordered_map<double, std::size_t> number_pool;
    std::size_t pool_index;
    std::vector<value_type> &const_pool;

public:
    explicit const_pool_manager(std::vector<value_type> &cp);
    std::size_t store_const(const std::string &value);
    std::size_t store_const(double value);
};

} // namespace bytecode

} // namespace emel

#endif // CONST_POOL_MANAGER_H