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
#include "symbols_manager.h"

#include <boost/lexical_cast.hpp>

namespace emel { namespace bytecode {

symbol &symbol_table::add_symbol(const std::string &name,
    symbol::type type, symbol::kind kind)
{
    auto it = table.emplace(name, symbol {
        name, index_list.size(), type, kind
    });

    index_list.push_back(it);
    return it->second;
}

symbols_manager::symbols_manager(symbols &syms)
    : syms(syms)
{
}

std::pair<symbol *, bool>
symbols_manager::find_symbol(const std::string &name, symbol::type type)
{
    auto it = syms[type].table.find(name);
    if(syms[type].table.cend() == it)
        return std::make_pair(nullptr, false);
    return std::make_pair(&it->second, true);
}

std::pair<const symbol *, bool>
symbols_manager::find_symbol(const std::string &name, symbol::type type) const
{
    auto it = syms[type].table.find(name);
    if(syms[type].table.cend() == it)
        return std::make_pair(nullptr, false);
    return std::make_pair(&it->second, true);
}

symbol &symbols_manager::add_symbol(const std::string &name,
    symbol::type type, symbol::kind kind)
{
    return syms[type].add_symbol(name, type, kind);
}

void symbols_manager::drop_symbols(std::size_t n, symbol::type type)
{
    auto &table = syms[type];
    if(n > table.index_list.size())
        n = table.index_list.size();

    for(; n; --n) {
        table.table.erase(table.index_list.back());
        table.index_list.pop_back();
    }
}

std::size_t symbols_manager::num_symbols(symbol::type type) const
{
    return syms[type].index_list.size();
}

/*static*/
std::string symbols_manager::name_encode(const std::string &class_name,
    const std::string &member_name, symbol::type type)
{
    switch (type) {
        case symbol::local:
            return boost::lexical_cast<std::string>(member_name.size()) + member_name;

        case symbol::field:
            return boost::lexical_cast<std::string>(class_name.size()) + class_name
                + "@" + boost::lexical_cast<std::string>(member_name.size()) + member_name;

        case symbol::static_field:
            return boost::lexical_cast<std::string>(class_name.size()) + class_name
                + "#" + boost::lexical_cast<std::string>(member_name.size()) + member_name;

        case symbol::method:
            return boost::lexical_cast<std::string>(class_name.size()) + class_name
                + "$" + boost::lexical_cast<std::string>(member_name.size()) + member_name;

        case symbol::static_method:
            return boost::lexical_cast<std::string>(class_name.size()) + class_name
                + "%" + boost::lexical_cast<std::string>(member_name.size()) + member_name;

        default:
            return member_name;
    }
}

} // namespace bytecode

} // namespace emel
