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

#include "../emel/ast.h"
#include "keywords.h"
#include "values.h"
#include "error-handler.h"
#include "position-handler.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/function.hpp>

namespace emel { namespace spirit_frontend {

class expressions : virtual public keywords, virtual public values
{
    using iterator_type = pos_iter;

protected:
    qi::rule<iterator_type, ast::node(), skipper> expr;
    qi::rule<iterator_type, ast::assign(), skipper> assignment;
    qi::rule<iterator_type, ast::node(), qi::locals<ast::node>, skipper> ternary;
    qi::rule<iterator_type, ast::node(), qi::locals<op_kind>, skipper> log_or,
            log_xor, log_and, rel_eq, rel_order, addition, multiplication;

    qi::rule<iterator_type, ast::node(), skipper> factor;
    qi::rule<iterator_type, ast::variable(), skipper> var_ref;
    qi::rule<iterator_type, ast::un_op(), skipper> unary;
    qi::rule<iterator_type, ast::call(), skipper> call;

    boost::phoenix::function<position_handler> ph;
    boost::phoenix::function<error_handler> eh;

public:
    expressions();
};

} // namespace spirit_frontend

} // namespace emel
