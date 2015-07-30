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
#include "spirit_parser.h"
#include "grammar.h"

namespace emel { namespace spirit_frontend {

bool spirit_parser::parse(source_iter first, source_iter last,
    std::string file_name, ast::node &ret) const
{
    pos_iter pos_begin(first, last, file_name);
    pos_iter pos_end;
    grammar g;
    skipper s;

    bool r = false;

    try {
        r = qi::phrase_parse(pos_begin, pos_end, g, s, ret);

    } catch(const std::runtime_error &e) {
        std::string rest(pos_begin, pos_end);
// TODO        std::cerr << "Parsing failed\n" << "stopped at: \""
//                  << rest << "\"\n";
        return false;
    }

    if(pos_begin != pos_end) {
        std::string rest(pos_begin, pos_end);
// TODO        std::cerr << "Parsing failed\n" << "stopped at: \""
//                  << rest << "\"\n";
        return false;
    }

    return r;
}

} // namespace spirit_frontend

} // namespace emel

static emel::spirit_frontend::spirit_parser g_instance;

extern "C" {

EMEL_EXPORT emel::parser *emel_frontend_instance()
{
    return &g_instance;
}

EMEL_EXPORT const char *emel_frontend_name()
{
    return "spirit";
}

EMEL_EXPORT const char *emel_frontend_version()
{
    return EMEL_FRONTEND_VERSION;
}

} // extern "C"
