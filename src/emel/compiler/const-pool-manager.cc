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
#include "const-pool-manager.h"

namespace emel { namespace compiler {

const_pool_manager::const_pool_manager(std::vector<value_type> &cp)
    : pool_index(1), const_pool(cp)
{
    const_pool.emplace_back(empty_value);
}

std::size_t const_pool_manager::store_const(const std::string &value)
{
    auto pair = string_pool.emplace(value, pool_index);
    std::size_t index = pair.second ?
                pool_index++ : pair.first->second;
    if(pair.second)
        const_pool.emplace_back(value);
    return index;
}

std::size_t const_pool_manager::store_const(const char *value)
{
    return store_const(std::string(value));
}

std::size_t const_pool_manager::store_const(double value)
{
    auto pair = number_pool.emplace(value, pool_index);
    std::size_t index = pair.second ?
                pool_index++ : pair.first->second;
    if(pair.second)
        const_pool.emplace_back(value);
    return index;
}

std::size_t const_pool_manager::store_const(bool value)
{
    auto &opt = value ? opt_true : opt_false;
    if(!opt) {
        opt = boost::make_optional(pool_index++);
        const_pool.emplace_back(value);
    }
    return opt.value();
}

} // namespace compiler

} // namespace emel
