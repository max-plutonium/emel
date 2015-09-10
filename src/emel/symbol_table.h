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
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <type_traits>
#include <unordered_map>
#include <deque>
#include <array>
#include <utility>
#include <string>
#include <vector>

namespace emel {

template <typename Tp, typename Enum, std::size_t Size,
          bool guard = std::is_integral<Enum>::value || std::is_enum<Enum>::value>
class basic_symbol_table;

template <typename Tp, typename Enum, std::size_t Size>
class basic_symbol_table<Tp, Enum, Size, true>
{
public:
    using value_type = Tp;
    using enum_type = Enum;
    using size_type = std::size_t;
    static constexpr size_type max_size = Size;

protected:
    struct entry {
        std::unordered_multimap<std::string, value_type> table;
        std::deque<decltype(table)::iterator> index_list;
    };

    std::array<entry, max_size> entries;

public:
    std::pair<value_type *, bool>
    find_symbol(const std::string &name, enum_type type)
    {
        auto it = entries[type].table.find(name);
        if(entries[type].table.cend() == it)
            return std::make_pair(nullptr, false);
        return std::make_pair(&it->second, true);
    }

    std::pair<const value_type *, bool>
    find_symbol(const std::string &name, enum_type type) const
    {
        auto it = entries[type].table.find(name);
        if(entries[type].table.cend() == it)
            return std::make_pair(nullptr, false);
        return std::make_pair(&it->second, true);
    }

    std::vector<std::pair<value_type *, enum_type>>
    find_symbols(const std::string &name)
    {
        std::vector<std::pair<value_type *, enum_type>> ret;

        for(enum_type type = 0; type < max_size; ++type) {
            auto pair = find_symbol(name, type);
            if(pair.second)
                ret.push_back(std::make_pair(pair.first, type));
        }

        return ret;
    }

    std::vector<std::pair<const value_type *, enum_type>>
    find_symbols(const std::string &name) const
    {
        std::vector<std::pair<const value_type *, enum_type>> ret;

        for(enum_type type = 0; type < max_size; ++type) {
            auto pair = find_symbol(name, type);
            if(pair.second)
                ret.push_back(std::make_pair(pair.first, type));
        }

        return ret;
    }

    value_type &add_symbol(const std::string &name, value_type value, enum_type type)
    {
        auto &ent = entries[type];
        auto it = ent.table.emplace(name, std::move(value));
        ent.index_list.push_back(it);
        return it->second;
    }

    void drop_symbols(std::size_t n, enum_type type)
    {
        auto &ent = entries[type];
        if(n > ent.index_list.size())
            n = ent.index_list.size();

        for(; n; --n) {
            ent.table.erase(ent.index_list.back());
            ent.index_list.pop_back();
        }
    }

    std::size_t num_symbols(enum_type type) const
    {
        return entries[type].index_list.size();
    }
};

} // namespace emel

#endif // SYMBOL_TABLE_H
