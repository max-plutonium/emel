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
#ifndef SPIRIT_PARSER_H
#define SPIRIT_PARSER_H

#include "../emel/parser.h"

namespace emel { namespace spirit_frontend {

class spirit_parser : public parser
{
public:
    virtual bool parse(source_iter first, source_iter last,
        std::string file_name, ast::node &ret) const override;
};

} // namespace spirit_frontend

} // namespace emel

#endif // SPIRIT_PARSER_H
