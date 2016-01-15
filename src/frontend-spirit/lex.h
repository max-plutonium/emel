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

#include "../emel/tokens.h"

#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

namespace emel { namespace spirit_frontend {

using source_iter = std::string::const_iterator;
using pos_iter = boost::spirit::classic::position_iterator<source_iter>;
namespace lex = boost::spirit::lex;
using boost::spirit::utf8_char;

using token_type = lex::lexertl::token<
    pos_iter, boost::mpl::vector<std::string, double, bool>,
        boost::mpl::false_, token>;

using lexer_type = lex::lexertl::actor_lexer<token_type>;

class lexer : public lex::lexer<lexer_type>
{
  template <typename Tp>
    using token_def = lex::token_def<Tp, utf8_char, token>;
    using keyword = lex::token_def<lex::omit>;

    lex::token_def<lex::omit> ws, comment;

public:
    token_def<std::string> id;
    token_def<double> number;
    token_def<std::string> text;
    token_def<bool> true_, false_;

    keyword class_, end_class,
        while_, end_while,
        for_, end_for,
        if_, else_, end_if,
        switch_, case_, default_, end_switch,
        continue_, break_, return_,
        try_, catch_,
        byref, byval, as_external, ref_of, val_of,
        colon, comma, dot, semicolon,
        assign, q_sign, or_, xor_, and_, not_,
        eq, ne, lt, gt, lte, gte,
        plus, minus, mul, div,
        left_paren, right_paren,
        left_brace, right_brace,
        left_bracket, right_bracket;

public:
    lexer();
};

} // namespace spirit_frontend

} // namespace emel
