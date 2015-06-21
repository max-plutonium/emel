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
#include "values.h"

#include <boost/phoenix/phoenix.hpp>
#include <boost/lexical_cast.hpp>

namespace emel { namespace spirit_frontend {

static double get_double(pos_iter f, pos_iter l) {
    return boost::lexical_cast<double>(std::string(f, l));
}

values::values()
{
    using qi::_val;
    using qi::_1;
    using qi::_2;
    using qi::_3;
    using qi::_4;
    using boost::phoenix::begin;
    using boost::phoenix::end;

    value %= text_value | number_value | bool_value;

    value.name("value");

    id = qi::lexeme[ (qi::alpha | '_') >> *(qi::alnum | '_') ];

    id.name("id");

    escaped = '\\' >> qi::char_("\"");
    text_content = *(escaped | ~qi::char_('"'));
    text_value = '"' > text_content > '"';

    text_value.name("text");

    number_value = ("0x" >> qi::hex)[ _val = _1 ]
        | qi::raw[ qi::lexeme[ -(qi::char_('+') | qi::char_('-'))
            >> (+qi::digit >> -(qi::char_('.') >> *qi::digit)) ] ]
            [ _val = boost::phoenix::bind(get_double, begin(_1), end(_1)) ];

    number_value.name("number");

    bool_value %= true_[ _val = true ] | false_[ _val = false ];

    bool_value.name("boolean");
}

} // namespace spirit_frontend

} // namespace emel
