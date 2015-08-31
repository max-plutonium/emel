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
#ifndef SYMBOLS_MANAGER_H
#define SYMBOLS_MANAGER_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <deque>
#include <utility>

namespace emel { namespace bytecode {

struct symbol
{
    enum type {
        local, field, static_field,
        method, static_method, max_type
    };

    enum kind {
        regular, param = 0x2, ref = 0x4
    };

    std::string name;
    std::size_t index;
    type t;
    kind k;
};

struct symbol_table {
    std::unordered_multimap<std::string, symbol> table;
    std::deque<decltype(table)::iterator> index_list;

    symbol &add_symbol(const std::string &name, symbol::type type = symbol::local,
        symbol::kind kind = symbol::regular);
};

using symbols = std::array<symbol_table, symbol::max_type>;

class symbols_manager
{
    symbols &syms;

public:
    explicit symbols_manager(symbols &syms);

    std::pair<symbol *, bool>
    find_symbol(const std::string &name, symbol::type type);

    std::pair<const symbol *, bool>
    find_symbol(const std::string &name, symbol::type type) const;

    symbol &add_symbol(const std::string &name, symbol::type type = symbol::local,
        symbol::kind kind = symbol::regular);

    void drop_symbols(std::size_t n, symbol::type type);
    std::size_t num_symbols(symbol::type type) const;

    static std::string name_encode(const std::string &class_name,
        const std::string &member_name, symbol::type type);
};

} // namespace bytecode

} // namespace emel

#endif // SYMBOLS_MANAGER_H
