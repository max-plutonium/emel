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

#include <type_traits>
#include <unordered_map>
#include <deque>
#include <array>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>

namespace emel { namespace compiler {

template <typename Tp, typename Enum, std::size_t Size,
    typename = typename std::enable_if<std::is_integral<Enum>::value
        || std::is_enum<Enum>::value>::type>
class basic_symbol_table
{
public:
    using value_type = Tp;
    using enum_type = Enum;
    using size_type = std::size_t;
    static constexpr size_type max_size = Size;

protected:
    struct entry {
        std::unordered_multimap<std::string, value_type> table;
        std::deque<typename decltype(table)::iterator> index_list;
    };

    std::array<entry, max_size> entries;

public:
    std::pair<value_type *, bool>
    find_symbol(const std::string &name, enum_type type)
    {
        auto it = entries[static_cast<std::size_t>(type)].table.find(name);
        if(entries[static_cast<std::size_t>(type)].table.cend() == it)
            return std::make_pair(nullptr, false);
        return std::make_pair(&it->second, true);
    }

    std::pair<const value_type *, bool>
    find_symbol(const std::string &name, enum_type type) const
    {
        auto it = entries[static_cast<std::size_t>(type)].table.find(name);
        if(entries[static_cast<std::size_t>(type)].table.cend() == it)
            return std::make_pair(nullptr, false);
        return std::make_pair(&it->second, true);
    }

  template <typename Predicate>
    std::pair<value_type *, bool>
    find_symbol_if(const std::string &name, enum_type type, Predicate &&pred)
    {
        std::pair<value_type *, bool> result { nullptr, false };

        for(auto pair = entries[static_cast<std::size_t>(type)].table.equal_range(name);
            pair.first != pair.second; ++pair.first)
        {
            if(pred(pair.first->second)) {
                result = std::make_pair(&pair.first->second, true);
                break;
            }
        }

        return result;
    }

  template <typename Predicate>
    std::pair<const value_type *, bool>
    find_symbol_if(const std::string &name, enum_type type, Predicate &&pred) const
    {
        std::pair<value_type *, bool> result { nullptr, false };

        for(auto pair = entries[static_cast<std::size_t>(type)].table.equal_range(name);
            pair.first != pair.second; ++pair.first)
        {
            if(pred(pair.first->second)) {
                result = std::make_pair(&pair.first->second, true);
                break;
            }
        }

        return result;
    }

    std::vector<std::pair<value_type *, enum_type>>
    find_symbols(const std::string &name)
    {
        std::vector<std::pair<value_type *, enum_type>> ret;

        for(std::size_t type = 0; type < max_size; ++type) {
            auto pair = find_symbol(name, static_cast<enum_type>(type));
            if(pair.second)
                ret.push_back(std::make_pair(pair.first, static_cast<enum_type>(type)));
        }

        return ret;
    }

    std::vector<std::pair<const value_type *, enum_type>>
    find_symbols(const std::string &name) const
    {
        std::vector<std::pair<const value_type *, enum_type>> ret;

        for(std::size_t type = 0; type < max_size; ++type) {
            auto pair = find_symbol(name, static_cast<enum_type>(type));
            if(pair.second)
                ret.push_back(std::make_pair(pair.first, static_cast<enum_type>(type)));
        }

        return ret;
    }

    std::vector<value_type *>
    find_symbols(const std::string &name, enum_type type)
    {
        std::vector<value_type *> ret;
        auto pair = entries[static_cast<std::size_t>(type)].table.equal_range(name);
        ret.reserve(std::distance(pair.first, pair.second));

        std::transform(pair.first, pair.second, std::back_inserter(ret),
            [](std::pair<const std::string, value_type> &entry) {
                return &entry.second;
            });

        return ret;
    }

    std::vector<const value_type *>
    find_symbols(const std::string &name, enum_type type) const
    {
        std::vector<const value_type *> ret;
        auto pair = entries[static_cast<std::size_t>(type)].table.equal_range(name);
        ret.reserve(std::distance(pair.first, pair.second));

        std::transform(pair.first, pair.second, std::back_inserter(ret),
            [](const std::pair<const std::string, value_type> &entry) {
                return &entry.second;
            });

        return ret;
    }

    value_type &add_symbol(const std::string &name, value_type value, enum_type type)
    {
        auto &ent = entries[static_cast<std::size_t>(type)];
        auto it = ent.table.emplace(name, std::move(value));
        ent.index_list.push_back(it);
        return it->second;
    }

    void drop_symbols(std::size_t n, enum_type type)
    {
        auto &ent = entries[static_cast<std::size_t>(type)];
        if(n > ent.index_list.size())
            n = ent.index_list.size();

        for(; n; --n) {
            ent.table.erase(ent.index_list.back());
            ent.index_list.pop_back();
        }
    }

    std::size_t num_symbols(enum_type type) const
    {
        return entries[static_cast<std::size_t>(type)].index_list.size();
    }
};

} // namespace compiler

} // namespace emel
