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
#include "parser.h"
#include "plugins.h"

#include <mutex>
#include <iostream>

namespace emel {

static std::once_flag once_flag;

/*static*/
parser *parser::instance(const std::string &name)
{
    std::call_once(once_flag, &plugin::load_dir, &frontend, "");

    if(name.empty()) {
        auto versions = frontend.versions();
        if(versions.empty())
            return nullptr;
        return frontend.load_version(versions.back()).second;
    }

    return frontend.load_name(name).second;
}

bool parser::parse_string(const std::string &content, ast::node &ret) const
{
    if(content.empty()) return false;
    return parse(content.cbegin(), content.cend(), "fake.emel", ret);
}

std::vector<ast::node> parser::parse_dir(const std::string &dir_name)
{
    ast::node node;
    std::vector<ast::node> ret;

    loader.scan_dir(dir_name);
    const auto class_names = loader.names();

    for(auto &name : class_names) {
        const auto content = loader.read_source(name);
        if(!parse(content.cbegin(), content.cend(),
                  loader.get_path_for(name), node))
            std::cerr << "Parsing class " << name << " failed" << std::endl;
        else
            ret.push_back(std::move(node));
    }

    return ret;
}

} // namespace emel
