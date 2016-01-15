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

#include "ast.h"
#include "source-loader.h"

namespace emel EMEL_EXPORT {

using source_iter = std::string::const_iterator;

class parser
{
    source_loader loader;

public:
    virtual ~parser() = default;
    static parser *instance(const std::string &name = std::string());
    bool parse_string(const std::string &content, ast::node &ret) const;

    virtual bool parse(source_iter first, source_iter last,
        const std::string &file_name, ast::node &ret) const = 0;

    std::vector<ast::node> parse_dir(const std::string &dir_name = std::string());
};

} // namespace emel
