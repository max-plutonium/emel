/*
 * Copyright (C) 2015, 2016 Max Plutonium <plutonium.max@gmail.com>
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

#include "../opcodes.h"

#include <unordered_map>
#include <boost/optional.hpp>
#include <string>
#include <vector>

namespace emel { namespace compiler {

class const_pool_manager
{
    std::unordered_map<std::string, std::size_t> string_pool;
    std::unordered_map<double, std::size_t> number_pool;
    boost::optional<std::size_t> opt_true, opt_false;
    std::size_t pool_index;

protected:
    std::vector<value_type> &const_pool;

public:
    explicit const_pool_manager(std::vector<value_type> &cp);
    std::size_t store_const(const std::string &value);
    std::size_t store_const(const char *value);
    std::size_t store_const(double value);
    std::size_t store_const(bool value);
};

} // namespace compiler

} // namespace emel
