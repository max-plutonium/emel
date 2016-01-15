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

#include "expressions.h"

namespace emel { namespace spirit_frontend {

class grammar : public qi::grammar<pos_iter, ast::node(), skipper>
    , virtual public keywords, virtual public values, virtual public expressions
{
    using iterator_type = pos_iter;

    qi::rule<iterator_type, ast::node(), skipper> root;
    qi::rule<iterator_type, ast::class_(), skipper> class_def;
    qi::rule<iterator_type, ast::param(), skipper> param_def;
    qi::rule<iterator_type, ast::method(), skipper> method_def;
    qi::rule<iterator_type, std::vector<ast::node>(), skipper> exprs;
    qi::rule<iterator_type, ast::node(), skipper> block_expr, branch_expr, line_expr;
    qi::rule<iterator_type, ast::try_(), skipper> try_block;
    qi::rule<iterator_type, ast::case_(), skipper> case_branch;
    qi::rule<iterator_type, ast::switch_(), skipper> switch_branch;
    qi::rule<iterator_type, ast::if_(), skipper> if_branch;
    qi::rule<iterator_type, ast::for_(), skipper> for_loop;
    qi::rule<iterator_type, ast::while_(), skipper> while_loop;
    qi::rule<iterator_type, ast::continue_(), skipper> continue_branch;
    qi::rule<iterator_type, ast::break_(), skipper> break_branch;
    qi::rule<iterator_type, ast::return_(), skipper> return_branch;

public:
    grammar();
};

} // namespace spirit_frontend

} // namespace emel
