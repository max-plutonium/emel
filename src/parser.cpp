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

#include <FlexLexer.h>
#include <functional>

#define YYSTYPE emel::ast::node
#include "gnu/gnu_parser.hpp"

#include "spirit/grammar.h"

#include <fstream>
#include <boost/spirit/home/qi/parse.hpp>

emel::ast::node *yylval = 0;
yy::location *yylloc = 0;
std::string *current_file = 0;
emel::ast::node root_nodes;

namespace emel {

class spirit_parser : public parser
{
public:
    virtual bool parse(source_iter first, source_iter last,
        std::string file_name, ast::node &ret) override;
};

class gnu_parser : public parser, public yy::parser
{
    using yylex_type = std::function<int (emel::ast::node *, yy::location *)>;

    yyFlexLexer lexer;
    yylex_type parse_func = [this](ast::node *n, yy::location *loc) {
        yylval = n;
        yylloc = loc;
        return lexer.yylex();
    };

public:
    gnu_parser() : yy::parser(parse_func) { }

    virtual bool parse(source_iter first, source_iter last,
        std::string file_name, ast::node &ret) override;

    // parser interface
private:
    virtual void error(const location_type &loc, const std::string &msg)
    {
        std::cerr << *loc.begin.filename << ":"
                  << loc.begin.line << ":" << loc.begin.column
                  << " : " << msg << std::endl;
    }
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
    return std::unique_ptr<parser> { new spirit_parser };
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

bool gnu_parser::parse(source_iter first, source_iter last,
    std::string file_name, ast::node &ret)
{
    std::string str(first, last);
    std::istringstream is(str);
    std::ostringstream os;
    lexer.switch_streams(&is, &os);

//    set_debug_level(3);

    current_file = &file_name;
    const bool res = !yy::parser::parse();
    yylval = 0;
    yylloc = 0;
    current_file = 0;
    ret = std::move(root_nodes);
    return res;
}

} // namespace emel
