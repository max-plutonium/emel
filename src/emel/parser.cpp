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
#include "spirit/grammar.h"

#include <functional>
#include <fstream>
#include <boost/spirit/home/qi/parse.hpp>

namespace emel {

class spirit_parser : public parser
{
public:
    virtual bool parse(source_iter first, source_iter last,
        std::string file_name, ast::node &ret) override;
};

/*static*/
std::string parser::read_from_file(const std::string &file_name)
{
    std::ifstream is(file_name.c_str());

    if(!is.is_open()) {
        std::cerr << "Could not open file: " << file_name << std::endl;
        return std::string();
    }

    is.unsetf(std::ios::skipws);
    return std::string(
            std::istreambuf_iterator<char>(is.rdbuf()),
            std::istreambuf_iterator<char>()
        );
}

/*static*/
std::unique_ptr<parser> parser::create()
{
    return std::make_unique<spirit_parser>();
}

bool parser::parse_file(const std::string &file_name, ast::node &ret)
{
    const std::string content = read_from_file(file_name);
    if(content.empty()) return false;
    return parse(content.cbegin(), content.cend(), file_name, ret);
}

bool parser::parse_string(const std::string &content, ast::node &ret)
{
    if(content.empty()) return false;
    return parse(content.cbegin(), content.cend(), "fake.emel", ret);
}

bool spirit_parser::parse(source_iter first, source_iter last,
    std::string file_name, ast::node &ret)
{
    spirit_frontend::pos_iter pos_begin(first, last, file_name);
    spirit_frontend::pos_iter pos_end;
    spirit_frontend::grammar g;
    spirit_frontend::skipper s;

    bool r = false;

    try {
        r = spirit_frontend::qi::phrase_parse(pos_begin, pos_end, g, s, ret);

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

} // namespace emel
