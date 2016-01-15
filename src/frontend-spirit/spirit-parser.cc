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
#include "spirit-parser.h"
#include "grammar.h"

namespace emel { namespace spirit_frontend {

bool spirit_parser::parse(source_iter first, source_iter last,
    const std::string &file_name, ast::node &ret) const
{
    pos_iter pos_begin(first, last, file_name);
    pos_iter pos_end;
    grammar g;
    skipper s;

    bool r = false;

    try {
        r = qi::phrase_parse(pos_begin, pos_end, g, s, ret);

    } catch(const std::runtime_error &e) {
//        std::string rest(pos_begin, pos_end);
// TODO        std::cerr << "Parsing failed\n" << "stopped at: \""
//                  << rest << "\"\n";
        return false;
    }

    if(pos_begin != pos_end) {
//        std::string rest(pos_begin, pos_end);
// TODO        std::cerr << "Parsing failed\n" << "stopped at: \""
//                  << rest << "\"\n";
        return false;
    }

    return r;
}

} // namespace spirit_frontend

} // namespace emel

#include <boost/dll/alias.hpp>

static const char *s_type = "frontend";
static const char *s_name = "spirit";
static const char *s_version = EMEL_FRONTEND_VERSION;

static emel::parser *emel_spirit_frontend_instance() {
    static emel::spirit_frontend::spirit_parser instance;
    return &instance;
}

BOOST_DLL_ALIAS(s_type, emel_plugin_type)
BOOST_DLL_ALIAS(s_name, emel_plugin_name)
BOOST_DLL_ALIAS(s_version, emel_plugin_version)
BOOST_DLL_ALIAS(emel_spirit_frontend_instance, emel_plugin_instance)
