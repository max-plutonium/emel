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

#include "parser.h"

namespace emel EMEL_EXPORT {

class plugin
{
public:
    struct version {
        std::size_t major, minor, patch;
    };

protected:
    const char *type_name;
    std::string default_dir;

public:
    plugin(const char *type_name, const std::string &default_dir = std::string());

    std::size_t load_dir(const std::string &dir_name = std::string());

    std::pair<version, void *> load_name(const std::string &name) const;
    std::size_t names_count(const std::string &name) const;
    std::vector<std::string> names() const;

    std::pair<std::string, void *> load_version(version ver) const;
    std::size_t versions_count(version ver) const;
    std::vector<version> versions() const;
};

template <typename Tp>
class typed_plugin : public plugin
{
public:
    typed_plugin(const char *type_name, const std::string &default_dir = std::string())
        : plugin(type_name, default_dir)
    { }

    std::pair<version, Tp *> load_name(const std::string &name) const
    {
        auto p = plugin::load_name(name);
        return std::make_pair(std::move(p.first), reinterpret_cast<Tp *>(p.second));
    }

    std::pair<std::string, Tp *> load_version(version ver) const
    {
        auto p = plugin::load_version(ver);
        return std::make_pair(std::move(p.first), reinterpret_cast<Tp *>(p.second));
    }
};

extern typed_plugin<parser> frontend;

} // namespace emel
