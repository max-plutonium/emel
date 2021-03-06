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

#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/qi.hpp>

namespace emel { namespace spirit_frontend {

using source_iter = std::string::const_iterator;
using pos_iter = boost::spirit::classic::position_iterator<source_iter>;
namespace qi = boost::spirit::qi;

class skipper : public qi::grammar<pos_iter>
{
    using iterator_type = pos_iter;
    qi::rule<iterator_type> root;
    qi::rule<iterator_type> block_comment, line_comment, emel_comment;

public:
    skipper();
};

} // namespace spirit_frontend

} // namespace emel
