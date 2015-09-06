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

#include <iostream>
#include <fstream>
#include <mutex>

namespace emel {

/*static*/
std::string parser::read_from_file(const std::string &file_name)
{
    std::ifstream is(file_name.c_str());

    if(!is.is_open()) {
        std::cerr << "Could not open file: " << file_name << std::endl;
        return std::string();
    }

    is.unsetf(std::ios::skipws);

    std::string ret {
        std::istreambuf_iterator<char>(is.rdbuf()),
        std::istreambuf_iterator<char>() };

    is.close();
    return ret;
}

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

bool parser::parse_file(const std::string &file_name, ast::node &ret) const
{
    const std::string content = read_from_file(file_name);
    if(content.empty()) return false;
    return parse(content.cbegin(), content.cend(), file_name, ret);
}

bool parser::parse_string(const std::string &content, ast::node &ret) const
{
    if(content.empty()) return false;
    return parse(content.cbegin(), content.cend(), "fake.emel", ret);
}

} // namespace emel
