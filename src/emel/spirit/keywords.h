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
#ifndef KEYWORDS_H
#define KEYWORDS_H

#include "skipper.h"

#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/qi.hpp>

namespace emel { namespace spirit_frontend {

using source_iter = std::string::const_iterator;
using pos_iter = boost::spirit::classic::position_iterator<source_iter>;
namespace qi = boost::spirit::qi;

class keywords
{
    using iterator_type = pos_iter;

protected:
    qi::rule<iterator_type, skipper> end, true_, false_,
        class_, end_class,
        while_, end_while,
        for_, end_for,
        if_, else_, end_if,
        switch_, case_, default_, end_switch,
        continue_, break_, return_,
        try_, catch_,
        byref, byval, as_external, ref_of, val_of,
        colon, dbl_colon, comma, dot, semicolon,
        assign, q_sign, or_, xor_, and_, not_,
        eq, ne, lt, gt, lte, gte,
        plus, minus, mul, div,
        left_paren, right_paren,
        left_brace, right_brace,
        left_bracket, right_bracket;

public:
    keywords();
};

} // namespace spirit_frontend

} // namespace emel

#endif // KEYWORDS_H
